// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "D3D9GUIRenderer.h"

#include "../../../../../Engine/Core/Resources/Texture/TextureFactory.h"
#include "../../../../../Engine/GUI/GUI.h"
#include "../D3D9Renderer.h"

namespace selene
{

        static const uint32_t guiGeometryNumFramesPerBatch   = 256;
        static const uint32_t guiGeometryNumVerticesPerFrame = 6;
        static const uint32_t guiGeometryNumVectorsPerVertex = 2;
        static const uint32_t guiGeometryVertexStride = guiGeometryNumVectorsPerVertex * sizeof(Vector4d);
        static const uint32_t guiGeometryVertexBufferSize = guiGeometryNumFramesPerBatch *
                                                            guiGeometryNumVerticesPerFrame *
                                                            guiGeometryVertexStride;

        D3d9GuiRenderer::D3d9GuiRenderer()
        {
                d3dVertexDeclaration_ = nullptr;
                d3dVertexBuffer_ = nullptr;
                d3dDevice_ = nullptr;

                fullScreenQuad_ = nullptr;
                textureHandler_ = nullptr;
                capabilities_ = nullptr;
        }
        D3d9GuiRenderer::~D3d9GuiRenderer()
        {
                destroy();
        }

        //--------------------------------------------------------------------------------
        bool D3d9GuiRenderer::initialize(D3d9FullScreenQuad& fullScreenQuad,
                                         D3d9TextureHandler& textureHandler,
                                         D3d9Capabilities& capabilities,
                                         FileManager* fileManager)
        {
                destroy();

                if(fileManager == nullptr)
                        return false;

                fullScreenQuad_ = &fullScreenQuad;
                textureHandler_ = &textureHandler;
                capabilities_ = &capabilities;

                d3dDevice_ = D3d9Renderer::getDevice();
                if(d3dDevice_ == nullptr)
                        return false;

                // load shaders
                D3d9Shader d3dVertexShaders[NUM_OF_VERTEX_SHADERS] =
                {
                        D3d9Shader("GUIFramesPass.vsh", "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, *capabilities_),
                        D3d9Shader("GUITextPass.vsh",   "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, *capabilities_),
                        D3d9Shader("GUICursorPass.vsh", "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, *capabilities_)
                };

                D3d9Shader d3dPixelShaders[NUM_OF_PIXEL_SHADERS] =
                {
                        D3d9Shader("GUIFramesPass.psh", "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_),
                        D3d9Shader("GUITextPass.psh",   "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_),
                        D3d9Shader("GUICursorPass.psh", "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_)
                };

                for(uint32_t i = 0; i < NUM_OF_VERTEX_SHADERS; ++i)
                {
                        if(!vertexShaders_[i].create(d3dVertexShaders[i]))
                        {
                                destroy();
                                return false;
                        }
                }

                for(uint32_t i = 0; i < NUM_OF_PIXEL_SHADERS; ++i)
                {
                        if(!pixelShaders_[i].create(d3dPixelShaders[i]))
                        {
                                destroy();
                                return false;
                        }
                }

                // create GUI geometry
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

                if(FAILED(d3dDevice_->CreateVertexBuffer(guiGeometryVertexBufferSize, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
                                                         0, D3DPOOL_DEFAULT, &d3dVertexBuffer_, nullptr)))
                {
                        destroy();
                        return false;
                }

                // load textures
                TextureFactory<D3d9Texture> textureFactory(fileManager);
                fontTexture_.reset(static_cast<D3d9Texture*>(textureFactory.createResource("Fonts/Font.dds")));
                if(!fontTexture_)
                {
                        destroy();
                        return false;
                }

                if(!fontTexture_->retain())
                {
                        destroy();
                        return false;
                }

                cursorTexture_.reset(static_cast<D3d9Texture*>(textureFactory.createResource("Cursors/Cursor.dds")));
                if(!cursorTexture_)
                {
                        destroy();
                        return false;
                }

                if(!cursorTexture_->retain())
                {
                        destroy();
                        return false;
                }

                return true;
        }

        //--------------------------------------------------------------------------------
        void D3d9GuiRenderer::destroy()
        {
                for(uint32_t i = 0; i < NUM_OF_VERTEX_SHADERS; ++i)
                        vertexShaders_[i].destroy();

                for(uint32_t i = 0; i < NUM_OF_PIXEL_SHADERS; ++i)
                        pixelShaders_[i].destroy();

                SAFE_RELEASE(d3dVertexDeclaration_);
                SAFE_RELEASE(d3dVertexBuffer_);

                d3dDevice_ = nullptr;

                fontTexture_.reset();
                cursorTexture_.reset();

                fullScreenQuad_ = nullptr;
                textureHandler_ = nullptr;
                capabilities_ = nullptr;
        }

        //--------------------------------------------------------------------------------
        void D3d9GuiRenderer::renderGui(Gui* gui)
        {
                if(d3dDevice_ == nullptr || gui == nullptr)
                        return;

                if(gui->is(GUI_HIDDEN))
                        return;

                const auto& elements = gui->getElements();

                const float characterSize = 0.0625f;
                static Vector4d frameVertices[guiGeometryNumFramesPerBatch * guiGeometryNumVerticesPerFrame *
                                              guiGeometryNumVectorsPerVertex];
                static uint32_t vertexIndices[guiGeometryNumVerticesPerFrame] = {0, 2, 1, 2, 3, 1};

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

                d3dDevice_->SetVertexDeclaration(d3dVertexDeclaration_);
                d3dDevice_->SetStreamSource(1, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(2, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(3, nullptr, 0, 0);

                d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
                d3dDevice_->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
                d3dDevice_->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
                d3dDevice_->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
                d3dDevice_->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
                d3dDevice_->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

                vertexShaders_[VERTEX_SHADER_GUI_FRAMES_PASS].set();
                pixelShaders_[PIXEL_SHADER_GUI_FRAMES_PASS].set();

                // render frames
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

                        for(uint32_t i = 0; i < guiGeometryNumVerticesPerFrame; ++i)
                        {
                                currentVertex->define(position + vertexOffsets[vertexIndices[i]], 0.0f, 1.0f);
                                ++currentVertex;
                                *currentVertex = color;
                                ++currentVertex;
                        }

                        ++numFrames;

                        if(numFrames == guiGeometryNumFramesPerBatch)
                        {
                                renderGeometry(frameVertices, numFrames);
                                numFrames = 0;
                                currentVertex = frameVertices;
                        }
                }

                if(numFrames != 0)
                        renderGeometry(frameVertices, numFrames);

                // render text
                vertexShaders_[VERTEX_SHADER_GUI_TEXT_PASS].set();
                pixelShaders_[PIXEL_SHADER_GUI_TEXT_PASS].set();

                textureHandler_->setStageState(LOCATION_FONT_TEXTURE,
                                               D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_LINEAR,
                                               D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
                textureHandler_->setTexture(fontTexture_.get(), LOCATION_FONT_TEXTURE,
                                            D3d9TextureHandler::DUMMY_TEXTURE_WHITE);

                d3dDevice_->SetRenderState(D3DRS_ALPHAREF, static_cast<DWORD>(0x000000AA));
                d3dDevice_->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
                d3dDevice_->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

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

                                for(uint32_t j = 0; j < guiGeometryNumVerticesPerFrame; ++j)
                                {
                                        currentVertex->define(position + vertexOffsets[vertexIndices[j]],
                                                              textureCoordinates + fontOffsets[vertexIndices[j]]);
                                        ++currentVertex;
                                        *currentVertex = textColor;
                                        ++currentVertex;
                                }

                                ++numFrames;

                                if(numFrames == guiGeometryNumFramesPerBatch)
                                {
                                        renderGeometry(frameVertices, numFrames);

                                        numFrames = 0;
                                        currentVertex = frameVertices;
                                }

                                position.x += fontSize.x;
                        }
                }

                if(numFrames != 0)
                        renderGeometry(frameVertices, numFrames);

                // render cursor
                if(!gui->is(GUI_DISABLED))
                {
                        vertexShaders_[VERTEX_SHADER_GUI_CURSOR_PASS].set();
                        pixelShaders_[PIXEL_SHADER_GUI_CURSOR_PASS].set();

                        Vector4d cursorPositionAndSize(gui->getCursorPosition() - Vector2d(1.0f, 1.0f), 0.0f, 0.0f);
                        cursorPositionAndSize.y = -cursorPositionAndSize.y;
                        cursorPositionAndSize.z = 0.05f;

                        textureHandler_->setTexture(cursorTexture_.get(), LOCATION_CURSOR_TEXTURE,
                                                    D3d9TextureHandler::DUMMY_TEXTURE_WHITE);
                        d3dDevice_->SetVertexShaderConstantF(LOCATION_CURSOR_POSITION_AND_SIZE,
                                                             static_cast<const float*>(cursorPositionAndSize), 1);

                        fullScreenQuad_->render();
                }

                d3dDevice_->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
                d3dDevice_->SetRenderState(D3DRS_ALPHATESTENABLE,  FALSE);
        }

        //--------------------------------------------------------------------------------
        void D3d9GuiRenderer::renderGeometry(const Vector4d* vertices, uint32_t numFrames)
        {
                uint8_t* destinationBuffer = nullptr;
                uint32_t size = numFrames * guiGeometryNumVerticesPerFrame * guiGeometryVertexStride;
                if(FAILED(d3dVertexBuffer_->Lock(0, size, reinterpret_cast<void**>(&destinationBuffer), 0)))
                        return;

                memcpy(destinationBuffer, vertices, size);
                d3dVertexBuffer_->Unlock();

                d3dDevice_->SetStreamSource(0, d3dVertexBuffer_, 0, guiGeometryVertexStride);
                d3dDevice_->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2 * numFrames);
        }

}
