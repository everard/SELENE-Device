// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "D3D9RenderTargetContainer.h"
#include "../D3D9Renderer.h"

namespace selene
{

        // Returns nearest power of two
        uint32_t getNearestPowerOfTwo(uint32_t x)
        {
                --x;
                x |= x >> 1;
                x |= x >> 2;
                x |= x >> 4;
                x |= x >> 8;
                x |= x >> 16;
                return ++x;
        }

        D3d9RenderTargetContainer::D3d9RenderTargetContainer()
        {
                frameParameters_ = nullptr;
                capabilities_ = nullptr;
                parameters_ = nullptr;
        }
        D3d9RenderTargetContainer::~D3d9RenderTargetContainer()
        {
                destroy();
        }

        //------------------------------------------------------------------------------------
        bool D3d9RenderTargetContainer::initialize(D3d9FrameParameters& frameParameters,
                                                   Renderer::Parameters& parameters,
                                                   D3d9Capabilities& capabilities)
        {
                destroy();

                frameParameters_ = &frameParameters;
                capabilities_ = &capabilities;
                parameters_ = &parameters;

                LPDIRECT3DDEVICE9 d3dDevice = D3d9Renderer::getDevice();
                if(d3dDevice == nullptr)
                        return false;

                // prepare parameters
                uint32_t halfWidth  = parameters_->getWidth()  / 2;
                uint32_t halfHeight = parameters_->getHeight() / 2;
                uint32_t nearestPowerOfTwo = parameters_->getWidth() > parameters_->getHeight() ?
                                             parameters_->getWidth() : parameters_->getHeight();

                nearestPowerOfTwo = getNearestPowerOfTwo(nearestPowerOfTwo);
                if(nearestPowerOfTwo < parameters_->getWidth() ||
                   nearestPowerOfTwo < parameters_->getHeight())
                        nearestPowerOfTwo += nearestPowerOfTwo;

                uint32_t shadowMapSize = nearestPowerOfTwo / 2;

                frameParameters_->shadowMapKernelSize.define(1.0f / static_cast<float>(shadowMapSize));
                frameParameters_->screenSize.define(static_cast<float>(parameters_->getWidth()),
                                                    static_cast<float>(parameters_->getHeight()),
                                                    static_cast<float>(halfWidth),
                                                    static_cast<float>(halfHeight));

                frameParameters_->textureCoordinatesAdjustment.define( 1.0f / static_cast<float>(parameters_->getWidth()),
                                                                      -1.0f / static_cast<float>(parameters_->getHeight()),
                                                                       0.5f / static_cast<float>(parameters_->getWidth()),
                                                                       0.5f / static_cast<float>(parameters_->getHeight()));

                D3DFORMAT renderTargetFormats[NUM_OF_RENDER_TARGETS];
                for(uint8_t i = 0; i < NUM_OF_RENDER_TARGETS; ++i)
                        renderTargetFormats[i] = D3DFMT_A8R8G8B8;

                if(capabilities_->isR32fRenderTargetFormatSupported())
                        renderTargetFormats[RENDER_TARGET_POSITIONS] = D3DFMT_R32F;

                uint32_t widths [NUM_OF_RENDER_TARGETS];
                uint32_t heights[NUM_OF_RENDER_TARGETS];

                for(uint8_t i = 0; i < RENDER_TARGET_HALF_SIZE_HELPER_0; ++i)
                {
                        widths[i]  = parameters_->getWidth();
                        heights[i] = parameters_->getHeight();
                }

                for(uint8_t i = RENDER_TARGET_HALF_SIZE_HELPER_0; i < NUM_OF_RENDER_TARGETS; ++i)
                {
                        widths[i]  = halfWidth;
                        heights[i] = halfHeight;
                }

                D3DSURFACE_DESC surfaceDescriptor;

                // create new depth-stencil surface
                LPDIRECT3DSURFACE9 depthStencilSurface = nullptr;
                LPDIRECT3DSURFACE9 backBufferSurface = nullptr;

                if(FAILED(d3dDevice->GetRenderTarget(0, &backBufferSurface)))
                        return false;

                if(FAILED(d3dDevice->GetDepthStencilSurface(&depthStencilSurface)))
                {
                        SAFE_RELEASE(backBufferSurface);
                        return false;
                }

                if(FAILED(depthStencilSurface->GetDesc(&surfaceDescriptor)))
                {
                        SAFE_RELEASE(depthStencilSurface);
                        SAFE_RELEASE(backBufferSurface);
                        return false;
                }

                LPDIRECT3DSURFACE9 newDepthStencilSurface = nullptr;
                if(FAILED(d3dDevice->CreateDepthStencilSurface(nearestPowerOfTwo, nearestPowerOfTwo,
                                                               surfaceDescriptor.Format,
                                                               surfaceDescriptor.MultiSampleType,
                                                               surfaceDescriptor.MultiSampleQuality,
                                                               FALSE,
                                                               &newDepthStencilSurface,
                                                               nullptr)))
                {
                        SAFE_RELEASE(depthStencilSurface);
                        SAFE_RELEASE(backBufferSurface);
                        return false;
                }

                depthStencilSurface->Release();
                depthStencilSurface = newDepthStencilSurface;

                if(FAILED(d3dDevice->SetDepthStencilSurface(depthStencilSurface)))
                {
                        SAFE_RELEASE(depthStencilSurface);
                        SAFE_RELEASE(backBufferSurface);
                        return false;
                }

                backBuffer_.initialize(nullptr, backBufferSurface, depthStencilSurface);

                // create shadow map
                if(!shadowMap_.initialize(shadowMapSize, shadowMapSize,
                                          renderTargetFormats[RENDER_TARGET_POSITIONS],
                                          &surfaceDescriptor))
                {
                        destroy();
                        return false;
                }

                // create rest of the render targets
                for(uint8_t i = 0; i < NUM_OF_RENDER_TARGETS; ++i)
                {
                        if(!renderTargets_[i].initialize(widths[i], heights[i], renderTargetFormats[i], nullptr))
                        {
                                destroy();
                                return false;
                        }
                }

                return true;
        }

        //------------------------------------------------------------------------------------
        void D3d9RenderTargetContainer::destroy()
        {
                for(uint8_t i = 0; i < NUM_OF_RENDER_TARGETS; ++i)
                        renderTargets_[i].destroy();

                dummyRenderTarget_.destroy();
                backBuffer_.destroy();
                shadowMap_.destroy();
        }

        //------------------------------------------------------------------------------------
        const D3d9RenderTarget& D3d9RenderTargetContainer::getRenderTarget(uint8_t type) const
        {
                if(type >= NUM_OF_RENDER_TARGETS)
                        return dummyRenderTarget_;

                return renderTargets_[type];
        }

        //------------------------------------------------------------------------------------
        const D3d9RenderTarget& D3d9RenderTargetContainer::getBackBuffer() const
        {
                return backBuffer_;
        }

        //------------------------------------------------------------------------------------
        const D3d9RenderTarget& D3d9RenderTargetContainer::getShadowMap() const
        {
                return shadowMap_;
        }

}
