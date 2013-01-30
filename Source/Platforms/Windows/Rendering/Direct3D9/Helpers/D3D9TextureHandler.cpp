// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "D3D9TextureHandler.h"
#include "../D3D9Renderer.h"

namespace selene
{

        // Fills dummy white texture
        VOID WINAPI fillWhiteTexture(D3DXVECTOR4* output, const D3DXVECTOR2*, const D3DXVECTOR2*, LPVOID)
        {
                *output = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        // Fills dummy normal map
        VOID WINAPI fillNormalMap(D3DXVECTOR4* output, const D3DXVECTOR2*, const D3DXVECTOR2*, LPVOID)
        {
                *output = D3DXVECTOR4(0.5f, 0.5f, 1.0f, 1.0f);
        }

        D3d9TextureHandler::D3d9TextureHandler()
        {
                for(uint8_t i = 0; i < NUM_OF_DUMMY_TEXTURES; ++i)
                        d3dDummyTextures_[i] = nullptr;
                d3dDevice_ = nullptr;
        }
        D3d9TextureHandler::~D3d9TextureHandler()
        {
                destroy();
        }

        //----------------------------------------------------------------------------------------------------
        bool D3d9TextureHandler::initialize()
        {
                d3dDevice_ = D3d9Renderer::getDevice();
                if(d3dDevice_ == nullptr)
                        return false;

                for(uint32_t i = 0; i < NUM_OF_DUMMY_TEXTURES; ++i)
                {
                        if(FAILED(d3dDevice_->CreateTexture(1, 1, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
                                                            &d3dDummyTextures_[i], nullptr)))
                        {
                                destroy();
                                return false;
                        }
                }

                if(FAILED(D3DXFillTexture(d3dDummyTextures_[DUMMY_TEXTURE_WHITE], fillWhiteTexture, nullptr)) ||
                   FAILED(D3DXFillTexture(d3dDummyTextures_[DUMMY_TEXTURE_NORMAL_MAP], fillNormalMap, nullptr)))
                {
                        destroy();
                        return false;
                }

                return true;
        }

        //----------------------------------------------------------------------------------------------------
        void D3d9TextureHandler::destroy()
        {
                for(uint8_t i = 0; i < NUM_OF_DUMMY_TEXTURES; ++i)
                {
                        SAFE_RELEASE(d3dDummyTextures_[i]);
                }
                d3dDevice_ = nullptr;
        }

        //----------------------------------------------------------------------------------------------------
        void D3d9TextureHandler::setTexture(const Resource::Instance<Texture>& texture, DWORD stage,
                                            uint8_t dummyTextureIndex)
        {
                setTexture(*texture, stage, dummyTextureIndex);
        }

        //----------------------------------------------------------------------------------------------------
        void D3d9TextureHandler::setTexture(Texture* texture, DWORD stage, uint8_t dummyTextureIndex)
        {
                if(d3dDevice_ == nullptr)
                        return;

                if(texture == nullptr)
                        d3dDevice_->SetTexture(stage, d3dDummyTextures_[dummyTextureIndex]);
                else
                {
                        D3d9Texture* d3d9Texture = static_cast<D3d9Texture*>(texture);
                        d3dDevice_->SetTexture(stage, d3d9Texture->d3dTexture_);
                }
        }

        //----------------------------------------------------------------------------------------------------
        void D3d9TextureHandler::setStageState(DWORD stage, DWORD magFilter, DWORD minFilter, DWORD mipFilter,
                                               DWORD textureCoordinateModeU, DWORD textureCoordinateModeV)
        {
                if(d3dDevice_ == nullptr)
                        return;

                d3dDevice_->SetSamplerState(stage, D3DSAMP_MAGFILTER, magFilter);
                d3dDevice_->SetSamplerState(stage, D3DSAMP_MINFILTER, minFilter);
                d3dDevice_->SetSamplerState(stage, D3DSAMP_MIPFILTER, mipFilter);
                d3dDevice_->SetSamplerState(stage, D3DSAMP_ADDRESSU,  textureCoordinateModeU);
                d3dDevice_->SetSamplerState(stage, D3DSAMP_ADDRESSV,  textureCoordinateModeV);
        }

}
