// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLESBloomRenderer.h"

#include "../../../Platform.h"
#include "../GLESRenderer.h"

namespace selene
{

        GlesBloomRenderer::GlesBloomRenderer()
        {
                renderTargetContainer_ = nullptr;
                frameParameters_ = nullptr;
                fullScreenQuad_ = nullptr;
                textureHandler_ = nullptr;
        }
        GlesBloomRenderer::~GlesBloomRenderer()
        {
                destroy();
        }

        //-----------------------------------------------------------------------------------------------------------
        bool GlesBloomRenderer::initialize(GlesRenderTargetContainer& renderTargetContainer,
                                           GlesFrameParameters& frameParameters,
                                           GlesFullScreenQuad& fullScreenQuad,
                                           GlesTextureHandler& textureHandler)
        {
                destroy();

                renderTargetContainer_ = &renderTargetContainer;
                frameParameters_ = &frameParameters;
                fullScreenQuad_ = &fullScreenQuad;
                textureHandler_ = &textureHandler;

                // bright pass shaders
                static const char* vertexShaderBrightPass =
                        "attribute vec4 vertexPosition;\n"
                        "uniform vec4 textureCoordinatesAdjustment;\n"
                        "uniform vec4 imageScale;\n"
                        "varying vec2 textureCoordinates;\n"
                        "void main()\n"
                        "{\n"
                        "        float offset = 1.0 - imageScale.x;"
                        "        vec2 position = vertexPosition.xy * imageScale.x - vec2(offset, offset);\n"
                        "        gl_Position = vec4(position.x, position.y, 0.0, 1.0);\n"
                        "        textureCoordinates = vertexPosition.zw * textureCoordinatesAdjustment.xy;\n"
                        "}\n";

                static const char* fragmentShaderBrightPass =
                        "precision highp float;\n"
                        "varying vec2 textureCoordinates;\n"
                        "uniform sampler2D inputImage;\n"
                        "uniform vec4 bloomParameters;\n"
                        "void main()\n"
                        "{\n"
                        "        vec4 color = texture2D(inputImage, textureCoordinates);\n"
                        "        color *= bloomParameters.z / (bloomParameters.x + 0.001);\n"
                        "        color *= (1.0 + (color / bloomParameters.w));\n"
                        "        color -= 5.0;\n"
                        "        color = max(color, 0.0);\n"
                        "        color /= (10.0 + color);\n"
                        "        gl_FragColor = color;\n"
                        "}\n";

                // bloom pass shaders
                static const char* vertexShaderBloomPass =
                        "attribute vec4 vertexPosition;\n"
                        "uniform vec4 textureCoordinatesAdjustment;\n"
                        "uniform vec4 imageScale;\n"
                        "varying vec2 textureCoordinates;\n"
                        "void main()\n"
                        "{\n"
                        "        float offset = 1.0 - imageScale.x;"
                        "        vec2 position = vertexPosition.xy * imageScale.x - vec2(offset, offset);\n"
                        "        gl_Position = vec4(position.x, position.y, 0.0, 1.0);\n"
                        "        textureCoordinates = vertexPosition.zw * textureCoordinatesAdjustment.xy *\n"
                        "                             imageScale.x;\n"
                        "}\n";

                static const char* fragmentShaderBloomPass =
                        "precision highp float;\n"
                        "varying vec2 textureCoordinates;\n"
                        "uniform sampler2D inputImage;\n"
                        "uniform vec4 bloomParameters;\n"
                        "uniform vec4 blurOffsets[9];\n"
                        "void main()\n"
                        "{\n"
                        "        float weights[9];\n"
                        "        weights[0] = weights[8] = 0.05;\n"
                        "        weights[1] = weights[7] = 0.09;\n"
                        "        weights[2] = weights[6] = 0.13;\n"
                        "        weights[3] = weights[5] = 0.15;\n"
                        "        weights[4] = 0.16;\n"
                        "        vec4 color = vec4(0.0, 0.0, 0.0, 1.0);\n"
                        "        for(int i = 0; i < 9; ++i)\n"
                        "                color += weights[i] * texture2D(inputImage,\n"
                        "                                                textureCoordinates + blurOffsets[i].xy);\n"
                        "        gl_FragColor = color * bloomParameters.y;\n"
                        "}\n";

                // combine pass shaders
                static const char* vertexShaderCombinePass =
                        "attribute vec4 vertexPosition;\n"
                        "uniform vec4 textureCoordinatesAdjustment;\n"
                        "uniform vec4 imageScale;\n"
                        "varying vec2 textureCoordinates;\n"
                        "varying vec2 originalTextureCoordinates;\n"
                        "void main()\n"
                        "{"
                        "        gl_Position = vec4(vertexPosition.x, vertexPosition.y, 0.0, 1.0);\n"
                        "        textureCoordinates = vertexPosition.zw *\n"
                        "                             textureCoordinatesAdjustment.xy * imageScale.x;\n"
                        "        originalTextureCoordinates = vertexPosition.zw * textureCoordinatesAdjustment.xy;\n"
                        "}\n";

                static const char* fragmentShaderCombinePass =
                        "precision mediump float;\n"
                        "varying vec2 textureCoordinates;\n"
                        "varying vec2 originalTextureCoordinates;\n"
                        "uniform sampler2D inputImage;\n"
                        "uniform sampler2D combinedImage;\n"
                        "void main()\n"
                        "{\n"
                        "        gl_FragColor = texture2D(inputImage, textureCoordinates) +\n"
                        "                       texture2D(combinedImage, originalTextureCoordinates);\n"
                        "}\n";

                // load GLSL programs
                GlesGlslProgram::VertexAttribute vertexAttributes[] =
                {
                        GlesGlslProgram::VertexAttribute("vertexPosition", LOCATION_ATTRIBUTE_POSITION)
                };
                const uint8_t numVertexAttributes = sizeof(vertexAttributes) / sizeof(vertexAttributes[0]);

                static const char* vertexShaderSources[NUM_OF_GLSL_PROGRAMS] =
                {
                        vertexShaderBrightPass,
                        vertexShaderBloomPass,
                        vertexShaderCombinePass
                };

                static const char* fragmentShaderSources[NUM_OF_GLSL_PROGRAMS] =
                {
                        fragmentShaderBrightPass,
                        fragmentShaderBloomPass,
                        fragmentShaderCombinePass
                };

                for(uint8_t i = 0; i < NUM_OF_GLSL_PROGRAMS; ++i)
                {
                        if(!programs_[i].initialize(vertexShaderSources[i], fragmentShaderSources[i],
                                                    vertexAttributes, numVertexAttributes))
                        {
                                destroy();
                                return false;
                        }

                        variables_[i].obtainLocations(programs_[i]);
                }

                return true;
        }

