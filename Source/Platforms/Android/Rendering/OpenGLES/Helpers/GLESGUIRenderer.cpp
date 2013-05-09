// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLESGUIRenderer.h"

#include "../../../../../Engine/Core/Resources/Texture/TextureFactory.h"
#include "../../../../../Engine/GUI/GUI.h"
#include "../../../Platform.h"

namespace selene
{

        GlesGuiRenderer::GlesGuiRenderer()
        {
                fontTextureLocation_ = -1;
        }
        GlesGuiRenderer::~GlesGuiRenderer()
        {
                destroy();
        }

        //---------------------------------------------------------------------------------------------------------------------------------------------------------------------
        bool GlesGuiRenderer::initialize(GlesTextureHandler& textureHandler,
                                         FileManager* fileManager)
        {
                textureHandler_ = &textureHandler;

                TextureFactory<GlesTexture> textureFactory(fileManager);
                fontTexture_.reset(reinterpret_cast<GlesTexture*>(textureFactory.createResource("Fonts/Font.dds")));

                if(!fontTexture_)
                        return false;

                if(!fontTexture_->retain())
                        return false;

                static const char framesRenderingVertexShader[] =
                        "attribute vec4 vertexPosition;"
                        "attribute vec4 vertexColor;"
                        "varying vec4 frameColor;"
                        "void main()"
                        "{"
                        "        gl_Position = vec4(vertexPosition.x - 1.0, 1.0 - vertexPosition.y, 0.0, 1.0);"
                        "        frameColor  = vertexColor;"
                        "}\n";

                static const char framesRenderingFragmentShader[] =
                        "precision mediump float;"
                        "varying vec4 frameColor;"
                        "void main()"
                        "{"
                        "        gl_FragColor = frameColor;"
                        "}\n";

                static const char textRenderingVertexShader[] =
                        "attribute vec4 vertexPosition;"
                        "attribute vec4 vertexColor;"
                        "varying vec4 textColor;"
                        "varying vec2 textureCoordinates;"
                        "void main()"
                        "{"
                        "        gl_Position = vec4(vertexPosition.x - 1.0, 1.0 - vertexPosition.y, 0.0, 1.0);"
                        "        textureCoordinates = vec2(vertexPosition.z, vertexPosition.w);"
                        "        textColor = vertexColor;"
                        "}\n";

                static const char textRenderingFragmentShader[] =
                        "precision mediump float;"
                        "varying vec4 textColor;"
                        "varying vec2 textureCoordinates;"
                        "uniform sampler2D fontTexture;"
                        "void main()"
                        "{"
                        "        vec4 color = textColor * texture2D(fontTexture, textureCoordinates).rrrr;"
                        "        if(color.a > 0.1)"
                        "                gl_FragColor = color;"
                        "        else"
                        "                discard;"
                        "}\n";

                GlesGlslProgram::VertexAttribute vertexAttributes[] =
                {
                        GlesGlslProgram::VertexAttribute("vertexPosition", LOCATION_ATTRIBUTE_POSITION),
                        GlesGlslProgram::VertexAttribute("vertexColor",    LOCATION_ATTRIBUTE_COLOR)
                };
                const uint8_t numVertexAttributes = sizeof(vertexAttributes) / sizeof(vertexAttributes[0]);

                if(!framesRenderingProgram_.initialize(framesRenderingVertexShader, framesRenderingFragmentShader, vertexAttributes, numVertexAttributes))
                        return false;

                if(!textRenderingProgram_.initialize(textRenderingVertexShader, textRenderingFragmentShader, vertexAttributes, numVertexAttributes))
                        return false;

                fontTextureLocation_ = textRenderingProgram_.getUniformLocation("fontTexture");
                CHECK_GLES_ERROR("GlesGuiRenderer::retain: glGetUniformLocation");

                return true;
        }

        //---------------------------------------------------------------------------------------------------------------------------------------------------------------------
        void GlesGuiRenderer::destroy()
        {
                fontTexture_.reset();

                framesRenderingProgram_.destroy();
                textRenderingProgram_.destroy();

                fontTextureLocation_ = -1;
        }

