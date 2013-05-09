// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "D3D9BloomRenderer.h"
#include "../D3D9Renderer.h"

namespace selene
{

        D3d9BloomRenderer::D3d9BloomRenderer()
        {
                d3dDevice_ = nullptr;

                renderTargetContainer_ = nullptr;
                frameParameters_ = nullptr;
                fullScreenQuad_ = nullptr;
                textureHandler_ = nullptr;
                capabilities_ = nullptr;
        }
        D3d9BloomRenderer::~D3d9BloomRenderer()
        {
                destroy();
        }

        //------------------------------------------------------------------------------------------------------------------------
        bool D3d9BloomRenderer::initialize(D3d9RenderTargetContainer& renderTargetContainer,
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
                        D3d9Shader("BrightPass.vsh",  "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, *capabilities_),
                        D3d9Shader("Bloom.vsh",       "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, *capabilities_),
                        D3d9Shader("CombinePass.vsh", "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, *capabilities_)
                };

                D3d9Shader d3dPixelShaders[NUM_OF_PIXEL_SHADERS] =
                {
                        D3d9Shader("BrightPass.psh",  "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_),
                        D3d9Shader("Bloom.psh",       "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_),
                        D3d9Shader("CombinePass.psh", "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_)
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

                return true;
        }

        //------------------------------------------------------------------------------------------------------------------------
        void D3d9BloomRenderer::destroy()
        {
                for(uint32_t i = 0; i < NUM_OF_VERTEX_SHADERS; ++i)
                        vertexShaders_[i].destroy();

                for(uint32_t i = 0; i < NUM_OF_PIXEL_SHADERS; ++i)
                        pixelShaders_[i].destroy();

                d3dDevice_ = nullptr;

                renderTargetContainer_ = nullptr;
                frameParameters_ = nullptr;
                fullScreenQuad_ = nullptr;
                textureHandler_ = nullptr;
                capabilities_ = nullptr;
        }

        //------------------------------------------------------------------------------------------------------------------------
        void D3d9BloomRenderer::renderBloom()
        {
                if(d3dDevice_ == nullptr)
                        return;

                Vector4d imageScale(0.25f);
                Vector4d kernelSize(1.0f / frameParameters_->screenSize.x, 1.0f / frameParameters_->screenSize.y);
                Vector4d blurOffsets[2][NUM_OF_BLUR_OFFSETS];
                const uint8_t shift = static_cast<uint8_t>(NUM_OF_BLUR_OFFSETS * 0.5);
                for(uint8_t i = 0; i < 2; ++i)
                {
                        for(int8_t j = 0; j < NUM_OF_BLUR_OFFSETS; ++j)
                        {
                                static_cast<float*>(blurOffsets[i][j])[i] = static_cast<float>(j - shift) *
                                                                            static_cast<float*>(kernelSize)[i];
                        }
                }

                // bright pass
                d3dDevice_->SetRenderTarget(0, renderTargetContainer_->getRenderTarget(RENDER_TARGET_HELPER_0).getSurface());
                d3dDevice_->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

                d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                d3dDevice_->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
                d3dDevice_->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

                d3dDevice_->SetStreamSource(1, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(2, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(3, nullptr, 0, 0);

                vertexShaders_[VERTEX_SHADER_BRIGHT_PASS].set();
                pixelShaders_[PIXEL_SHADER_BRIGHT_PASS].set();

                d3dDevice_->SetVertexShaderConstantF(LOCATION_IMAGE_SCALE,
                                                     static_cast<const float*>(imageScale), 1);

                d3dDevice_->SetPixelShaderConstantF(LOCATION_TEXTURE_COORDINATES_ADJUSTMENT,
                                                    static_cast<const float*>(frameParameters_->textureCoordinatesAdjustment), 1);
                d3dDevice_->SetPixelShaderConstantF(LOCATION_BLOOM_PARAMETERS,
                                                    static_cast<const float*>(frameParameters_->bloomParameters), 1);

                textureHandler_->setStageState(LOCATION_INPUT_IMAGE_BRIGHT_PASS,
                                               D3DTEXF_POINT, D3DTEXF_LINEAR, D3DTEXF_LINEAR,
                                               D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(LOCATION_INPUT_IMAGE_BRIGHT_PASS,
                                       renderTargetContainer_->getRenderTarget(RENDER_TARGET_RESULT).getTexture());

                fullScreenQuad_->render();

                // bloom
                vertexShaders_[VERTEX_SHADER_BLOOM_PASS].set();
                pixelShaders_[PIXEL_SHADER_BLOOM_PASS].set();

                d3dDevice_->SetVertexShaderConstantF(LOCATION_IMAGE_SCALE,
                                                     static_cast<const float*>(imageScale), 1);

                d3dDevice_->SetPixelShaderConstantF(LOCATION_TEXTURE_COORDINATES_ADJUSTMENT,
                                                    static_cast<const float*>(frameParameters_->textureCoordinatesAdjustment), 1);
                d3dDevice_->SetPixelShaderConstantF(LOCATION_BLOOM_PARAMETERS,
                                                    static_cast<const float*>(frameParameters_->bloomParameters), 1);

                for(uint8_t i = 0; i < 2; ++i)
                {
                        uint8_t sourceRenderTarget = RENDER_TARGET_HELPER_0 + i;
                        uint8_t resultRenderTarget = RENDER_TARGET_HELPER_1 - i;

                        d3dDevice_->SetRenderTarget(0, renderTargetContainer_->getRenderTarget(resultRenderTarget).getSurface());
                        d3dDevice_->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

                        d3dDevice_->SetPixelShaderConstantF(LOCATION_BLUR_OFFSETS,
                                                            reinterpret_cast<const float*>(blurOffsets[i]), NUM_OF_BLUR_OFFSETS);

                        textureHandler_->setStageState(LOCATION_INPUT_IMAGE_BLOOM_PASS,
                                                       D3DTEXF_POINT, D3DTEXF_LINEAR, D3DTEXF_LINEAR,
                                                       D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
                        d3dDevice_->SetTexture(LOCATION_INPUT_IMAGE_BLOOM_PASS,
                                               renderTargetContainer_->getRenderTarget(sourceRenderTarget).getTexture());

                        fullScreenQuad_->render();
                }

                // combine pass
                d3dDevice_->SetRenderTarget(0, renderTargetContainer_->getRenderTarget(RENDER_TARGET_HELPER_1).getSurface());

                vertexShaders_[VERTEX_SHADER_COMBINE_PASS].set();
                pixelShaders_[PIXEL_SHADER_COMBINE_PASS].set();

                d3dDevice_->SetVertexShaderConstantF(LOCATION_IMAGE_SCALE,
                                                     static_cast<const float*>(imageScale), 1);

                d3dDevice_->SetPixelShaderConstantF(LOCATION_TEXTURE_COORDINATES_ADJUSTMENT,
                                                    static_cast<const float*>(frameParameters_->textureCoordinatesAdjustment), 1);

                textureHandler_->setStageState(LOCATION_BLOOM_IMAGE_COMBINE_PASS,
                                               D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_LINEAR,
                                               D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(LOCATION_BLOOM_IMAGE_COMBINE_PASS,
                                       renderTargetContainer_->getRenderTarget(RENDER_TARGET_HELPER_0).getTexture());

                textureHandler_->setStageState(LOCATION_ORIGINAL_IMAGE_COMBINE_PASS,
                                               D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE,
                                               D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(LOCATION_ORIGINAL_IMAGE_COMBINE_PASS,
                                       renderTargetContainer_->getRenderTarget(RENDER_TARGET_RESULT).getTexture());

                fullScreenQuad_->render();
        }

}
