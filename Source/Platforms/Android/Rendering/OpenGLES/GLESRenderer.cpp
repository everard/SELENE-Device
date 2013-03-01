// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLESRenderer.h"

#include "../../../../Engine/Scene/Nodes/ParticleSystem.h"
#include "../../../../Engine/Scene/Nodes/Camera.h"
#include "../../../../Engine/Scene/Nodes/Actor.h"
#include "../../../../Engine/GUI/GUI.h"

#include "../../Application/AndroidApplication.h"
#include "../../Platform.h"

#include "Resources/GLESTexture.h"
#include "Resources/GLESMesh.h"

namespace selene
{

        //-------------------------------------------------------------------
        bool GlesRenderer::initialize(const Renderer::Parameters& parameters)
        {
                parameters_ = parameters;
                writeLogEntry("--- Initializing OpenGL ES renderer ---");

                // initialize helpers
                if(!initializeHelpers())
                        return false;

                // prepare OpenGL ES
                glEnable(GL_CULL_FACE);
                glEnable(GL_DEPTH_TEST);
                CHECK_GLES_ERROR("GlesRenderer::initialize: glEnable");

                return true;
        }

        //-------------------------------------------------------------------
        void GlesRenderer::destroy()
        {
                destroyHelpers();
                capabilities_.destroyContext();
        }

        //-------------------------------------------------------------------
        void GlesRenderer::render(const Camera& camera)
        {
                auto& renderingData = const_cast<Renderer::Data&>(camera.getRenderingData());

                // get matrices
                frameParameters_.viewProjectionMatrix = camera.getViewProjectionMatrix();
                frameParameters_.projectionMatrix = camera.getProjectionMatrix();
                frameParameters_.viewMatrix = camera.getViewMatrix();

                frameParameters_.normalsMatrix = frameParameters_.viewMatrix;
                frameParameters_.normalsMatrix.invert();
                frameParameters_.viewInvMatrix = frameParameters_.normalsMatrix;
                frameParameters_.normalsMatrix.transpose();

                // get vectors
                const auto& projectionInvMatrix = camera.getProjectionInvMatrix();
                frameParameters_.projectionParameters = camera.getProjectionParameters();
                frameParameters_.unprojectionVector.define(projectionInvMatrix.a[0][0],
                                                           projectionInvMatrix.a[1][1],
                                                           1.0f, 0.0);

                frameParameters_.conversionParameters.define(frameParameters_.projectionParameters.w * frameParameters_.projectionParameters.z,
                                                             frameParameters_.projectionParameters.z - frameParameters_.projectionParameters.w,
                                                             frameParameters_.projectionParameters.w, 1.0f);

                frameParameters_.bloomParameters.define(0.08f, 0.18f, 0.64f, 1.5f);
                frameParameters_.ssaoParameters.define(2.5f, -0.2f, 1.0f, 1.0f);
                frameParameters_.edgeDetectionParameters.define(2.5f, 0.99f, 0.0f, 0.0f);

                // set viewport
                glViewport(0, 0, parameters_.getWidth(), parameters_.getHeight());
                glEnable(GL_DEPTH_TEST);

                actorsRenderer_.renderPositionsAndNormals(renderingData.getActorNode());
                lightingRenderer_.renderLighting(renderingData.getLightNode());
                actorsRenderer_.renderShading(renderingData.getActorNode());

                glDisable(GL_DEPTH_TEST);
                glDisable(GL_CULL_FACE);
                CHECK_GLES_ERROR("GlesRenderer::render: glDisable");

                // output result to screen
                renderTargetContainer_.setBackBuffer();
                resultRenderingProgram_.set();

                glUniform4fv(textureCoordinatesAdjustmentLocation_, 1,
                             static_cast<const float*>(frameParameters_.textureCoordinatesAdjustment));
                CHECK_GLES_ERROR("GlesRenderer::render: glUniform4fv");

                glUniform1i(resultTextureLocation_, 0);
                CHECK_GLES_ERROR("GlesRenderer::render: glUniform1i");

                textureHandler_.setTexture(renderTargetContainer_.getRenderTarget(RENDER_TARGET_RESULT), 0);

                fullScreenQuad_.render();

                // unbind texture
                textureHandler_.setTexture(0, 0);

                // render GUI
                Gui* gui = camera.getGui();
                guiRenderer_.renderGui(gui);

                eglSwapBuffers(capabilities_.getDisplay(), capabilities_.getSurface());
        }

