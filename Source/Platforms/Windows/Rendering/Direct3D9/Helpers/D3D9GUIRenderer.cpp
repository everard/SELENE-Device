// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "../../../../../Engine/Core/FileManager/FileManager.h"
#include "../../../../../Engine/GUI/GUI.h"
#include "D3D9GUIRenderer.h"

namespace selene
{

        const uint32_t D3d9GuiRenderer::numVerticesPerElement_ = 6;

        const uint32_t D3d9GuiRenderer::cursorVertexStride_ = 4 * sizeof(float);

        const uint32_t D3d9GuiRenderer::vertexStride_ = 8 * sizeof(float);

        const uint32_t D3d9GuiRenderer::numVertices_ = 24576;

        const uint32_t D3d9GuiRenderer::numFaces_ = 8192;

        D3d9GuiRenderer::D3d9GuiRenderer()
        {
                d3dVertexDeclaration_ = d3dCursorVertexDeclaration_ = nullptr;
                d3dVertexBuffer_ = d3dCursorVertexBuffer_ = nullptr;

                d3dCursorTexture_ = nullptr;
                d3dFontTexture_ = nullptr;

                d3dDevice_ = nullptr;
                gui_ = nullptr;

                textVertexIndex_ = 0;
                numFrameFaces_ = 0;
                numTextFaces_ = 0;
        }
        D3d9GuiRenderer::~D3d9GuiRenderer()
        {
                destroy();
        }

