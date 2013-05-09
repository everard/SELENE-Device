// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "D3D9RenderTarget.h"
#include "../D3D9Renderer.h"

namespace selene
{

        D3d9RenderTarget::D3d9RenderTarget()
        {
                d3dTexture_ = nullptr;
                d3dSurface_ = nullptr;
                d3dDepthStencilSurface_ = nullptr;
        }
        D3d9RenderTarget::~D3d9RenderTarget()
        {
                destroy();
        }

        //-------------------------------------------------------------------------------------------------------
        bool D3d9RenderTarget::initialize(uint32_t width, uint32_t height, D3DFORMAT format,
                                          D3DSURFACE_DESC* depthStencilSurfaceDescriptor)
        {
                LPDIRECT3DDEVICE9 d3dDevice = D3d9Renderer::getDevice();
                if(d3dDevice == nullptr)
                        return false;

                destroy();

                if(FAILED(d3dDevice->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, format,
                                                   D3DPOOL_DEFAULT, &d3dTexture_, nullptr)))
                {
                        destroy();
                        return false;
                }

                if(d3dTexture_ == nullptr)
                        return false;

                if(FAILED(d3dTexture_->GetSurfaceLevel(0, &d3dSurface_)))
                {
                        destroy();
                        return false;
                }

                if(d3dSurface_ == nullptr)
                {
                        destroy();
                        return false;
                }

                if(depthStencilSurfaceDescriptor == nullptr)
                        return true;

                if(FAILED(d3dDevice->CreateDepthStencilSurface(width, height,
                                                               depthStencilSurfaceDescriptor->Format,
                                                               depthStencilSurfaceDescriptor->MultiSampleType,
                                                               depthStencilSurfaceDescriptor->MultiSampleQuality,
                                                               FALSE,
                                                               &d3dDepthStencilSurface_,
                                                               nullptr)))
                {
                        destroy();
                        return false;
                }

                if(d3dDepthStencilSurface_ == nullptr)
                {
                        destroy();
                        return false;
                }

                return true;
        }

        //-------------------------------------------------------------------------------------------------------
        void D3d9RenderTarget::initialize(LPDIRECT3DTEXTURE9 d3dTexture, LPDIRECT3DSURFACE9 d3dSurface,
                                          LPDIRECT3DSURFACE9 d3dDepthStencilSurface)
        {
                d3dTexture_ = d3dTexture;
                d3dSurface_ = d3dSurface;
                d3dDepthStencilSurface_ = d3dDepthStencilSurface;
        }

        //-------------------------------------------------------------------------------------------------------
        void D3d9RenderTarget::destroy()
        {
                SAFE_RELEASE(d3dTexture_);
                SAFE_RELEASE(d3dSurface_);
                SAFE_RELEASE(d3dDepthStencilSurface_);
        }

        //-------------------------------------------------------------------------------------------------------
        LPDIRECT3DTEXTURE9 D3d9RenderTarget::getTexture() const
        {
                return d3dTexture_;
        }

        //-------------------------------------------------------------------------------------------------------
        LPDIRECT3DSURFACE9 D3d9RenderTarget::getSurface() const
        {
                return d3dSurface_;
        }

        //-------------------------------------------------------------------------------------------------------
        LPDIRECT3DSURFACE9 D3d9RenderTarget::getDepthStencilSurface() const
        {
                return d3dDepthStencilSurface_;
        }

}