        //-----------------------------------------------------------------------------------------------------------
        void GlesBloomRenderer::destroy()
        {
                for(uint32_t i = 0; i < NUM_OF_GLSL_PROGRAMS; ++i)
                        programs_[i].destroy();

                renderTargetContainer_ = nullptr;
                frameParameters_ = nullptr;
                fullScreenQuad_ = nullptr;
                textureHandler_ = nullptr;
        }

        //-----------------------------------------------------------------------------------------------------------
        void GlesBloomRenderer::renderBloom()
        {
                Vector4d imageScale(0.25f);
                Vector4d blurOffsets[2][NUM_OF_BLUR_OFFSETS];
                const uint8_t shift = static_cast<uint8_t>(NUM_OF_BLUR_OFFSETS * 0.5);
                float kernelSize = 1.0f / frameParameters_->renderTargetSize.x;
                for(uint8_t i = 0; i < 2; ++i)
                {
                        for(int8_t j = 0; j < NUM_OF_BLUR_OFFSETS; ++j)
                        {
                                static_cast<float*>(blurOffsets[i][j])[i] =
                                        static_cast<float>(j - shift) * kernelSize;
                        }
                }

                GLsizei width  = static_cast<GLsizei>(frameParameters_->screenSize.x);
                GLsizei height = static_cast<GLsizei>(frameParameters_->screenSize.y);

                glViewport(0, 0, width, height);

                glDisable(GL_DEPTH_TEST);
                CHECK_GLES_ERROR("GlesBloomRenderer::renderBloom: glDisable(GL_DEPTH_TEST)");

                glDisable(GL_CULL_FACE);
                CHECK_GLES_ERROR("GlesBloomRenderer::renderBloom: glDisable(GL_CULL_FACE)");

                // bright pass
                if(!renderTargetContainer_->setRenderTarget(RENDER_TARGET_HELPER_0))
                {
                        LOGI("****************************** FAILED: GlesBloomRenderer::renderBloom:"
                             "setRenderTarget(RENDER_TARGET_HELPER_0)");
                        return;
                }

                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting: glClear");

                programs_[GLSL_PROGRAM_BRIGHT_PASS].set();
                Variables* variables = &variables_[GLSL_PROGRAM_BRIGHT_PASS];

                glUniform1i(variables->locationInputImage, 0);
                CHECK_GLES_ERROR("GlesBloomRenderer::renderBloom: glUniform1i(locationInputImage)");

                textureHandler_->setTexture(renderTargetContainer_->getRenderTarget(RENDER_TARGET_RESULT), 0);

                glUniform4fv(variables->locationTextureCoordinatesAdjustment, 1,
                             static_cast<const float*>(frameParameters_->textureCoordinatesAdjustment));
                glUniform4fv(variables->locationBloomParameters, 1,
                             static_cast<const float*>(frameParameters_->bloomParameters));
                glUniform4fv(variables->locationImageScale, 1, static_cast<const float*>(imageScale));
                CHECK_GLES_ERROR("GlesBloomRenderer::renderBloom: glUniform4fv");

                fullScreenQuad_->render();

                // unbind texture
                textureHandler_->setTexture(0, 0);

                // bloom
                programs_[GLSL_PROGRAM_BLOOM_PASS].set();
                variables = &variables_[GLSL_PROGRAM_BLOOM_PASS];

                glUniform1i(variables->locationInputImage, 0);
                CHECK_GLES_ERROR("GlesBloomRenderer::renderBloom: glUniform1i(locationInputImage)");

                glUniform4fv(variables->locationTextureCoordinatesAdjustment, 1,
                             static_cast<const float*>(frameParameters_->textureCoordinatesAdjustment));
                glUniform4fv(variables->locationBloomParameters, 1,
                             static_cast<const float*>(frameParameters_->bloomParameters));
                glUniform4fv(variables->locationImageScale, 1, static_cast<const float*>(imageScale));
                CHECK_GLES_ERROR("GlesBloomRenderer::renderBloom: glUniform4fv");

                for(uint8_t i = 0; i < 2; ++i)
                {
                        uint8_t sourceRenderTarget = RENDER_TARGET_HELPER_0 + i;
                        uint8_t resultRenderTarget = RENDER_TARGET_HELPER_1 - i;

                        if(!renderTargetContainer_->setRenderTarget(resultRenderTarget))
                        {
                                LOGI("****************************** FAILED: GlesBloomRenderer::renderBloom:"
                                     "setRenderTarget(resultRenderTarget)");
                                return;
                        }

                        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                        glClear(GL_COLOR_BUFFER_BIT);
                        CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting: glClear");

                        textureHandler_->setTexture(renderTargetContainer_->getRenderTarget(sourceRenderTarget), 0);

                        glUniform4fv(variables->locationBlurOffsets, NUM_OF_BLUR_OFFSETS,
                                     reinterpret_cast<const float*>(blurOffsets[i]));
                        CHECK_GLES_ERROR("GlesBloomRenderer::renderBloom: glUniform4fv");

                        fullScreenQuad_->render();

                        // unbind texture
                        textureHandler_->setTexture(0, 0);
                }

                // combine pass
                if(!renderTargetContainer_->setRenderTarget(RENDER_TARGET_HELPER_1))
                {
                        LOGI("****************************** FAILED: GlesBloomRenderer::renderBloom:"
                             "setRenderTarget(RENDER_TARGET_HELPER_1)");
                        return;
                }

                programs_[GLSL_PROGRAM_COMBINE_PASS].set();
                variables = &variables_[GLSL_PROGRAM_COMBINE_PASS];

                glUniform1i(variables->locationInputImage,    0);
                glUniform1i(variables->locationCombinedImage, 1);
                CHECK_GLES_ERROR("GlesBloomRenderer::renderBloom: glUniform1i");

                glUniform4fv(variables->locationTextureCoordinatesAdjustment, 1,
                             static_cast<const float*>(frameParameters_->textureCoordinatesAdjustment));
                glUniform4fv(variables->locationImageScale, 1, static_cast<const float*>(imageScale));
                CHECK_GLES_ERROR("GlesBloomRenderer::renderBloom: glUniform4fv");

                textureHandler_->setTexture(renderTargetContainer_->getRenderTarget(RENDER_TARGET_HELPER_0), 0);
                textureHandler_->setTexture(renderTargetContainer_->getRenderTarget(RENDER_TARGET_RESULT), 1);

                fullScreenQuad_->render();

                // unbind textures
                textureHandler_->setTexture(0, 0);
                textureHandler_->setTexture(0, 1);
        }

        GlesBloomRenderer::Variables::Variables()
        {
                locationInputImage    = -1;
                locationCombinedImage = -1;

                locationTextureCoordinatesAdjustment  = -1;
                locationBloomParameters = -1;
                locationBlurOffsets = -1;
                locationImageScale = -1;
        }
        GlesBloomRenderer::Variables::~Variables() {}

        //-----------------------------------------------------------------------------------------------------------
        void GlesBloomRenderer::Variables::obtainLocations(GlesGlslProgram& program)
        {
                locationInputImage    = program.getUniformLocation("inputImage");
                locationCombinedImage = program.getUniformLocation("combinedImage");

                locationTextureCoordinatesAdjustment  = program.getUniformLocation("textureCoordinatesAdjustment");
                locationBloomParameters               = program.getUniformLocation("bloomParameters");
                locationBlurOffsets                   = program.getUniformLocation("blurOffsets");
                locationImageScale                    = program.getUniformLocation("imageScale");
        }

}