        //--------------------------------------------------------
        bool D3d9GuiRenderer::initialize(FileManager* fileManager)
        {
                if(fileManager == nullptr)
                        return false;

                d3dDevice_ = D3d9Device::getInterface();
                if(d3dDevice_ == nullptr)
                        return false;

                destroy();

                const char* fontTextureFileName = fileManager->find("Fonts//Font.dds");
                if(fontTextureFileName == nullptr)
                        return false;

                if(FAILED(D3DXCreateTextureFromFile(d3dDevice_, fontTextureFileName, &d3dFontTexture_)))
                {
                        destroy();
                        return false;
                }

                const char* cursorTextureFileName = fileManager->find("Cursors//Cursor.dds");
                if(cursorTextureFileName == nullptr)
                {
                        destroy();
                        return false;
                }

                if(FAILED(D3DXCreateTextureFromFile(d3dDevice_, cursorTextureFileName, &d3dCursorTexture_)))
                {
                        destroy();
                        return false;
                }

                // create GUI elements geometry
                static D3DVERTEXELEMENT9 d3dVertexElements[] =
                {
                        {0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
                        {0, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
                        D3DDECL_END()
                };

                if(FAILED(d3dDevice_->CreateVertexDeclaration(d3dVertexElements, &d3dVertexDeclaration_)))
                {
                        destroy();
                        return false;
                }

                if(FAILED(d3dDevice_->CreateVertexBuffer(numVertices_ * vertexStride_, D3DUSAGE_WRITEONLY,
                                                         0, D3DPOOL_DEFAULT, &d3dVertexBuffer_, nullptr)))
                {
                        destroy();
                        return false;
                }

                // create cursor geometry
                static D3DVERTEXELEMENT9 d3dCursorVertexElements[] =
                {
                        {0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
                        D3DDECL_END()
                };

                static float vertices[] =
                {
                        0.0f, -0.1f, 0.0f, 1.0f,
                        0.0f,  0.0f,  0.0f, 0.0f,
                        0.1f, -0.1f, 1.0f, 1.0f,
                        0.1f, 0.0f, 1.0f, 0.0f
                };

                if(FAILED(d3dDevice_->CreateVertexDeclaration(d3dCursorVertexElements,
                                                              &d3dCursorVertexDeclaration_)))
                {
                        destroy();
                        return false;
                }

                if(FAILED(d3dDevice_->CreateVertexBuffer(4 * cursorVertexStride_, 0, 0, D3DPOOL_DEFAULT,
                                                         &d3dCursorVertexBuffer_, nullptr)))
                {
                        destroy();
                        return false;
                }

                void* destinationBuffer = nullptr;
                if(FAILED(d3dCursorVertexBuffer_->Lock(0, 4 * cursorVertexStride_, &destinationBuffer, 0)))
                {
                        destroy();
                        return false;
                }
                memcpy(destinationBuffer, (void*)vertices, 4 * cursorVertexStride_);
                d3dVertexBuffer_->Unlock();

                return true;
        }

        //--------------------------------------------------------
        void D3d9GuiRenderer::destroy()
        {
                SAFE_RELEASE(d3dCursorVertexDeclaration_);
                SAFE_RELEASE(d3dVertexDeclaration_);
                SAFE_RELEASE(d3dCursorVertexBuffer_);
                SAFE_RELEASE(d3dVertexBuffer_);

                SAFE_RELEASE(d3dCursorTexture_);
                SAFE_RELEASE(d3dFontTexture_);

                textVertexIndex_ = 0;
                numFrameFaces_ = 0;
                numTextFaces_ = 0;
        }

        //--------------------------------------------------------
        void D3d9GuiRenderer::setGui(Gui* gui)
        {
                gui_ = gui;
        }

        //--------------------------------------------------------
        bool D3d9GuiRenderer::prepare()
        {
                if(d3dVertexDeclaration_ == nullptr || d3dVertexBuffer_ == nullptr ||
                   d3dFontTexture_ == nullptr || d3dDevice_ == nullptr || gui_ == nullptr)
                {
                        textVertexIndex_ = 0;
                        numFrameFaces_ = 0;
                        numTextFaces_ = 0;
                        return false;
                }

                if(gui_->is(GUI_HIDDEN))
                        return false;

                if(gui_->is(GUI_UPDATED))
                {
                        d3dDevice_->SetVertexDeclaration(d3dVertexDeclaration_);
                        d3dDevice_->SetStreamSource(0, d3dVertexBuffer_, 0, vertexStride_);
                        return true;
                }

                gui_->setFlags(GUI_UPDATED);

                textVertexIndex_ = 0;
                numFrameFaces_ = 0;
                numTextFaces_ = 0;

                const std::map<int32_t, std::shared_ptr<Gui::Element>>& elements = gui_->getElements();

                if(elements.size() == 0)
                        return false;

                uint8_t* destinationBuffer = nullptr;
                if(FAILED(d3dVertexBuffer_->Lock(0, numVertices_ * vertexStride_,
                                                 (void**)&destinationBuffer, 0)))
                {
                        destroy();
                        return false;
                }

                uint32_t vertexNo = 0;

                const float characterSize = 0.0625f;
                Vector2d vertexOffsets[4];
                Vector2d fontOffsets[4] =
                {
                        Vector2d(0.0f, 0.0f),
                        Vector2d(characterSize, 0.0f),
                        Vector2d(0.0f, characterSize),
                        Vector2d(characterSize, characterSize)
                };
                Vector4d vertices[2 * numVerticesPerElement_];
                uint32_t vertexIndices[numVerticesPerElement_] = {0, 1, 2, 2, 1, 3};

                // prepare frames
                for(auto it = elements.begin(); it != elements.end(); ++it)
                {
                        if(vertexNo > (numVertices_ - numVerticesPerElement_))
                                break;

                        Gui::Element* element = it->second.get();
                        if(element == nullptr)
                                continue;

                        if(element->is(GUI_ELEMENT_HIDDEN))
                                continue;

                        Vector4d color = element->getBackgroundColor(GUI_ELEMENT_COLOR_DEFAULT);

                        if(element->is(GUI_ELEMENT_SELECTED))
                                color = element->getBackgroundColor(GUI_ELEMENT_COLOR_SELECTED);
                        else if(element->is(GUI_ELEMENT_TOUCHED))
                                color = element->getBackgroundColor(GUI_ELEMENT_COLOR_TOUCHED);

                        const Vector2d& size = element->getSize();
                        vertexOffsets[1] = Vector2d(size.x, 0.0f);
                        vertexOffsets[2] = Vector2d(0.0f, size.y);
                        vertexOffsets[3] = Vector2d(size.x, size.y);

                        for(uint32_t i = 0; i < numVerticesPerElement_; ++i)
                        {
                                vertices[2 * i] = Vector4d(element->getPosition() +
                                                           vertexOffsets[vertexIndices[i]]);
                                vertices[2 * i + 1] = color;
                        }

                        numFrameFaces_ += 2;
                        vertexNo += numVerticesPerElement_;

                        memcpy(destinationBuffer, vertices, numVerticesPerElement_ * vertexStride_);

                        destinationBuffer += numVerticesPerElement_ * vertexStride_;
                }

                // prepare text
                textVertexIndex_ = vertexNo;

                for(auto it = elements.begin(); it != elements.end(); ++it)
                {
                        if(vertexNo > (numVertices_ - numVerticesPerElement_))
                                break;

                        Gui::Element* element = it->second.get();
                        if(element == nullptr)
                                continue;

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

                        const Vector2d& fontSize = element->getFontSize();
                        vertexOffsets[1] = Vector2d(fontSize.x, 0.0f);
                        vertexOffsets[2] = Vector2d(0.0f, fontSize.y);
                        vertexOffsets[3] = Vector2d(fontSize.x, fontSize.y);

                        Vector2d position = element->getPosition();

                        for(size_t i = 0; i < text.length(); ++i)
                        {
                                if(vertexNo > (numVertices_ - numVerticesPerElement_))
                                        break;

                                uint8_t c = text[i];
                                Vector2d textureCoordinates((float)(c % 16) * characterSize,
                                                            (float)(c >> 4) * characterSize);

                                for(uint32_t j = 0; j < numVerticesPerElement_; ++j)
                                {
                                        vertices[2 * j] = Vector4d(position +
                                                                   vertexOffsets[vertexIndices[j]],
                                                                   textureCoordinates +
                                                                   fontOffsets[vertexIndices[j]]);
                                        vertices[2 * j + 1] = textColor;
                                }

                                numTextFaces_ += 2;
                                vertexNo += numVerticesPerElement_;

                                memcpy(destinationBuffer, vertices, numVerticesPerElement_ * vertexStride_);

                                destinationBuffer += numVerticesPerElement_ * vertexStride_;

                                position.x += fontSize.x;
                        }
                }

                d3dVertexBuffer_->Unlock();
                d3dDevice_->SetVertexDeclaration(d3dVertexDeclaration_);
                d3dDevice_->SetStreamSource(0, d3dVertexBuffer_, 0, vertexStride_);
                return true;
        }

        //--------------------------------------------------------
        bool D3d9GuiRenderer::renderFrames()
        {
                if(numFrameFaces_ == 0)
                        return false;

                d3dDevice_->DrawPrimitive(D3DPT_TRIANGLELIST, 0, numFrameFaces_);

                return true;
        }

        //--------------------------------------------------------
        bool D3d9GuiRenderer::renderText()
        {
                if(numTextFaces_ == 0)
                        return false;

                d3dDevice_->SetTexture(0, d3dFontTexture_);
                d3dDevice_->DrawPrimitive(D3DPT_TRIANGLELIST, textVertexIndex_, numTextFaces_);

                return true;
        }

        //--------------------------------------------------------
        bool D3d9GuiRenderer::renderCursor()
        {
                Vector4d cursorPosition(gui_->getCursorPosition() - Vector2d(1.0f, 1.0f), 0.0f, 0.0f);
                cursorPosition.y = -cursorPosition.y;

                d3dDevice_->SetTexture(0, d3dCursorTexture_);
                d3dDevice_->SetVertexShaderConstantF(0, static_cast<const float*>(cursorPosition), 1);

                d3dDevice_->SetVertexDeclaration(d3dCursorVertexDeclaration_);
                d3dDevice_->SetStreamSource(0, d3dCursorVertexBuffer_, 0, cursorVertexStride_);
                d3dDevice_->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

                return true;
        }

}
