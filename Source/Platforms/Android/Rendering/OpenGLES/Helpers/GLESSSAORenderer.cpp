// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)
/*
#include "D3D9SSAORenderer.h"
#include "../D3D9Renderer.h"

namespace selene
{

        static float ssaoGeometryBuffer[] =
        {
                // radius 1.0
                -1.0f, -1.0f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, -0.5f, -0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, -0.5f, -0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 1.0f,
                 1.0f,  1.0f, 1.0f, 0.0f, -0.5f, -0.5f, -0.5f, 1.0f,

                -1.0f, -1.0f, 0.0f, 1.0f, 0.5f, -0.5f, -0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, -0.5f, -0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, -0.5f, -0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, -0.5f, -0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, -0.5f, -0.5f, 1.0f,
                 1.0f,  1.0f, 1.0f, 0.0f, 0.5f, -0.5f, -0.5f, 1.0f,

                -1.0f, -1.0f, 0.0f, 1.0f, -0.5f, 0.5f, -0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, 0.5f, -0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, 0.5f, -0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, 0.5f, -0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, 0.5f, -0.5f, 1.0f,
                 1.0f,  1.0f, 1.0f, 0.0f, -0.5f, 0.5f, -0.5f, 1.0f,

                -1.0f, -1.0f, 0.0f, 1.0f, 0.5f, 0.5f, -0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, 0.5f, -0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, 0.5f, -0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, 0.5f, -0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, 0.5f, -0.5f, 1.0f,
                 1.0f,  1.0f, 1.0f, 0.0f, 0.5f, 0.5f, -0.5f, 1.0f,

                -1.0f, -1.0f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, -0.5f, 0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, -0.5f, 0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, -0.5f, 0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, -0.5f, 0.5f, 1.0f,
                 1.0f,  1.0f, 1.0f, 0.0f, -0.5f, -0.5f, 0.5f, 1.0f,

                -1.0f, -1.0f, 0.0f, 1.0f, 0.5f, -0.5f, 0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, -0.5f, 0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, -0.5f, 0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f, 1.0f,
                 1.0f,  1.0f, 1.0f, 0.0f, 0.5f, -0.5f, 0.5f, 1.0f,

                -1.0f, -1.0f, 0.0f, 1.0f, -0.5f, 0.5f, 0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, 0.5f, 0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, 0.5f, 0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, 0.5f, 0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, 0.5f, 0.5f, 1.0f,
                 1.0f,  1.0f, 1.0f, 0.0f, -0.5f, 0.5f, 0.5f, 1.0f,

                -1.0f, -1.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f,
                 1.0f,  1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f,

                // radius 0.5
                -1.0f, -1.0f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.5f,
                 1.0f,  1.0f, 1.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.5f,

                -1.0f, -1.0f, 0.0f, 1.0f, 0.5f, -0.5f, -0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, -0.5f, -0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, -0.5f, -0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, -0.5f, -0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, -0.5f, -0.5f, 0.5f,
                 1.0f,  1.0f, 1.0f, 0.0f, 0.5f, -0.5f, -0.5f, 0.5f,

                -1.0f, -1.0f, 0.0f, 1.0f, -0.5f, 0.5f, -0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, 0.5f, -0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, 0.5f, -0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, 0.5f, -0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, 0.5f, -0.5f, 0.5f,
                 1.0f,  1.0f, 1.0f, 0.0f, -0.5f, 0.5f, -0.5f, 0.5f,

                -1.0f, -1.0f, 0.0f, 1.0f, 0.5f, 0.5f, -0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, 0.5f, -0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, 0.5f, -0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, 0.5f, -0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, 0.5f, -0.5f, 0.5f,
                 1.0f,  1.0f, 1.0f, 0.0f, 0.5f, 0.5f, -0.5f, 0.5f,

                -1.0f, -1.0f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, -0.5f, 0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, -0.5f, 0.5f, 0.5f,
                 1.0f,  1.0f, 1.0f, 0.0f, -0.5f, -0.5f, 0.5f, 0.5f,

                -1.0f, -1.0f, 0.0f, 1.0f, 0.5f, -0.5f, 0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, -0.5f, 0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, -0.5f, 0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f, 0.5f,
                 1.0f,  1.0f, 1.0f, 0.0f, 0.5f, -0.5f, 0.5f, 0.5f,

                -1.0f, -1.0f, 0.0f, 1.0f, -0.5f, 0.5f, 0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, 0.5f, 0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, 0.5f, 0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, 0.5f, 0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, 0.5f, 0.5f, 0.5f,
                 1.0f,  1.0f, 1.0f, 0.0f, -0.5f, 0.5f, 0.5f, 0.5f,

                -1.0f, -1.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.5f,
                 1.0f,  1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.5f
        };

        static const uint32_t ssaoGeometryVertexStride = 2 * sizeof(Vector4d);
        static const uint32_t ssaoGeometryNumVertices = 96;
        static const uint32_t ssaoGeometryNumFaces = 32;
        static const uint32_t ssaoGeometryBufferSize = ssaoGeometryVertexStride * ssaoGeometryNumVertices;

        // Fills random texture
        VOID WINAPI fillRandomTexture(D3DXVECTOR4* output, const D3DXVECTOR2*, const D3DXVECTOR2*, LPVOID)
        {
                static D3DXVECTOR4 colors[16] =
                {
                        D3DXVECTOR4(138.0f / 255.0f, 106.0f / 255.0f, 241.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(119.0f / 255.0f, 37.0f / 255.0f, 111.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(149.0f / 255.0f, 208.0f / 255.0f, 90.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(138.0f / 255.0f, 184.0f / 255.0f, 23.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(57.0f / 255.0f, 105.0f / 255.0f, 205.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(20.0f / 255.0f, 134.0f / 255.0f, 144.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(46.0f / 255.0f, 87.0f / 255.0f, 83.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(75.0f / 255.0f, 160.0f / 255.0f, 33.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(81.0f / 255.0f, 208.0f / 255.0f, 127.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(156.0f / 255.0f, 202.0f / 255.0f, 199.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(90.0f / 255.0f, 174.0f / 255.0f, 200.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(208.0f / 255.0f, 155.0f / 255.0f, 77.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(215.0f / 255.0f, 72.0f / 255.0f, 92.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(229.0f / 255.0f, 89.0f / 255.0f, 196.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(139.0f / 255.0f, 43.0f / 255.0f, 194.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(129.0f / 255.0f, 75.0f / 255.0f, 28.0f / 255.0f, 1.0f)
                };

                *output = colors[rand() % 16];
        }

        D3d9SsaoRenderer::D3d9SsaoRenderer()
        {
                d3dVertexDeclaration_ = nullptr;
                d3dRandomTexture_ = nullptr;
                d3dVertexBuffer_  = nullptr;
                d3dDevice_ = nullptr;

                renderTargetContainer_ = nullptr;
                frameParameters_ = nullptr;
                fullScreenQuad_ = nullptr;
                textureHandler_ = nullptr;
                capabilities_ = nullptr;
        }
        D3d9SsaoRenderer::~D3d9SsaoRenderer()
        {
                destroy();
        }

        //---------------------------------------------------------------------------------
        bool D3d9SsaoRenderer::initialize(D3d9RenderTargetContainer& renderTargetContainer,
                                          D3d9FrameParameters& frameParameters,
                                          D3d9FullScreenQuad& fullScreenQuad,
                                          D3d9TextureHandler& textureHandler,
                                          D3d9Capabilities& capabilities)
        {
                destroy();

                renderTargetContainer_ = &renderTargetContainer;
                frameParameters_ = &frameParameters;
                fullScreenQuad_ = &fullScreenQuad;
                textureHandler_ = &textureHandler;
                capabilities_ = &capabilities;

                d3dDevice_ = D3d9Renderer::getDevice();
                if(d3dDevice_ == nullptr)
                        return false;

                // load shaders
                D3d9Shader d3dVertexShaders[NUM_OF_VERTEX_SHADERS] =
                {
                        D3d9Shader("SSAOPass.vsh",  "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, *capabilities_),
                        D3d9Shader("SSAOBlurX.vsh", "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, *capabilities_),
                        D3d9Shader("SSAOBlurY.vsh", "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, *capabilities_)
                };

                D3d9Shader d3dPixelShaders[NUM_OF_PIXEL_SHADERS] =
                {
                        D3d9Shader("SSAOPass.psh",  "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_),
                        D3d9Shader("SSAOBlurX.psh", "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_),
                        D3d9Shader("SSAOBlurY.psh", "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_)
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

                if(capabilities_->isThirdShaderModelSupported())
                {
                        D3d9Shader d3dOptionalVertexShader("SSAO30Pass.vsh", "vs_1_1", 0, D3d9Shader::LIBRARY_VERTEX_SHADER, *capabilities_);
                        D3d9Shader d3dOptionalPixelShader("SSAO30Pass.psh",  "ps_3_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_);

                        if(!optionalVertexShaders_[OPTIONAL_VERTEX_SHADER_SSAO_PASS].create(d3dOptionalVertexShader) ||
                           !optionalPixelShaders_[OPTIONAL_PIXEL_SHADER_SSAO_PASS].create(d3dOptionalPixelShader))
                        {
                                destroy();
                                return false;
                        }
                }

                // create vertex declaration and vertex buffer
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

                if(FAILED(d3dDevice_->CreateVertexBuffer(ssaoGeometryBufferSize,
                                                         0, 0, D3DPOOL_DEFAULT, &d3dVertexBuffer_, nullptr)))
                {
                        destroy();
                        return false;
                }

                void* destinationBuffer = nullptr;
                if(FAILED(d3dVertexBuffer_->Lock(0, ssaoGeometryBufferSize, &destinationBuffer, 0)))
                {
                        destroy();
                        return false;
                }
                memcpy(destinationBuffer, static_cast<void*>(ssaoGeometryBuffer), ssaoGeometryBufferSize);
                d3dVertexBuffer_->Unlock();

                // create random texture
                if(FAILED(d3dDevice_->CreateTexture(64, 64, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
                                                    &d3dRandomTexture_, nullptr)))
                {
                        destroy();
                        return false;
                }

                srand(0);
                if(FAILED(D3DXFillTexture(d3dRandomTexture_, fillRandomTexture, nullptr)))
                {
                        destroy();
                        return false;
                }

                return true;
        }

        //---------------------------------------------------------------------------------
        void D3d9SsaoRenderer::destroy()
        {
                for(uint32_t i = 0; i < NUM_OF_OPTIONAL_VERTEX_SHADERS; ++i)
                        optionalVertexShaders_[i].destroy();

                for(uint32_t i = 0; i < NUM_OF_OPTIONAL_PIXEL_SHADERS; ++i)
                        optionalPixelShaders_[i].destroy();

                for(uint32_t i = 0; i < NUM_OF_VERTEX_SHADERS; ++i)
                        vertexShaders_[i].destroy();

                for(uint32_t i = 0; i < NUM_OF_PIXEL_SHADERS; ++i)
                        pixelShaders_[i].destroy();

                SAFE_RELEASE(d3dVertexDeclaration_);
                SAFE_RELEASE(d3dVertexBuffer_);
                SAFE_RELEASE(d3dRandomTexture_);

                d3dDevice_ = nullptr;

                renderTargetContainer_ = nullptr;
                frameParameters_ = nullptr;
                fullScreenQuad_ = nullptr;
                textureHandler_ = nullptr;
                capabilities_ = nullptr;
        }

        //---------------------------------------------------------------------------------
        void D3d9SsaoRenderer::renderSsao()
        {
                if(d3dDevice_ == nullptr)
                        return;

                d3dDevice_->SetStreamSource(1, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(2, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(3, nullptr, 0, 0);

                if(capabilities_->isThirdShaderModelSupported())
                {
                        d3dDevice_->SetRenderTarget(0, renderTargetContainer_->getRenderTarget(RENDER_TARGET_HELPER_0).getSurface());
                        d3dDevice_->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

                        optionalVertexShaders_[OPTIONAL_VERTEX_SHADER_SSAO_PASS].set();
                        optionalPixelShaders_[OPTIONAL_PIXEL_SHADER_SSAO_PASS].set();
                }
                else
                {
                        d3dDevice_->SetRenderTarget(0, renderTargetContainer_->getRenderTarget(RENDER_TARGET_HALF_SIZE_HELPER).getSurface());
                        d3dDevice_->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

                        vertexShaders_[VERTEX_SHADER_SSAO_PASS].set();
                        pixelShaders_[PIXEL_SHADER_SSAO_PASS].set();
                }

                d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                d3dDevice_->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

                d3dDevice_->SetPixelShaderConstantF(LOCATION_UNPROJECTION_VECTOR,
                                                    static_cast<const float*>(frameParameters_->unprojectionVector), 1);
                d3dDevice_->SetPixelShaderConstantF(LOCATION_PROJECTION_PARAMETERS,
                                                    static_cast<const float*>(frameParameters_->projectionParameters), 1);
                d3dDevice_->SetPixelShaderConstantF(LOCATION_SSAO_PARAMETERS,
                                                    static_cast<const float*>(frameParameters_->ssaoParameters), 1);

                d3dDevice_->SetVertexShaderConstantF(LOCATION_SCREEN_SIZE_SSAO_PASS,
                                                     static_cast<const float*>(frameParameters_->screenSize), 1);

                textureHandler_->setStageState(LOCATION_POSITIONS_MAP,
                                               D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE,
                                               D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(LOCATION_POSITIONS_MAP,
                                       renderTargetContainer_->getRenderTarget(RENDER_TARGET_POSITIONS).getTexture());

                textureHandler_->setStageState(LOCATION_NORMALS_MAP,
                                               D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE,
                                               D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(LOCATION_NORMALS_MAP,
                                       renderTargetContainer_->getRenderTarget(RENDER_TARGET_NORMALS).getTexture());

                textureHandler_->setStageState(LOCATION_HELPER_TEXTURE, D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE);
                d3dDevice_->SetTexture(LOCATION_HELPER_TEXTURE, d3dRandomTexture_);

                if(capabilities_->isThirdShaderModelSupported())
                {
                        fullScreenQuad_->render();
                }
                else
                {
                        d3dDevice_->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ONE);
                        d3dDevice_->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
                        d3dDevice_->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

                        d3dDevice_->SetVertexDeclaration(d3dVertexDeclaration_);
                        d3dDevice_->SetStreamSource(0, d3dVertexBuffer_, 0, ssaoGeometryVertexStride);
                        d3dDevice_->DrawPrimitive(D3DPT_TRIANGLELIST, 0, ssaoGeometryNumFaces);

                        d3dDevice_->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
                }

                // blur SSAO
                Vector4d edgeDetectionParameters = frameParameters_->edgeDetectionParameters;
                edgeDetectionParameters.z = 4.0f / frameParameters_->screenSize.x;
                edgeDetectionParameters.w = 4.0f / frameParameters_->screenSize.y;

                blurSsao(edgeDetectionParameters, !capabilities_->isThirdShaderModelSupported());
                edgeDetectionParameters.z = 2.0f / frameParameters_->screenSize.x;
                edgeDetectionParameters.w = 2.0f / frameParameters_->screenSize.y;
                blurSsao(edgeDetectionParameters);
                edgeDetectionParameters.z = 1.0f / frameParameters_->screenSize.x;
                edgeDetectionParameters.w = 1.0f / frameParameters_->screenSize.y;
                blurSsao(edgeDetectionParameters);
        }

        //---------------------------------------------------------------------------------
        void D3d9SsaoRenderer::blurSsao(const Vector4d& edgeDetectionParameters,
                                        bool shouldUpscale)
        {
                LPDIRECT3DSURFACE9 renderTargets[] =
                {
                        renderTargetContainer_->getRenderTarget(RENDER_TARGET_HELPER_1).getSurface(),
                        renderTargetContainer_->getRenderTarget(RENDER_TARGET_HELPER_0).getSurface()
                };

                LPDIRECT3DTEXTURE9 textures[] =
                {
                        renderTargetContainer_->getRenderTarget(RENDER_TARGET_HELPER_0).getTexture(),
                        renderTargetContainer_->getRenderTarget(RENDER_TARGET_HELPER_1).getTexture()
                };

                for(uint8_t i = 0; i < 2; ++i)
                {
                        d3dDevice_->SetRenderTarget(0, renderTargets[i]);
                        d3dDevice_->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

                        vertexShaders_[VERTEX_SHADER_SSAO_BLUR_X_PASS + i].set();
                        pixelShaders_[PIXEL_SHADER_SSAO_BLUR_X_PASS + i].set();

                        d3dDevice_->SetPixelShaderConstantF(LOCATION_UNPROJECTION_VECTOR,
                                                            static_cast<const float*>(frameParameters_->unprojectionVector), 1);
                        d3dDevice_->SetPixelShaderConstantF(LOCATION_PROJECTION_PARAMETERS,
                                                            static_cast<const float*>(frameParameters_->projectionParameters), 1);
                        d3dDevice_->SetPixelShaderConstantF(LOCATION_EDGE_DETECTION_PARAMETERS,
                                                            static_cast<const float*>(edgeDetectionParameters), 1);
                        d3dDevice_->SetPixelShaderConstantF(LOCATION_SCREEN_SIZE_BLUR_PASS,
                                                            static_cast<const float*>(frameParameters_->screenSize), 1);
                        d3dDevice_->SetPixelShaderConstantF(LOCATION_TEXTURE_COORDINATES_ADJUSTMENT,
                                                            static_cast<const float*>(frameParameters_->textureCoordinatesAdjustment), 1);

                        textureHandler_->setStageState(LOCATION_POSITIONS_MAP,
                                                       D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE,
                                                       D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
                        d3dDevice_->SetTexture(LOCATION_POSITIONS_MAP,
                                               renderTargetContainer_->getRenderTarget(RENDER_TARGET_POSITIONS).getTexture());

                        textureHandler_->setStageState(LOCATION_NORMALS_MAP,
                                                       D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE,
                                                       D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
                        d3dDevice_->SetTexture(LOCATION_NORMALS_MAP,
                                               renderTargetContainer_->getRenderTarget(RENDER_TARGET_NORMALS).getTexture());

                        if(shouldUpscale)
                        {
                                textureHandler_->setStageState(LOCATION_SSAO_BUFFER,
                                                               D3DTEXF_LINEAR, D3DTEXF_POINT, D3DTEXF_LINEAR,
                                                               D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
                                d3dDevice_->SetTexture(LOCATION_SSAO_BUFFER,
                                                       renderTargetContainer_->getRenderTarget(RENDER_TARGET_HALF_SIZE_HELPER).getTexture());
                                shouldUpscale = false;
                        }
                        else
                        {
                                textureHandler_->setStageState(LOCATION_SSAO_BUFFER,
                                                               D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE,
                                                               D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
                                d3dDevice_->SetTexture(LOCATION_SSAO_BUFFER, textures[i]);
                        }

                        fullScreenQuad_->render();
                }
        }

}*/
