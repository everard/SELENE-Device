// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "../../../Application/WindowsApplication.h"
#include "D3D9Capabilities.h"

namespace selene
{

        D3d9Capabilities::D3d9Capabilities():
                isR32fRenderTargetFormatSupported_(false), isMultipleRenderTargetSupported_(false),
                isThirdShaderModelSupported_(false), maxTextureAnisotropy_(1), maxTextureSize_(1) {}
        D3d9Capabilities::~D3d9Capabilities() {}

        //------------------------------------------------------------------------------------------
        bool D3d9Capabilities::createCompatibleDevice(LPDIRECT3D9 d3d,
                                                      const Renderer::Parameters& parameters,
                                                      Renderer::EffectsList& effectsList,
                                                      D3DPRESENT_PARAMETERS& d3dPresentParameters,
                                                      LPDIRECT3DDEVICE9& d3dDevice)
        {
                WindowsApplication* windowsApplication =
                        dynamic_cast<WindowsApplication*>(parameters.getApplication());
                if(windowsApplication == nullptr)
                        return false;

                // get display mode
                D3DDISPLAYMODE d3dDisplayMode;
                if(FAILED(d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3dDisplayMode)))
                        return false;

                memset(&d3dPresentParameters, 0, sizeof(D3DPRESENT_PARAMETERS));

                d3dPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
                d3dPresentParameters.BackBufferFormat = d3dDisplayMode.Format;
                if(parameters.isFullScreenEnabled())
                        d3dPresentParameters.Windowed = FALSE;
                else
                        d3dPresentParameters.Windowed = TRUE;

                d3dPresentParameters.BackBufferWidth  = parameters.getWidth();
                d3dPresentParameters.BackBufferHeight = parameters.getHeight();
                d3dPresentParameters.EnableAutoDepthStencil = TRUE;
                d3dPresentParameters.AutoDepthStencilFormat = D3DFMT_D24S8;
                d3dPresentParameters.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
                d3dPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

                D3DCAPS9 d3dCaps;
                d3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dCaps);

                if(d3dCaps.PixelShaderVersion < D3DPS_VERSION(2, 0))
                        return false;

                maxTextureAnisotropy_ = d3dCaps.MaxAnisotropy;
                if(maxTextureAnisotropy_ < 1)
                        maxTextureAnisotropy_ = 1;
                else if(maxTextureAnisotropy_ > 8)
                        maxTextureAnisotropy_ = 8;

                maxTextureSize_ = d3dCaps.MaxTextureWidth < d3dCaps.MaxTextureHeight ?
                                  d3dCaps.MaxTextureWidth : d3dCaps.MaxTextureHeight;

                isThirdShaderModelSupported_ = false;
                if(d3dCaps.PixelShaderVersion >= D3DPS_VERSION(3, 0))
                        isThirdShaderModelSupported_ = true;

                isMultipleRenderTargetSupported_ = false;
                if(d3dCaps.NumSimultaneousRTs > 1)
                        isMultipleRenderTargetSupported_ = true;

                isR32fRenderTargetFormatSupported_ = true;
                if(FAILED(d3d->CheckDeviceFormat(d3dCaps.AdapterOrdinal, d3dCaps.DeviceType,
                                                 d3dDisplayMode.Format, D3DUSAGE_RENDERTARGET,
                                                 D3DRTYPE_TEXTURE, D3DFMT_R32F)))
                        isR32fRenderTargetFormatSupported_ = false;

                // create device
                DWORD d3dFlags = 0;
                if(IS_SET(d3dCaps.DevCaps, D3DDEVCAPS_HWTRANSFORMANDLIGHT) &&
                   d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1,1))
                        d3dFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
                else
                        d3dFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

                if(IS_SET(d3dCaps.DevCaps, D3DDEVCAPS_PUREDEVICE) &&
                   IS_SET(d3dFlags, D3DCREATE_HARDWARE_VERTEXPROCESSING))
                        d3dFlags |= D3DCREATE_PUREDEVICE;

                // create D3D9 device
                if(FAILED(d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
                                            windowsApplication->getWindowHandle(),
                                            d3dFlags, &d3dPresentParameters, &d3dDevice)))
                        return false;

                if(d3dDevice == nullptr)
                        return false;

                try
                {
                        Effect ssao =
                        {
                                "SSAO", 1,
                                {
                                        {"Radius", 0.0f, std::numeric_limits<float>::max(), 2.5f},
                                        {"Normal influence bias", -1.0f, 1.0f, -0.2f},
                                        {"Minimum cosine alpha", -1.0f, 1.0f, 0.99f}
                                }
                        };

                        Effect bloom =
                        {
                                "Bloom", 1,
                                {
                                        {"Luminance", 0.0f, 1.0f, 0.08f},
                                        {"Scale", 0.0f, std::numeric_limits<float>::max(), 1.5f}
                                }
                        };

                        Effect shadows =
                        {
                                "Shadows", 1, {}
                        };

                        effectsList.push_back(ssao);
                        effectsList.push_back(bloom);
                        effectsList.push_back(shadows);
                }
                catch(...)
                {
                        return false;
                }

                return true;
        }

        //------------------------------------------------------------------------------------------
        bool D3d9Capabilities::isR32fRenderTargetFormatSupported() const
        {
                return isR32fRenderTargetFormatSupported_;
        }

        //------------------------------------------------------------------------------------------
        bool D3d9Capabilities::isMultipleRenderTargetSupported() const
        {
                return isMultipleRenderTargetSupported_;
        }

        //------------------------------------------------------------------------------------------
        bool D3d9Capabilities::isThirdShaderModelSupported() const
        {
                return isThirdShaderModelSupported_;
        }

        //------------------------------------------------------------------------------------------
        DWORD D3d9Capabilities::getMaxTextureAnisotropy() const
        {
                return maxTextureAnisotropy_;
        }

        //------------------------------------------------------------------------------------------
        DWORD D3d9Capabilities::getMaxTextureSize() const
        {
                return maxTextureSize_;
        }

}