        //---------------------------------------------------------------------------------------------------------------------------------------------------------------------
        void GlesGuiRenderer::renderGui(Gui* gui)
        {
                if(gui == nullptr)
                        return;

                const auto& elements = gui->getElements();

                const uint32_t numFramesPerBatch   = 50;
                const uint32_t numVerticesPerFrame = 6;
                const uint32_t numVectorsPerVertex = 2;
                const float characterSize = 0.0625f;
                static Vector4d frameVertices[numVectorsPerVertex * numFramesPerBatch * numVerticesPerFrame];
                static uint32_t vertexIndices[numVerticesPerFrame] = {0, 2, 1, 2, 3, 1};

                static Vector2d vertexOffsets[] =
                {
                        Vector2d(),
                        Vector2d(),
                        Vector2d(),
                        Vector2d()
                };

                static Vector2d fontOffsets[4] =
                {
                        Vector2d(0.0f, 0.0f),
                        Vector2d(characterSize, 0.0f),
                        Vector2d(0.0f, characterSize),
                        Vector2d(characterSize, characterSize)
                };

                glDisable(GL_DEPTH_TEST);
                CHECK_GLES_ERROR("GlesGuiRenderer::renderGui: glDisable");

                glEnable(GL_BLEND);
                CHECK_GLES_ERROR("GlesGuiRenderer::renderGui: glEnable");

                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                CHECK_GLES_ERROR("GlesGuiRenderer::renderGui: glBlendFunc");

                // render frames
                framesRenderingProgram_.set();
                CHECK_GLES_ERROR("GlesGuiRenderer::renderGui: glUseProgram");

                glEnableVertexAttribArray(LOCATION_ATTRIBUTE_POSITION);
                CHECK_GLES_ERROR("GlesGuiRenderer::renderGui: glEnableVertexAttribArray");

                glEnableVertexAttribArray(LOCATION_ATTRIBUTE_COLOR);
                CHECK_GLES_ERROR("GlesGuiRenderer::renderGui: glEnableVertexAttribArray");

                uint32_t numFrames = 0;
                Vector4d* currentVertex = frameVertices;
                for(auto it = elements.begin(); it != elements.end(); ++it)
                {
                        if(!it->second)
                                continue;

                        auto element = it->second.get();
                        if(element->is(GUI_ELEMENT_HIDDEN))
                                continue;

                        Vector4d color = element->getBackgroundColor(GUI_ELEMENT_COLOR_DEFAULT);

                        if(element->is(GUI_ELEMENT_SELECTED))
                                color = element->getBackgroundColor(GUI_ELEMENT_COLOR_SELECTED);
                        else if(element->is(GUI_ELEMENT_TOUCHED))
                                color = element->getBackgroundColor(GUI_ELEMENT_COLOR_TOUCHED);

                        const auto& size = element->getSize();
                        const auto& position = element->getPosition();

                        vertexOffsets[1] = Vector2d(size.x, 0.0f);
                        vertexOffsets[2] = Vector2d(0.0f, size.y);
                        vertexOffsets[3] = Vector2d(size.x, size.y);

                        for(uint32_t i = 0; i < numVerticesPerFrame; ++i)
                        {
                                currentVertex->define(position + vertexOffsets[vertexIndices[i]], 0.0f, 1.0f);
                                ++currentVertex;
                                *currentVertex = color;
                                ++currentVertex;
                        }

                        ++numFrames;

                        if(numFrames == numFramesPerBatch)
                        {
                                glVertexAttribPointer(LOCATION_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector4d), static_cast<void*>(frameVertices));
                                glVertexAttribPointer(LOCATION_ATTRIBUTE_COLOR,    4, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector4d), static_cast<void*>(frameVertices + 1));
                                glDrawArrays(GL_TRIANGLES, 0, numFrames * numVerticesPerFrame);

                                numFrames = 0;
                                currentVertex = frameVertices;
                        }
                }

                if(numFrames != 0)
                {
                        glVertexAttribPointer(LOCATION_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector4d), static_cast<void*>(frameVertices));
                        glVertexAttribPointer(LOCATION_ATTRIBUTE_COLOR,    4, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector4d), static_cast<void*>(frameVertices + 1));
                        glDrawArrays(GL_TRIANGLES, 0, numFrames * numVerticesPerFrame);
                }

                // render text
                textRenderingProgram_.set();
                CHECK_GLES_ERROR("GlesGuiRenderer::renderGui: glUseProgram");

                glUniform1i(fontTextureLocation_, 0);

                textureHandler_->setTexture(fontTexture_.get(), 0, GlesTextureHandler::DUMMY_TEXTURE_WHITE);
                textureHandler_->setSamplerState(0, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

                numFrames = 0;
                currentVertex = frameVertices;
                for(auto it = elements.begin(); it != elements.end(); ++it)
                {
                        if(!it->second)
                                continue;

                        auto element = it->second.get();
                        if(element->is(GUI_ELEMENT_HIDDEN))
                                continue;

                        const std::string& text = element->getText();
                        if(text.length() == 0)
                                continue;

                        Vector4d textColor = element->getTextColor(GUI_ELEMENT_COLOR_DEFAULT);

                        if(element->is(GUI_ELEMENT_SELECTED))
                                textColor = element->getTextColor(GUI_ELEMENT_COLOR_SELECTED);
                        else if(element->is(GUI_ELEMENT_TOUCHED))
                                textColor = element->getTextColor(GUI_ELEMENT_COLOR_TOUCHED);

                        const auto& fontSize = element->getFontSize();
                        vertexOffsets[1] = Vector2d(fontSize.x, 0.0f);
                        vertexOffsets[2] = Vector2d(0.0f, fontSize.y);
                        vertexOffsets[3] = Vector2d(fontSize.x, fontSize.y);

                        auto position = element->getPosition();
                        float elementRightBoundary = element->getSize().x + position.x;

                        for(size_t i = 0; i < text.length(); ++i)
                        {
                                uint8_t c = text[i];
                                Vector2d textureCoordinates(static_cast<float>(c % 16) * characterSize,
                                                            static_cast<float>(c >> 4) * characterSize);

                                for(uint32_t j = 0; j < numVerticesPerFrame; ++j)
                                {
                                        currentVertex->define(position + vertexOffsets[vertexIndices[j]],
                                                              textureCoordinates + fontOffsets[vertexIndices[j]]);
                                        ++currentVertex;
                                        *currentVertex = textColor;
                                        ++currentVertex;
                                }

                                ++numFrames;

                                if(numFrames == numFramesPerBatch)
                                {
                                        glVertexAttribPointer(LOCATION_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector4d), static_cast<void*>(frameVertices));
                                        glVertexAttribPointer(LOCATION_ATTRIBUTE_COLOR,    4, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector4d), static_cast<void*>(frameVertices + 1));
                                        glDrawArrays(GL_TRIANGLES, 0, numFrames * numVerticesPerFrame);

                                        numFrames = 0;
                                        currentVertex = frameVertices;
                                }

                                position.x += fontSize.x;
                                if(position.x >= elementRightBoundary)
                                        break;
                        }
                }

                if(numFrames != 0)
                {
                        glVertexAttribPointer(LOCATION_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector4d), static_cast<void*>(frameVertices));
                        glVertexAttribPointer(LOCATION_ATTRIBUTE_COLOR,    4, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector4d), static_cast<void*>(frameVertices + 1));
                        glDrawArrays(GL_TRIANGLES, 0, numFrames * numVerticesPerFrame);
                }

                glDisableVertexAttribArray(LOCATION_ATTRIBUTE_POSITION);
                glDisableVertexAttribArray(LOCATION_ATTRIBUTE_COLOR);
                CHECK_GLES_ERROR("GlesGuiRenderer::renderGui: glDisableVertexAttribArray");

                glDisable(GL_BLEND);
                CHECK_GLES_ERROR("GlesGuiRenderer::renderGui: glDisable");
        }

}
