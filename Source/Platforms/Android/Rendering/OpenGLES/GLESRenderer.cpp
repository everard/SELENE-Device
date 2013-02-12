// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLESRenderer.h"

#include "../../../../Engine/Scene/Nodes/ParticleSystem.h"
#include "../../../../Engine/Scene/Nodes/Camera.h"
#include "../../../../Engine/Scene/Nodes/Actor.h"
#include "../../Application/AndroidApplication.h"
#include "../../../../Engine/GUI/GUI.h"
#include "../../Platform.h"

#include "Resources/GLESTexture.h"
#include "Resources/GLESMesh.h"

namespace selene
{

        //--------------------------------------------------------------------------------------------
        bool GlesRenderer::initialize(const Renderer::Parameters& parameters)
        {
                parameters_ = parameters;
                writeLogEntry("--- Initializing OpenGL ES renderer ---");

                // initialize helpers
                if(!actorsRenderer_.initialize(frameParameters_, textureHandler_, capabilities_))
                {
                        writeLogEntry("ERROR: Could not initialize texture handler.");
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

                // prepare OpenGL ES
                glEnable(GL_CULL_FACE);
                glEnable(GL_DEPTH_TEST);
                CHECK_GLES_ERROR("glEnable");

                return true;
        }

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::destroy()
        {
                actorsRenderer_.destroy();
                textureHandler_.destroy();
                guiRenderer_.destroy();

                capabilities_.destroyContext();
        }

        //--------------------------------------------------------------------------------------------
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
                const Matrix& projectionInvMatrix = camera.getProjectionInvMatrix();
                frameParameters_.projectionParameters = camera.getProjectionParameters();
                frameParameters_.unprojectionVector.define(projectionInvMatrix.a[0][0],
                                                           projectionInvMatrix.a[1][1],
                                                           1.0, 0.0);

                frameParameters_.bloomParameters.define(0.08f, 0.18f, 0.64f, 1.5f);
                frameParameters_.ssaoParameters.define(2.5f, -0.2f, 1.0f, 1.0f);
                frameParameters_.edgeDetectionParameters.define(2.5f, 0.99f, 0.0f, 0.0f);

                // set viewport
                glViewport(0, 0, parameters_.getWidth(), parameters_.getHeight());

                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glEnable(GL_DEPTH_TEST);

                CHECK_GLES_ERROR("BEFORE START RENDER GlesRenderer::render");

                actorsRenderer_.renderPositionsAndNormals(renderingData.getActorNode());

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

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::writeLogEntry(const char* entry)
        {
                LOGI("%s", entry);
                if(parameters_.getLog() != nullptr)
                        (*parameters_.getLog()) << entry << std::endl;
        }

        //--------------------------------------------------------------------------------------------
        bool GlesRenderer::retain()
        {
                if(!capabilities_.createCompatibleContext(parameters_))
                        return false;

                if(!actorsRenderer_.initialize(frameParameters_, textureHandler_, capabilities_))
                        return false;

                if(!textureHandler_.initialize())
                        return false;

                if(!guiRenderer_.retain())
                        return false;

                if(!ResourceManager::retainResources())
                        return false;

                return true;
        }

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::discard()
        {
                ResourceManager::discardResources();

                actorsRenderer_.destroy();
                textureHandler_.destroy();
                guiRenderer_.discard();

                capabilities_.destroyContext();
        }

}
