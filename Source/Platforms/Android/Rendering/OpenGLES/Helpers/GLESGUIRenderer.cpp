// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLESGUIRenderer.h"

#include "../../../../../Engine/Core/Resources/Texture/TextureFactory.h"
#include "../../../../../Engine/GUI/GUI.h"
#include "../../../Platform.h"

namespace selene
{

        static const char framesRenderingVertexShader[] =
                "attribute vec4 position;"
                "attribute vec4 color;"
                "varying vec4 frameColor;"
                "void main()"
                "{"
                "        gl_Position = vec4(position.x - 1.0, 1.0 - position.y, 0.0, 1.0);"
                "        frameColor  = color;"
                "}\n";

        static const char framesRenderingFragmentShader[] =
                "precision mediump float;"
                "varying vec4 frameColor;"
                "void main()"
                "{"
                "        gl_FragColor = frameColor;"
                "}\n";

        static const char textRenderingVertexShader[] =
                "attribute vec4 position;"
                "attribute vec4 color;"
                "varying vec4 textColor;"
                "varying vec2 textureCoordinates;"
                "void main()"
                "{"
                "        gl_Position = vec4(position.x - 1.0, 1.0 - position.y, 0.0, 1.0);"
                "        textureCoordinates = vec2(position.z, position.w);"
                "        textColor = color;"
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

        GlesGuiRenderer::GlesGuiRenderer()
        {
                framesPositionLocation_ = framesColorLocation_ = -1;
                textPositionLocation_   = textColorLocation_   = -1;
                fontTextureLocation_ = -1;
        }
        GlesGuiRenderer::~GlesGuiRenderer()
        {
                destroy();
        }

        //--------------------------------------------------------
        bool GlesGuiRenderer::initialize(FileManager* fileManager)
        {
                TextureFactory<GlesTexture> textureFactory(fileManager);
                fontTexture_.reset(reinterpret_cast<GlesTexture*>(textureFactory.createResource("Fonts/Font.dds")));

                if(!fontTexture_)
                        return false;

                return retain();
        }

        //--------------------------------------------------------
        void GlesGuiRenderer::destroy()
        {
                fontTexture_.reset();
                discard();
        }

        //--------------------------------------------------------
        bool GlesGuiRenderer::retain()
        {
                if(!fontTexture_->retain())
                        return false;

                if(!framesRenderingProgram_.initialize(framesRenderingVertexShader, framesRenderingFragmentShader))
                        return false;

                if(!textRenderingProgram_.initialize(textRenderingVertexShader, textRenderingFragmentShader))
                        return false;

                framesPositionLocation_ = framesRenderingProgram_.getAttributeLocation("position");
                GlesRenderer::checkGlesError("glGetAttribLocation");

                framesColorLocation_ = framesRenderingProgram_.getAttributeLocation("color");
                GlesRenderer::checkGlesError("glGetAttribLocation");

                textPositionLocation_ = textRenderingProgram_.getAttributeLocation("position");
                GlesRenderer::checkGlesError("glGetAttribLocation");

                textColorLocation_ = textRenderingProgram_.getAttributeLocation("color");
                GlesRenderer::checkGlesError("glGetAttribLocation");

                fontTextureLocation_ = textRenderingProgram_.getUniformLocation("fontTexture");
                GlesRenderer::checkGlesError("glGetUniformLocation");

                return true;
        }

        //--------------------------------------------------------
        void GlesGuiRenderer::discard()
        {
                if(fontTexture_)
                        fontTexture_->discard();

                framesRenderingProgram_.destroy();
                textRenderingProgram_.destroy();

                framesPositionLocation_ = framesColorLocation_ = -1;
                textPositionLocation_   = textColorLocation_   = -1;
                fontTextureLocation_ = -1;
        }

        //--------------------------------------------------------
        void GlesGuiRenderer::render(Gui* gui)
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

                // render frames
                framesRenderingProgram_.use();
                GlesRenderer::checkGlesError("glUseProgram");

                glEnableVertexAttribArray(framesPositionLocation_);
                GlesRenderer::checkGlesError("glEnableVertexAttribArray");

                glEnableVertexAttribArray(framesColorLocation_);
                GlesRenderer::checkGlesError("glEnableVertexAttribArray");

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
                                glVertexAttribPointer(framesPositionLocation_, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector4d), static_cast<void*>(frameVertices));
                                glVertexAttribPointer(framesColorLocation_,    4, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector4d), static_cast<void*>(frameVertices + 1));
                                glDrawArrays(GL_TRIANGLES, 0, numFrames * numVerticesPerFrame);

                                numFrames = 0;
                                currentVertex = frameVertices;
                        }
                }

                if(numFrames != 0)
                {
                        glVertexAttribPointer(framesPositionLocation_, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector4d), static_cast<void*>(frameVertices));
                        glVertexAttribPointer(framesColorLocation_,    4, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector4d), static_cast<void*>(frameVertices + 1));
                        glDrawArrays(GL_TRIANGLES, 0, numFrames * numVerticesPerFrame);
                }

                glDisableVertexAttribArray(framesPositionLocation_);
                GlesRenderer::checkGlesError("glEnableVertexAttribArray");

                glDisableVertexAttribArray(framesColorLocation_);
                GlesRenderer::checkGlesError("glEnableVertexAttribArray");

                // render text
                textRenderingProgram_.use();
                GlesRenderer::checkGlesError("glUseProgram");

                glEnableVertexAttribArray(textPositionLocation_);
                GlesRenderer::checkGlesError("glEnableVertexAttribArray");

                glEnableVertexAttribArray(textColorLocation_);
                GlesRenderer::checkGlesError("glEnableVertexAttribArray");

                glUniform1i(fontTextureLocation_, 0);

                glActiveTexture(GL_TEXTURE0);
                GlesRenderer::checkGlesError("glActiveTexture");

                glBindTexture(GL_TEXTURE_2D, fontTexture_->texture_);
                GlesRenderer::checkGlesError("glBindTexture");

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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
                                        glVertexAttribPointer(textPositionLocation_, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector4d), static_cast<void*>(frameVertices));
                                        glVertexAttribPointer(textColorLocation_,    4, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector4d), static_cast<void*>(frameVertices + 1));
                                        glDrawArrays(GL_TRIANGLES, 0, numFrames * numVerticesPerFrame);

                                        numFrames = 0;
                                        currentVertex = frameVertices;
                                }

                                position.x += fontSize.x;
                        }
                }

                if(numFrames != 0)
                {
                        glVertexAttribPointer(textPositionLocation_, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector4d), static_cast<void*>(frameVertices));
                        glVertexAttribPointer(textColorLocation_,    4, GL_FLOAT, GL_FALSE, 2 * sizeof(Vector4d), static_cast<void*>(frameVertices + 1));
                        glDrawArrays(GL_TRIANGLES, 0, numFrames * numVerticesPerFrame);
                }

                glDisableVertexAttribArray(textPositionLocation_);
                GlesRenderer::checkGlesError("glEnableVertexAttribArray");

                glDisableVertexAttribArray(textColorLocation_);
                GlesRenderer::checkGlesError("glEnableVertexAttribArray");
        }

}