        GlesRenderer::GlesRenderer(): parameters_(nullptr, nullptr, 0, 0, nullptr, 0) {}
        GlesRenderer::GlesRenderer(const GlesRenderer&): Renderer(), Status(),
                                                         parameters_(nullptr, nullptr, 0, 0, nullptr, 0) {}
        GlesRenderer::~GlesRenderer()
        {
                destroy();
        }

        //-------------------------------------------------------------------
        void GlesRenderer::writeLogEntry(const char* entry)
        {
                LOGI("****************************** %s", entry);
                if(parameters_.getLog() != nullptr)
                        (*parameters_.getLog()) << entry << std::endl;
        }

        //-------------------------------------------------------------------
        bool GlesRenderer::retain()
        {
                if(!capabilities_.createCompatibleContext(parameters_))
                        return false;

                if(!initialize(parameters_))
                        return false;

                if(!ResourceManager::retainResources())
                        return false;

                return true;
        }

        //-------------------------------------------------------------------
        void GlesRenderer::discard()
        {
                ResourceManager::discardResources();
                destroy();
        }

        //-------------------------------------------------------------------
        bool GlesRenderer::initializeHelpers()
        {
                static const char* resultVertexShader =
                        "attribute vec4 vertexPosition;"
                        "uniform vec4 textureCoordinatesAdjustment;"
                        "varying vec2 textureCoordinates;"
                        "void main()"
                        "{"
                        "        gl_Position = vec4(vertexPosition.x, vertexPosition.y, 0.0, 1.0);"
                        "        textureCoordinates  = vertexPosition.zw * textureCoordinatesAdjustment.xy;"
                        "}\n";

                static const char* resultFragmentShader =
                        "precision mediump float;\n"
                        "varying vec2 textureCoordinates;\n"
                        "uniform sampler2D resultTexture;\n"
                        "void main()\n"
                        "{\n"
                        "        gl_FragColor = texture2D(resultTexture, textureCoordinates);\n"
                        "}\n";

                GlesGlslProgram::VertexAttribute vertexAttributes[] =
                {
                        GlesGlslProgram::VertexAttribute("vertexPosition", 0)
                };
                const uint8_t numVertexAttributes = sizeof(vertexAttributes) / sizeof(vertexAttributes[0]);

                if(!resultRenderingProgram_.initialize(resultVertexShader, resultFragmentShader,
                                                       vertexAttributes, numVertexAttributes))
                {
                        writeLogEntry("ERROR: Could not initialize result GLSL program.");
                        return false;
                }

                textureCoordinatesAdjustmentLocation_ = resultRenderingProgram_.getUniformLocation("textureCoordinatesAdjustment");
                resultTextureLocation_ = resultRenderingProgram_.getUniformLocation("resultTexture");

                if(!renderTargetContainer_.initialize(frameParameters_, parameters_))
                {
                        writeLogEntry("ERROR: Could not initialize render target container.");
                        return false;
                }

                if(!actorsRenderer_.initialize(renderTargetContainer_, frameParameters_, textureHandler_))
                {
                        writeLogEntry("ERROR: Could not initialize actors renderer.");
                        return false;
                }

                if(!lightingRenderer_.initialize(renderTargetContainer_, frameParameters_, actorsRenderer_, textureHandler_))
                {
                        writeLogEntry("ERROR: Could not initialize lighting renderer.");
                        return false;
                }

                if(!fullScreenQuad_.initialize())
                {
                        writeLogEntry("ERROR: Could not initialize full-screen quad.");
                        return false;
                }

                if(!textureHandler_.initialize())
                {
                        writeLogEntry("ERROR: Could not initialize texture handler.");
                        return false;
                }

                if(!guiRenderer_.initialize(textureHandler_, parameters_.getFileManager()))
                {
                        writeLogEntry("ERROR: Could not initialize GUI renderer.");
                        return false;
                }

                return true;
        }

        //-------------------------------------------------------------------
        void GlesRenderer::destroyHelpers()
        {
                resultRenderingProgram_.destroy();
                textureCoordinatesAdjustmentLocation_ = -1;
                resultTextureLocation_ = -1;

                renderTargetContainer_.destroy();
                lightingRenderer_.destroy();
                actorsRenderer_.destroy();
                fullScreenQuad_.destroy();
                textureHandler_.destroy();
                guiRenderer_.destroy();
        }

}
