// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "D3D9Renderer.h"

#include "../../../../Engine/Scene/Nodes/ParticleSystem.h"
#include "../../../../Engine/Scene/Nodes/Camera.h"
#include "../../../../Engine/Scene/Nodes/Actor.h"
#include "../../Application/WindowsApplication.h"
#include "../../../../Engine/GUI/GUI.h"

#include "Resources/D3D9Texture.h"
#include "Resources/D3D9Mesh.h"

namespace selene
{

        // Fills random texture
        VOID WINAPI fillRandomTexture(D3DXVECTOR4* output, const D3DXVECTOR2* textureCoordinates,
                                      const D3DXVECTOR2* texelSize, LPVOID data)
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

                D3DXVECTOR2 dummy = *textureCoordinates;
                dummy = *texelSize;
                data  = nullptr;
                *output = colors[rand() % 16];
        }

        // Fills white dummy texture
        VOID WINAPI fillWhiteTexture(D3DXVECTOR4* output, const D3DXVECTOR2* textureCoordinates,
                                     const D3DXVECTOR2* texelSize, LPVOID data)
        {
                D3DXVECTOR2 dummy = *textureCoordinates;
                dummy = *texelSize;
                data  = nullptr;
                *output = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        // Fills dummy normal map
        VOID WINAPI fillNormalMap(D3DXVECTOR4* output, const D3DXVECTOR2* textureCoordinates,
                                  const D3DXVECTOR2* texelSize, LPVOID data)
        {
                D3DXVECTOR2 dummy = *textureCoordinates;
                dummy = *texelSize;
                data  = nullptr;
                *output = D3DXVECTOR4(0.5f, 0.5f, 1.0f, 1.0f);
        }

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

        D3d9Renderer::D3d9Renderer(): parameters_(nullptr, nullptr, 0, 0, nullptr, 0)
        {
                d3dDevice_ = nullptr;

                memset(d3dDummyTextures_, 0, sizeof(d3dDummyTextures_));
                d3dRandomTexture_ = nullptr;

                d3dMeshVertexDeclaration_ = nullptr;

                memset(d3dRenderTargetTextures_, 0, sizeof(d3dRenderTargetTextures_));
                memset(d3dRenderTargetSurfaces_, 0, sizeof(d3dRenderTargetSurfaces_));
                memset(d3dHalfSizeRenderTargetTextures_, 0, sizeof(d3dHalfSizeRenderTargetTextures_));
                memset(d3dHalfSizeRenderTargetSurfaces_, 0, sizeof(d3dHalfSizeRenderTargetSurfaces_));
                d3dBackBufferSurface_ = d3dDepthStencilSurface_ = nullptr;
                d3dShadowMapRenderTargetSurface_ = nullptr;
                d3dShadowMapDepthStencilSurface_ = nullptr;
                d3dShadowMapTexture_ = nullptr;

                d3dMaxTextureAnisotropy_ = 1;
                isR32fRenderTargetFormatSupported_ = false;
                isMultipleRenderTargetSupported_ = false;
                isThirdShaderModelSupported_ = false;
        }
        D3d9Renderer::~D3d9Renderer()
        {
                destroy();

                D3d9Device* device = D3d9Device::getInstance();
                if(device != nullptr)
                        device->destroy();
        }

        //--------------------------------------------------------------------------------------------
        bool D3d9Renderer::initialize(const Renderer::Parameters& parameters)
        {
                destroy();

                parameters_ = parameters;
                WindowsApplication* windowsApplication = dynamic_cast<WindowsApplication*>(parameters_.getApplication());

                writeLogEntry("--- Initializing Direct3D 9 renderer ---");
                if(windowsApplication == nullptr)
                {
                        writeLogEntry("ERROR: Could not get windows application");
                        return false;
                }

                setFlags(parameters_.getFlags());

                // check parameters
                if(parameters_.getWidth()  < 128 || parameters_.getWidth()  > 4096 ||
                   parameters_.getHeight() < 128 || parameters_.getHeight() > 4096)
                {
                        writeLogEntry("ERROR: Width and height of the rendering area must be in range [128; 4096]");
                        return false;
                }

                uint32_t halfWidth  = parameters_.getWidth()  >> 1;
                uint32_t halfHeight = parameters_.getHeight() >> 1;

                uint32_t nearestPowerOfTwo = parameters_.getWidth() > parameters_.getHeight() ?
                                             parameters_.getWidth() : parameters_.getHeight();

                nearestPowerOfTwo = getNearestPowerOfTwo(nearestPowerOfTwo);
                if(nearestPowerOfTwo < parameters_.getWidth() ||
                   nearestPowerOfTwo < parameters_.getHeight())
                        nearestPowerOfTwo = nearestPowerOfTwo << 1;

                // create d3d
                D3d9Device* device = D3d9Device::getInstance();
                if(device == nullptr)
                {
                        writeLogEntry("ERROR: Not enough memory");
                        return false;
                }

                device->d3d_ = Direct3DCreate9(D3D_SDK_VERSION);
                if(device->d3d_ == nullptr)
                {
                        writeLogEntry("ERROR: Could not initialize Direct3D");
                        return false;
                }

                // get display mode
                D3DDISPLAYMODE d3dDisplayMode;
                if(FAILED(device->d3d_->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3dDisplayMode)))
                {
                        writeLogEntry("ERROR: Could not get display mode");
                        destroy();
                        return false;
                }

                D3DPRESENT_PARAMETERS d3dPresentParameters;
                ZeroMemory(&d3dPresentParameters, sizeof(D3DPRESENT_PARAMETERS));

                d3dPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
                d3dPresentParameters.BackBufferFormat = d3dDisplayMode.Format;
                if(is(RENDERING_FULL_SCREEN_ENABLED))
                        d3dPresentParameters.Windowed = FALSE;
                else
                        d3dPresentParameters.Windowed = TRUE;

                d3dPresentParameters.BackBufferWidth  = parameters_.getWidth();
                d3dPresentParameters.BackBufferHeight = parameters_.getHeight();
                d3dPresentParameters.EnableAutoDepthStencil = TRUE;
                d3dPresentParameters.AutoDepthStencilFormat = D3DFMT_D24S8;
                d3dPresentParameters.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
                d3dPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

                // check device caps
                D3DCAPS9 d3dCaps;
                device->d3d_->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dCaps);
                d3dMaxTextureAnisotropy_ = d3dCaps.MaxAnisotropy;
                if(d3dMaxTextureAnisotropy_ < 1)
                        d3dMaxTextureAnisotropy_ = 1;
                else if(d3dMaxTextureAnisotropy_ > 8)
                        d3dMaxTextureAnisotropy_ = 8;

                if(d3dCaps.PixelShaderVersion < D3DPS_VERSION(2, 0))
                {
                        destroy();
                        return false;
                }

                isThirdShaderModelSupported_ = false;
                if(d3dCaps.PixelShaderVersion >= D3DPS_VERSION(3, 0))
                {
                        writeLogEntry("PS 3.0 is supported");
                        isThirdShaderModelSupported_ = true;
                }

                isMultipleRenderTargetSupported_ = false;
                if(d3dCaps.NumSimultaneousRTs > 1)
                {
                        writeLogEntry("MRT is supported");
                        isMultipleRenderTargetSupported_ = true;
                }

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
                if(FAILED(device->d3d_->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, windowsApplication->getWindowHandle(),
                                                     d3dFlags, &d3dPresentParameters, &device->d3dDevice_)))
                {
                        writeLogEntry("ERROR: Could not create Direct3D device");
                        destroy();
                        return false;
                }

                if(FAILED(device->d3d_->CheckDeviceFormat(d3dCaps.AdapterOrdinal, d3dCaps.DeviceType,
                                                          d3dDisplayMode.Format, D3DUSAGE_RENDERTARGET,
                                                          D3DRTYPE_TEXTURE, D3DFMT_R32F)))
                {
                        isR32fRenderTargetFormatSupported_ = false;
                }
                else
                {
                        writeLogEntry("R32F texture format is supported");
                        isR32fRenderTargetFormatSupported_ = true;
                }

                d3dDevice_ = D3d9Device::getInterface();
                if(d3dDevice_ == nullptr)
                {
                        writeLogEntry("ERROR: Direct3D device is not created");
                        destroy();
                        return false;
                }

                std::string pixelShaderLibrary, vertexShaderLibrary, emptyLibrary;
                vertexShaderLibrary = "\
                float4 QuatMul(float4 q1, float4 q2)\
                {\
                        float3 im = q1.w * q2.xyz + q1.xyz * q2.w + cross(q1.xyz, q2.xyz);\
                        float4 dt = q1 * q2;\
                        float re = dot(dt, float4(-1.0, -1.0, -1.0, 1.0));\
                        return float4(im, re);\
                }\
                \
                float4 QuatRotate(float3 p, float4 q)\
                {\
                        float4 temp = QuatMul(q, float4(p, 0.0));\
                        return QuatMul(temp, float4(-q.x, -q.y, -q.z, q.w));\
                }\
                \
                float3 Transform(float4 offset, float4 rot, float3 pos)\
                {\
                        return offset.xyz + QuatRotate(pos, rot).xyz;\
                }\n\
                \n";

                pixelShaderLibrary = "\
                float3 EncodeNormal(float3 n)\
                {\
                        return 0.5 * n + 0.5;\
                }\
                \
                half3 DecodeNormal(half3 enc)\
                {\
                        return 2.0 * enc - 1.0;\
                }";

                if(isR32fRenderTargetFormatSupported_)
                {
                        pixelShaderLibrary += "\
                        \
                        float4 EncodePos(float4 pos, float4 proj)\
                        {\
                                return (pos.z / proj.w).xxxx;\
                        }\
                        \
                        float DecodeZEye(float4 ZEnc, float4 proj)\
                        {\
                                return ZEnc.r * proj.w;\
                        }\
                        \
                        float3 DecodePos(float4 ZEnc, float2 XY, float4 proj, float4 unproj)\
                        {\
                                float zv = DecodeZEye(ZEnc, proj);\
                                float4 p = float4(XY, 1.0, 0.0);\
                                float4 orig = p * unproj * zv.xxxx;\
                                return orig.xyz;\
                        }\
                        \n\
                        \n";
                }
                else
                {
                        pixelShaderLibrary += "\
                        float4 EncodePos(float4 pos, float4 proj)\
                        {\
                                float4 enc = float4(16777216.0, 65536.0, 256.0, 1.0) * (pos.z / proj.w);\
                                enc  = frac(enc);\
                                enc -= enc.xxyz * float4(0.0, 1.0 / 256.0, 1.0 / 256.0, 1.0 / 256.0);\
                                return enc;\
                        }\
                        \
                        float DecodeZEye(float4 ZEnc, float4 proj)\
                        {\
                                return dot(ZEnc, float4(1.0 / (256.0 * 256.0 * 256.0), 1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0)) * proj.w;\
                        }\
                        \
                        float3 DecodePos(float4 ZEnc, float2 XY, float4 proj, float4 unproj)\
                        {\
                                float zv = DecodeZEye(ZEnc, proj);\
                                float4 p = float4(XY, 1.0, 0.0);\
                                float4 orig = p * unproj * zv.xxxx;\
                                return orig.xyz;\
                        }\
                        \n\
                        \n";
                }

                // create shaders
                FileManager* fileManager = parameters_.getFileManager();
                D3d9Shader d3dVertexShaders[NUM_OF_VERTEX_SHADERS] =
                {
                        D3d9Shader("Shaders//PositionPass.vsh", fileManager, emptyLibrary, "vs_1_1", 0),
                        D3d9Shader("Shaders//SkinPositionPass.vsh", fileManager, vertexShaderLibrary, "vs_2_0", 0),
                        D3d9Shader("Shaders//NormalsPass.vsh", fileManager, emptyLibrary, "vs_1_1", 0),
                        D3d9Shader("Shaders//SkinNormalsPass.vsh", fileManager, vertexShaderLibrary, "vs_2_0", 0),
                        D3d9Shader("Shaders//DirectionalLightAccPass.vsh", fileManager, emptyLibrary, "vs_1_1", 0),
                        D3d9Shader("Shaders//PointLightAccPass.vsh", fileManager, emptyLibrary, "vs_1_1", 0),
                        D3d9Shader("Shaders//SpotLightAccPass.vsh", fileManager, emptyLibrary, "vs_2_0", 0),
                        D3d9Shader("Shaders//SpotLightShadowPass.vsh", fileManager, emptyLibrary, "vs_1_1", 0),
                        D3d9Shader("Shaders//ShadingPass.vsh", fileManager, emptyLibrary, "vs_1_1", 0),
                        D3d9Shader("Shaders//SkinShadingPass.vsh", fileManager, vertexShaderLibrary, "vs_2_0", 0),
                        D3d9Shader("Shaders//ResultPass.vsh", fileManager, emptyLibrary, "vs_1_1", 0),
                        D3d9Shader("Shaders//SSAOPass.vsh", fileManager, emptyLibrary, "vs_1_1", 0),
                        D3d9Shader("Shaders//SSAOBlurX.vsh", fileManager, emptyLibrary, "vs_1_1", 0),
                        D3d9Shader("Shaders//SSAOBlurY.vsh", fileManager, emptyLibrary, "vs_1_1", 0),
                        D3d9Shader("Shaders//EdgeDetect.vsh", fileManager, emptyLibrary, "vs_1_1", 0),
                        D3d9Shader("Shaders//GUIFramesPass.vsh", fileManager, emptyLibrary, "vs_1_1", 0),
                        D3d9Shader("Shaders//GUITextPass.vsh", fileManager, emptyLibrary, "vs_1_1", 0),
                        D3d9Shader("Shaders//GUICursorPass.vsh", fileManager, emptyLibrary, "vs_1_1", 0),
                        D3d9Shader("Shaders//BrightPass.vsh", fileManager, emptyLibrary, "vs_1_1", 0),
                        D3d9Shader("Shaders//Bloom.vsh", fileManager, emptyLibrary, "vs_1_1", 0),
                        D3d9Shader("Shaders//CombinePass.vsh", fileManager, emptyLibrary, "vs_1_1", 0),
                        D3d9Shader("Shaders//ParticlesPass.vsh", fileManager, emptyLibrary, "vs_1_1", 0)
                };

                D3d9Shader d3dPixelShaders[NUM_OF_PIXEL_SHADERS] =
                {
                        D3d9Shader("Shaders//PositionPass.psh", fileManager, pixelShaderLibrary, "ps_2_0", 0),
                        D3d9Shader("Shaders//NormalsPass.psh", fileManager, pixelShaderLibrary, "ps_2_0", 0),
                        D3d9Shader("Shaders//DirectionalLightAccPass.psh", fileManager, pixelShaderLibrary, "ps_2_0", 0),
                        D3d9Shader("Shaders//PointLightAccPass.psh", fileManager, pixelShaderLibrary, "ps_2_0", 0),
                        D3d9Shader("Shaders//SpotLightAccPass.psh", fileManager, pixelShaderLibrary, "ps_2_0", 0),
                        D3d9Shader("Shaders//SpotLightAccPassWithShadows.psh", fileManager, pixelShaderLibrary, "ps_2_0", 0),
                        D3d9Shader("Shaders//SpotLightShadowPass.psh", fileManager, pixelShaderLibrary, "ps_2_0", 0),
                        D3d9Shader("Shaders//ShadingPass.psh", fileManager, pixelShaderLibrary, "ps_2_0", 0),
                        D3d9Shader("Shaders//ShadingPassSSAO.psh", fileManager, pixelShaderLibrary, "ps_2_0", 0),
                        D3d9Shader("Shaders//ResultPass.psh", fileManager, pixelShaderLibrary, "ps_1_1", 0),
                        D3d9Shader("Shaders//SSAOPass.psh", fileManager, pixelShaderLibrary, "ps_2_0", 0),
                        D3d9Shader("Shaders//SSAOBlurX.psh", fileManager, pixelShaderLibrary, "ps_2_0", 0),
                        D3d9Shader("Shaders//SSAOBlurY.psh", fileManager, pixelShaderLibrary, "ps_2_0", 0),
                        D3d9Shader("Shaders//EdgeDetect.psh", fileManager, pixelShaderLibrary, "ps_2_0", 0),
                        D3d9Shader("Shaders//GUIFramesPass.psh", fileManager, pixelShaderLibrary, "ps_2_0", 0),
                        D3d9Shader("Shaders//GUITextPass.psh", fileManager, pixelShaderLibrary, "ps_2_0", 0),
                        D3d9Shader("Shaders//GUICursorPass.psh", fileManager, pixelShaderLibrary, "ps_2_0", 0),
                        D3d9Shader("Shaders//BrightPass.psh", fileManager, pixelShaderLibrary, "ps_2_0", 0),
                        D3d9Shader("Shaders//BloomX.psh", fileManager, pixelShaderLibrary, "ps_2_0", 0),
                        D3d9Shader("Shaders//BloomY.psh", fileManager, pixelShaderLibrary, "ps_2_0", 0),
                        D3d9Shader("Shaders//CombinePass.psh", fileManager, pixelShaderLibrary, "ps_2_0", 0),
                        D3d9Shader("Shaders//ParticlesPass.psh", fileManager, pixelShaderLibrary, "ps_2_0", 0)
                };

                for(uint32_t i = 0; i < NUM_OF_VERTEX_SHADERS; ++i)
                {
                        if(!vertexShaders_[i].create(d3dVertexShaders[i]))
                        {
                                writeLogEntry("ERROR: Could not create vertex shader");
                                destroy();
                                return false;
                        }
                }

                for(uint32_t i = 0; i < NUM_OF_PIXEL_SHADERS; ++i)
                {
                        if(!pixelShaders_[i].create(d3dPixelShaders[i]))
                        {
                                writeLogEntry("ERROR: Could not create pixel shader");
                                destroy();
                                return false;
                        }
                }

                // create optional shaders
                if(isMultipleRenderTargetSupported_)
                {
                        D3d9Shader d3dOptionalVertexShaders[NUM_OF_MESH_UNITS] =
                        {
                                D3d9Shader("Shaders//PositionNormalsPass.vsh", fileManager, vertexShaderLibrary, "vs_1_1", 0),
                                D3d9Shader("Shaders//SkinPositionNormalsPass.vsh", fileManager, vertexShaderLibrary, "vs_2_0", 0)
                        };

                        D3d9Shader d3dOptionalPixelShader("Shaders//PositionNormalsPass.psh", fileManager, pixelShaderLibrary, "ps_2_0", 0);

                        for(uint32_t i = 0; i < NUM_OF_MESH_UNITS; ++i)
                        {
                                if(!optionalVertexShaders_[OPTIONAL_VERTEX_SHADER_POSITIONS_AND_NORMALS_PASS + i].create(d3dOptionalVertexShaders[i]))
                                {
                                        writeLogEntry("ERROR: Could not create optional vertex shader");
                                        destroy();
                                        return false;
                                }
                        }

                        if(!optionalPixelShaders_[OPTIONAL_PIXEL_SHADER_POSITIONS_AND_NORMALS_PASS].create(d3dOptionalPixelShader))
                        {
                                writeLogEntry("ERROR: Could not create optional pixel shader");
                                destroy();
                                return false;
                        }
                }

                if(isThirdShaderModelSupported_)
                {
                        D3d9Shader d3dOptionalVertexShader("Shaders//SSAO30Pass.vsh", fileManager, vertexShaderLibrary, "vs_1_1", 0);
                        D3d9Shader d3dOptionalPixelShader("Shaders//SSAO30Pass.psh", fileManager, pixelShaderLibrary, "ps_3_0", 0);

                        if(!optionalVertexShaders_[OPTIONAL_VERTEX_SHADER_SSAO_PASS].create(d3dOptionalVertexShader))
                        {
                                writeLogEntry("ERROR: Could not create optional vertex shader");
                                destroy();
                                return false;
                        }

                        if(!optionalPixelShaders_[OPTIONAL_PIXEL_SHADER_SSAO_PASS].create(d3dOptionalPixelShader))
                        {
                                writeLogEntry("ERROR: Could not create optional pixel shader");
                                destroy();
                                return false;
                        }
                }

                // create dummy textures
                for(uint32_t i = 0; i < NUM_OF_DUMMY_TEXTURES; ++i)
                {
                        if(FAILED(d3dDevice_->CreateTexture(1, 1, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
                                                            &d3dDummyTextures_[i], nullptr)))
                        {
                                writeLogEntry("ERROR: Could not create dummy texture");
                                destroy();
                                return false;
                        }
                }

                if(FAILED(D3DXFillTexture(d3dDummyTextures_[DUMMY_TEXTURE_WHITE], fillWhiteTexture, nullptr)) ||
                   FAILED(D3DXFillTexture(d3dDummyTextures_[DUMMY_TEXTURE_NORMAL_MAP], fillNormalMap, nullptr)))
                {
                        writeLogEntry("ERROR: Could not fill dummy texture");
                        destroy();
                        return false;
                }

                // create random texture
                if(FAILED(d3dDevice_->CreateTexture(64, 64, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
                                                    &d3dRandomTexture_, nullptr)))
                {
                        writeLogEntry("ERROR: Could not create random texture");
                        destroy();
                        return false;
                }

                srand(0);
                if(FAILED(D3DXFillTexture(d3dRandomTexture_, fillRandomTexture, nullptr)))
                {
                        writeLogEntry("ERROR: Could not fill random texture");
                        destroy();
                        return false;
                }

                // create mesh vertex declaration
                D3DVERTEXELEMENT9 vertexElements[] =
                {
                        {0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
                        {1, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
                        {1, 12, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0},
                        {2, 0,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
                        {3, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
                        {3, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2},
                        D3DDECL_END()
                };

                if(FAILED(d3dDevice_->CreateVertexDeclaration(vertexElements, &d3dMeshVertexDeclaration_)))
                {
                        writeLogEntry("ERROR: Could not create vertex declaration");
                        destroy();
                        return false;
                }

                // create render targets
                D3DFORMAT d3dRenderTargetFormats[NUM_OF_RENDER_TARGETS] =
                {
                        D3DFMT_A8R8G8B8,
                        D3DFMT_A8R8G8B8,
                        D3DFMT_A8R8G8B8,
                        D3DFMT_A8R8G8B8,
                        D3DFMT_A8R8G8B8
                };

                if(isR32fRenderTargetFormatSupported_)
                        d3dRenderTargetFormats[RENDER_TARGET_POSITIONS] = D3DFMT_R32F;

                for(uint32_t i = 0; i < NUM_OF_RENDER_TARGETS; ++i)
                {
                        if(FAILED(d3dDevice_->CreateTexture(parameters_.getWidth(), parameters_.getHeight(), 1,
                                                            D3DUSAGE_RENDERTARGET, d3dRenderTargetFormats[i],
                                                            D3DPOOL_DEFAULT, &d3dRenderTargetTextures_[i],
                                                            nullptr)))
                        {
                                writeLogEntry("ERROR: Could not create render target");
                                destroy();
                                return false;
                        }

                        if(FAILED(d3dRenderTargetTextures_[i]->GetSurfaceLevel(0, &d3dRenderTargetSurfaces_[i])))
                        {
                                writeLogEntry("ERROR: Could not get render target surface");
                                destroy();
                                return false;
                        }
                }

                // create half-size render targets
                for(uint32_t i = 0; i < NUM_OF_HALF_SIZE_RENDER_TARGETS; ++i)
                {
                        if(FAILED(d3dDevice_->CreateTexture(halfWidth, halfHeight, 1,
                                                            D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
                                                            D3DPOOL_DEFAULT, &d3dHalfSizeRenderTargetTextures_[i],
                                                            nullptr)))
                        {
                                writeLogEntry("ERROR: Could not create half-size render target");
                                destroy();
                                return false;
                        }

                        if(FAILED(d3dHalfSizeRenderTargetTextures_[i]->GetSurfaceLevel(0, &d3dHalfSizeRenderTargetSurfaces_[i])))
                        {
                                writeLogEntry("ERROR: Could not get half-size render target surface");
                                destroy();
                                return false;
                        }
                }

                // create shadow map
                uint32_t shadowMapSize = nearestPowerOfTwo >> 1;
                if(FAILED(d3dDevice_->CreateTexture(shadowMapSize, shadowMapSize, 1,
                                                    D3DUSAGE_RENDERTARGET, d3dRenderTargetFormats[RENDER_TARGET_POSITIONS],
                                                    D3DPOOL_DEFAULT, &d3dShadowMapTexture_, nullptr)))
                {
                        writeLogEntry("ERROR: Could not create shadow map");
                        destroy();
                        return false;
                }

                if(FAILED(d3dShadowMapTexture_->GetSurfaceLevel(0, &d3dShadowMapRenderTargetSurface_)))
                {
                        writeLogEntry("ERROR: Could not get shadow map render target surface");
                        destroy();
                        return false;
                }

                // create helpers
                if(!particlesRenderer_.initialize() ||
                   !fullScreenQuad_.initialize() ||
                   !lightsGeometry_.initialize() ||
                   !ssaoGeometry_.initialize()   ||
                   !guiRenderer_.initialize(fileManager))
                {
                        writeLogEntry("ERROR: Could not initialize helper geometry");
                        destroy();
                        return false;
                }

                // get default render target
                if(FAILED(d3dDevice_->GetRenderTarget(0, &d3dBackBufferSurface_)))
                {
                        writeLogEntry("ERROR: Could not get default render target surface");
                        destroy();
                        return false;
                }

                if(d3dBackBufferSurface_ == nullptr)
                {
                        writeLogEntry("ERROR: Could not get default render target surface");
                        destroy();
                        return false;
                }

                // get default depth stencil surface
                if(FAILED(d3dDevice_->GetDepthStencilSurface(&d3dDepthStencilSurface_)))
                {
                        writeLogEntry("ERROR: Could not get default depth stencil surface");
                        destroy();
                        return false;
                }

                if(d3dDepthStencilSurface_ == nullptr)
                {
                        writeLogEntry("ERROR: Could not get default depth stencil surface");
                        destroy();
                        return false;
                }

                // create depth stencil surfaces for scene and shadow map
                D3DSURFACE_DESC d3dSurfaceDesc;
                if(FAILED(d3dDepthStencilSurface_->GetDesc(&d3dSurfaceDesc)))
                {
                        writeLogEntry("ERROR: Could not get default depth stencil surface description");
                        destroy();
                        return false;
                }

                if(FAILED(d3dDevice_->CreateDepthStencilSurface(shadowMapSize, shadowMapSize,
                                                                d3dSurfaceDesc.Format,
                                                                d3dSurfaceDesc.MultiSampleType,
                                                                d3dSurfaceDesc.MultiSampleQuality,
                                                                FALSE,
                                                                &d3dShadowMapDepthStencilSurface_,
                                                                nullptr)))
                {
                        writeLogEntry("ERROR: Could not create depth stencil surface for shadow map");
                        destroy();
                        return false;
                }

                if(d3dShadowMapDepthStencilSurface_ == nullptr)
                {
                        writeLogEntry("ERROR: Could not create depth stencil surface for shadow map");
                        destroy();
                        return false;
                }

                LPDIRECT3DSURFACE9 newDepthStencilSurface = nullptr;
                if(FAILED(d3dDevice_->CreateDepthStencilSurface(nearestPowerOfTwo, nearestPowerOfTwo,
                                                                d3dSurfaceDesc.Format,
                                                                d3dSurfaceDesc.MultiSampleType,
                                                                d3dSurfaceDesc.MultiSampleQuality,
                                                                FALSE,
                                                                &newDepthStencilSurface,
                                                                nullptr)))
                {
                        writeLogEntry("ERROR: Could not create new depth stencil surface");
                        destroy();
                        return false;
                }

                if(newDepthStencilSurface == nullptr)
                {
                        writeLogEntry("ERROR: Could not create new depth stencil surface");
                        destroy();
                        return false;
                }

                // release old depth stencil surface and set new
                d3dDepthStencilSurface_->Release();
                d3dDepthStencilSurface_ = newDepthStencilSurface;

                if(FAILED(d3dDevice_->SetDepthStencilSurface(d3dDepthStencilSurface_)))
                {
                        writeLogEntry("ERROR: Could not set new depth stencil surface");
                        destroy();
                        return false;
                }

                // set screen size
                screenSize_.define(static_cast<float>(parameters_.getWidth()),
                                   static_cast<float>(parameters_.getHeight()),
                                   static_cast<float>(halfWidth),
                                   static_cast<float>(halfHeight));

                // set shadow map kernel size
                shadowMapKernelSize_.define(1.0f / static_cast<float>(shadowMapSize));

                // compute texture coordinates adjustment to directly map texels to pixels
                textureCoordinatesAdjustment_.define( 1.0f / static_cast<float>(parameters_.getWidth()),
                                                     -1.0f / static_cast<float>(parameters_.getHeight()),
                                                      0.5f / static_cast<float>(parameters_.getWidth()),
                                                      0.5f / static_cast<float>(parameters_.getHeight()));

                d3dDevice_->SetRenderState(D3DRS_LIGHTING, FALSE);
                d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                d3dDevice_->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

                return true;
        }

        //--------------------------------------------------------------------------------------------
        void D3d9Renderer::destroy()
        {
                for(uint32_t i = 0; i < NUM_OF_OPTIONAL_VERTEX_SHADERS; ++i)
                        optionalVertexShaders_[i].destroy();

                for(uint32_t i = 0; i < NUM_OF_OPTIONAL_PIXEL_SHADERS; ++i)
                        optionalPixelShaders_[i].destroy();

                for(uint32_t i = 0; i < NUM_OF_VERTEX_SHADERS; ++i)
                        vertexShaders_[i].destroy();

                for(uint32_t i = 0; i < NUM_OF_PIXEL_SHADERS; ++i)
                        pixelShaders_[i].destroy();

                particlesRenderer_.destroy();
                fullScreenQuad_.destroy();
                lightsGeometry_.destroy();
                ssaoGeometry_.destroy();
                guiRenderer_.destroy();

                for(uint8_t i = 0; i < NUM_OF_DUMMY_TEXTURES; ++i)
                        SAFE_RELEASE(d3dDummyTextures_[i]);
                SAFE_RELEASE(d3dRandomTexture_);

                SAFE_RELEASE(d3dMeshVertexDeclaration_);

                for(uint8_t i = 0; i < NUM_OF_RENDER_TARGETS; ++i)
                {
                        SAFE_RELEASE(d3dRenderTargetTextures_[i]);
                        SAFE_RELEASE(d3dRenderTargetSurfaces_[i]);
                }

                for(uint8_t i = 0; i < NUM_OF_HALF_SIZE_RENDER_TARGETS; ++i)
                {
                        SAFE_RELEASE(d3dHalfSizeRenderTargetTextures_[i]);
                        SAFE_RELEASE(d3dHalfSizeRenderTargetSurfaces_[i]);
                }

                SAFE_RELEASE(d3dBackBufferSurface_);
                SAFE_RELEASE(d3dDepthStencilSurface_);
                SAFE_RELEASE(d3dShadowMapRenderTargetSurface_);
                SAFE_RELEASE(d3dShadowMapDepthStencilSurface_);
                SAFE_RELEASE(d3dShadowMapTexture_);

                D3d9Device* device = D3d9Device::getInstance();
                if(device != nullptr)
                {
                        SAFE_RELEASE(device->d3dDevice_);
                        SAFE_RELEASE(device->d3d_);
                }
        }

        //--------------------------------------------------------------------------------------------
        void D3d9Renderer::render(const Camera& camera)
        {
                if(d3dDevice_ == nullptr)
                        return;

                guiRenderer_.setGui(gui_);

                // get matrices
                viewProjectionMatrix_ = camera.getViewProjectionMatrix();
                projectionMatrix_ = camera.getProjectionMatrix();
                viewMatrix_ = camera.getViewMatrix();

                normalsMatrix_ = viewMatrix_;
                normalsMatrix_.invert();
                viewInvMatrix_ = normalsMatrix_;
                normalsMatrix_.transpose();

                // get vectors
                const Matrix& projectionInvMatrix = camera.getProjectionInvMatrix();
                projectionParameters_ = camera.getProjectionParameters();
                unprojectionVector_.define(projectionInvMatrix.a[0][0],
                                           projectionInvMatrix.a[1][1],
                                           1.0, 0.0);

                // begin rendering
                if(FAILED(d3dDevice_->BeginScene()))
                        return;

                renderPositionsAndNormals();
                accumulateLighting();

                if(is(RENDERING_SSAO_ENABLED))
                        renderSsao();

                shade();
                renderParticles();

                uint8_t resultRenderTarget = RENDER_TARGET_NORMALS;
                if(is(RENDERING_BLOOM_ENABLED))
                {
                        renderBloom();
                        resultRenderTarget = RENDER_TARGET_BLURRED_SSAO;
                }

                // render result
                d3dDevice_->SetStreamSource(1, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(2, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(3, nullptr, 0, 0);

                d3dDevice_->SetRenderTarget(0, d3dBackBufferSurface_);
                d3dDevice_->Clear(0, nullptr, D3DCLEAR_TARGET,
                                  D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

                vertexShaders_[VERTEX_SHADER_RESULT_PASS].set();
                pixelShaders_[PIXEL_SHADER_RESULT_PASS].set();

                d3dDevice_->SetVertexShaderConstantF(0, (const float*)textureCoordinatesAdjustment_, 1);

                d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                d3dDevice_->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

                d3dDevice_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(0, d3dRenderTargetTextures_[resultRenderTarget]);

                fullScreenQuad_.render();

                // render GUI
                if(guiRenderer_.prepare())
                {
                        d3dDevice_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
                        d3dDevice_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
                        d3dDevice_->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

                        d3dDevice_->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
                        d3dDevice_->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
                        d3dDevice_->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

                        vertexShaders_[VERTEX_SHADER_GUI_FRAMES_PASS].set();
                        pixelShaders_[PIXEL_SHADER_GUI_FRAMES_PASS].set();

                        guiRenderer_.renderFrames();

                        vertexShaders_[VERTEX_SHADER_GUI_TEXT_PASS].set();
                        pixelShaders_[PIXEL_SHADER_GUI_TEXT_PASS].set();

                        d3dDevice_->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x000000AA);
                        d3dDevice_->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
                        d3dDevice_->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

                        guiRenderer_.renderText();

                        if(!gui_->is(GUI_DISABLED))
                        {
                                vertexShaders_[VERTEX_SHADER_GUI_CURSOR_PASS].set();
                                pixelShaders_[PIXEL_SHADER_GUI_CURSOR_PASS].set();

                                guiRenderer_.renderCursor();
                        }

                        d3dDevice_->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
                        d3dDevice_->SetRenderState(D3DRS_ALPHATESTENABLE,  FALSE);
                }

                // end rendering
                d3dDevice_->SetStreamSource(0, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(1, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(2, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(3, nullptr, 0, 0);
                d3dDevice_->EndScene();
                d3dDevice_->Present(nullptr, nullptr, 0, nullptr);

                // clear rendering lists
                clearLists();
        }

        //--------------------------------------------------------------------------------------------
        void D3d9Renderer::writeLogEntry(const char* entry)
        {
                if(parameters_.getLog() != nullptr)
                        (*parameters_.getLog()) << entry << std::endl;
        }

        //--------------------------------------------------------------------------------------------
        void D3d9Renderer::setTexture(const Resource::Instance<Texture>& texture, DWORD pass,
                                      uint8_t dummyTextureIndex)
        {
                setTexture(*texture, pass, dummyTextureIndex);
        }

        //--------------------------------------------------------------------------------------------
        void D3d9Renderer::setTexture(Texture* texture, DWORD pass, uint8_t dummyTextureIndex)
        {
                if(texture == nullptr)
                        d3dDevice_->SetTexture(pass, d3dDummyTextures_[dummyTextureIndex]);
                else
                {
                        D3d9Texture* d3d9Texture = static_cast<D3d9Texture*>(texture);
                        d3dDevice_->SetTexture(pass, d3d9Texture->d3dTexture_);
                }
        }

        //--------------------------------------------------------------------------------------------
        void D3d9Renderer::setSkeletonPose(const Array<Skeleton::Transform, uint16_t>& boneTransforms)
        {
                if(boneTransforms.isEmpty())
                        return;

                static Quaternion rotations[100];
                static Vector4d   positions[100];

                uint16_t numBoneTransforms = (boneTransforms.getSize() > 100) ? 100 : boneTransforms.getSize();

                for(uint16_t i = 0; i < numBoneTransforms; ++i)
                {
                        rotations[i] = boneTransforms[i].rotation;
                        positions[i].define(boneTransforms[i].position, 0.0f);
                }

                d3dDevice_->SetVertexShaderConstantF(12, reinterpret_cast<const float*>(rotations), numBoneTransforms);
                d3dDevice_->SetVertexShaderConstantF(112, reinterpret_cast<const float*>(positions), numBoneTransforms);
        }

        //--------------------------------------------------------------------------------------------
        void D3d9Renderer::setupNormalsPass(const Material& material)
        {
                Vector4d specularParameters(material.getGlossiness() * 0.005f);

                setTexture(material.getTextureMap(TEXTURE_MAP_NORMAL), 0, DUMMY_TEXTURE_NORMAL_MAP);
                setTexture(material.getTextureMap(TEXTURE_MAP_SPECULAR), 1, DUMMY_TEXTURE_WHITE);
                d3dDevice_->SetPixelShaderConstantF(1, static_cast<const float*>(specularParameters), 1);
        }

        //--------------------------------------------------------------------------------------------
        void D3d9Renderer::setupShadingPass(const Material& material)
        {
                Vector4d ambientColor( material.getColor(MATERIAL_COLOR_AMBIENT),  1.0);
                Vector4d diffuseColor( material.getColor(MATERIAL_COLOR_DIFFUSE),  1.0);
                Vector4d specularColor(material.getColor(MATERIAL_COLOR_SPECULAR), 1.0);
                Vector4d specularParameters(material.getSpecularLevel(), material.getGlossiness());

                setTexture(material.getTextureMap(TEXTURE_MAP_AMBIENT),  0, DUMMY_TEXTURE_WHITE);
                setTexture(material.getTextureMap(TEXTURE_MAP_DIFFUSE),  1, DUMMY_TEXTURE_WHITE);
                setTexture(material.getTextureMap(TEXTURE_MAP_SPECULAR), 2, DUMMY_TEXTURE_WHITE);

                d3dDevice_->SetPixelShaderConstantF(0, (const float*)ambientColor, 1);
                d3dDevice_->SetPixelShaderConstantF(1, (const float*)diffuseColor, 1);
                d3dDevice_->SetPixelShaderConstantF(2, (const float*)specularColor, 1);
                d3dDevice_->SetPixelShaderConstantF(3, (const float*)specularParameters, 1);
        }

        //--------------------------------------------------------------------------------------------
        void D3d9Renderer::setupLightAccumulationPass()
        {
                // set positions at sampler 0
                d3dDevice_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(0, d3dRenderTargetTextures_[RENDER_TARGET_POSITIONS]);

                // set normals at sampler 1
                d3dDevice_->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
                d3dDevice_->SetSamplerState(1, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetSamplerState(1, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(1, d3dRenderTargetTextures_[RENDER_TARGET_NORMALS]);

                d3dDevice_->SetVertexShaderConstantF(0, (const float*)viewProjectionMatrix_, 4);
                d3dDevice_->SetVertexShaderConstantF(4, (const float*)normalsMatrix_, 4);
                d3dDevice_->SetVertexShaderConstantF(8, (const float*)viewMatrix_, 4);

                d3dDevice_->SetPixelShaderConstantF(0, (const float*)textureCoordinatesAdjustment_, 1);
                d3dDevice_->SetPixelShaderConstantF(1, (const float*)unprojectionVector_,   1);
                d3dDevice_->SetPixelShaderConstantF(2, (const float*)projectionParameters_, 1);

                // set alpha blend parameters
                d3dDevice_->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ONE);
                d3dDevice_->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
                d3dDevice_->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        }

        //--------------------------------------------------------------------------------------------
        void D3d9Renderer::renderActors(const Mesh::Subset& meshSubset,
                                        const std::vector<Actor*>& actors,
                                        uint8_t meshRenderingUnit,
                                        uint8_t pass)
        {
                switch(pass)
                {
                        case RENDERING_PASS_POSITIONS_AND_NORMALS:
                        case RENDERING_PASS_NORMALS:
                                setupNormalsPass(meshSubset.material);
                                break;

                        case RENDERING_PASS_SHADING:
                                setupShadingPass(meshSubset.material);
                                break;
                }

                for(auto it = actors.begin(); it != actors.end(); ++it)
                {
                        Actor* actor = (*it);
                        if(actor == nullptr)
                                continue;

                        const Actor::ViewProjectionTransform& viewProjectionTransform = actor->getViewProjectionTransform();
                        d3dDevice_->SetVertexShaderConstantF(0, static_cast<const float*>(viewProjectionTransform.getWorldViewProjectionMatrix()), 4);

                        switch(pass)
                        {
                                case RENDERING_PASS_POSITIONS_AND_NORMALS:
                                        d3dDevice_->SetVertexShaderConstantF(4, static_cast<const float*>(viewProjectionTransform.getWorldViewMatrix()), 4);
                                        d3dDevice_->SetVertexShaderConstantF(8, static_cast<const float*>(viewProjectionTransform.getNormalsMatrix()), 4);
                                        break;

                                case RENDERING_PASS_POSITIONS:
                                        d3dDevice_->SetVertexShaderConstantF(4, static_cast<const float*>(viewProjectionTransform.getWorldViewMatrix()), 4);
                                        break;

                                case RENDERING_PASS_NORMALS:
                                        d3dDevice_->SetVertexShaderConstantF(8, static_cast<const float*>(viewProjectionTransform.getNormalsMatrix()), 4);
                                        break;
                        }

                        if(meshRenderingUnit == UNIT_MESH_SKIN)
                                setSkeletonPose(actor->getSkeleton().getFinalBoneTransforms());

                        d3dDevice_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, meshSubset.vertexIndex,
                                                         meshSubset.numVertices, 3 * meshSubset.faceIndex,
                                                         meshSubset.numFaces);
                }
        }

        //--------------------------------------------------------------------------------------------
        void D3d9Renderer::renderShadows(const Mesh::Subset& meshSubset,
                                         const std::vector<Actor*>& actors,
                                         const SpotLight& spotLight,
                                         uint8_t meshRenderingUnit)
        {
                for(auto it = actors.begin(); it != actors.end(); ++it)
                {
                        Actor* actor = (*it);
                        if(actor == nullptr)
                                continue;

                        d3dDevice_->SetVertexShaderConstantF(0, static_cast<const float*>(actor->getWorldMatrix() *
                                                                spotLight.getViewProjectionMatrix()), 4);
                        d3dDevice_->SetVertexShaderConstantF(4, static_cast<const float*>(actor->getWorldMatrix() *
                                                                spotLight.getViewMatrix()), 4);

                        if(meshRenderingUnit == UNIT_MESH_SKIN)
                                setSkeletonPose(actor->getSkeleton().getFinalBoneTransforms());

                        d3dDevice_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, meshSubset.vertexIndex,
                                                         meshSubset.numVertices, 3 * meshSubset.faceIndex,
                                                         meshSubset.numFaces);
                }
        }

        //--------------------------------------------------------------------------------------------
        void D3d9Renderer::createShadowMap(ActorsList& actorsList, const SpotLight& spotLight)
        {
                // set shadow map as render target
                d3dDevice_->SetRenderTarget(0, d3dShadowMapRenderTargetSurface_);
                d3dDevice_->SetDepthStencilSurface(d3dShadowMapDepthStencilSurface_);

                d3dDevice_->Clear(0, nullptr, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET,
                                  D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);

                d3dDevice_->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

                d3dDevice_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

                d3dDevice_->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
                d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                d3dDevice_->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

                // set shaders
                pixelShaders_[PIXEL_SHADER_POSITIONS_PASS].set();
                d3dDevice_->SetPixelShaderConstantF(0, (const float*)spotLight.getProjectionParameters(), 1);

                d3dDevice_->SetVertexDeclaration(d3dMeshVertexDeclaration_);
                d3dDevice_->SetStreamSource(1, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(2, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(3, nullptr, 0, 0);

                // walk through all meshes
                for(uint8_t i = 0; i < NUM_OF_MESH_UNITS; ++i)
                {
                        vertexShaders_[VERTEX_SHADER_POSITIONS_PASS + i].set();

                        for(bool result = actorsList.readFirstElement(i); result; result = actorsList.readNextElement())
                        {
                                D3d9Mesh* d3dMesh = static_cast<D3d9Mesh*>(actorsList.getCurrentKey());

                                if(d3dMesh == nullptr)
                                        break;

                                const Mesh::Data& meshData = d3dMesh->getData();

                                d3dDevice_->SetStreamSource(0, d3dMesh->d3dVertexBuffers_[Mesh::VERTEX_STREAM_POSITIONS], 0,
                                                            meshData.vertices[Mesh::VERTEX_STREAM_POSITIONS].getStride());
                                d3dDevice_->SetIndices(d3dMesh->d3dIndexBuffer_);

                                MeshSubsetsList* meshSubsetsList = actorsList.getCurrentData();

                                if(meshSubsetsList == nullptr)
                                        break;

                                DWORD cullModes[] = {D3DCULL_CCW, D3DCULL_NONE};

                                for(uint8_t j = 0; j < NUM_OF_MATERIAL_UNITS; ++j)
                                {
                                        d3dDevice_->SetRenderState(D3DRS_CULLMODE, cullModes[j]);

                                        for(bool result1 = meshSubsetsList->readFirstElement(j); result1; result1 = meshSubsetsList->readNextElement())
                                        {
                                                auto meshSubset = meshSubsetsList->getCurrentKey();
                                                auto actors     = meshSubsetsList->getCurrentData();

                                                if(meshSubset == nullptr || actors == nullptr)
                                                        break;

                                                renderShadows(*meshSubset, *actors, spotLight, i);
                                        }
                                }
                        }
                }

                // render shadow
                Matrix lightTextureMatrix, lightViewMatrix;

                lightViewMatrix    = viewInvMatrix_ * spotLight.getViewMatrix();
                lightTextureMatrix = viewInvMatrix_ * spotLight.getViewProjectionMatrix();

                // restore original depth stencil surface
                d3dDevice_->SetDepthStencilSurface(d3dDepthStencilSurface_);
                d3dDevice_->SetRenderTarget(0, d3dRenderTargetSurfaces_[RENDER_TARGET_SSAO]);

                d3dDevice_->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
                d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                d3dDevice_->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATEREQUAL);

                // set positions at sampler 0
                d3dDevice_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(0, d3dRenderTargetTextures_[RENDER_TARGET_POSITIONS]);

                // set shadow map at sampler 1
                d3dDevice_->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
                d3dDevice_->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
                d3dDevice_->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(1, d3dShadowMapTexture_);

                vertexShaders_[VERTEX_SHADER_SPOT_LIGHT_SHADOW_PASS].set();
                pixelShaders_[PIXEL_SHADER_SPOT_LIGHT_SHADOW_PASS].set();

                const Vector4d& lightProjectionParameters = spotLight.getProjectionParameters();
                Vector4d bias(lightProjectionParameters.w * 0.0375f);

                d3dDevice_->SetVertexShaderConstantF(0, (const float*)viewProjectionMatrix_, 4);
                d3dDevice_->SetVertexShaderConstantF(4, (const float*)normalsMatrix_, 4);
                d3dDevice_->SetVertexShaderConstantF(8, (const float*)viewMatrix_, 4);

                d3dDevice_->SetPixelShaderConstantF( 0, (const float*)textureCoordinatesAdjustment_, 1);
                d3dDevice_->SetPixelShaderConstantF( 1, (const float*)unprojectionVector_, 1);
                d3dDevice_->SetPixelShaderConstantF( 2, (const float*)projectionParameters_, 1);
                d3dDevice_->SetPixelShaderConstantF( 3, (const float*)lightProjectionParameters, 1);
                d3dDevice_->SetPixelShaderConstantF( 4, (const float*)lightViewMatrix, 4);
                d3dDevice_->SetPixelShaderConstantF( 8, (const float*)lightTextureMatrix, 4);
                d3dDevice_->SetPixelShaderConstantF(12, (const float*)bias, 1);
                d3dDevice_->SetPixelShaderConstantF(13, (const float*)shadowMapKernelSize_, 1);

                if(lightsGeometry_.beginRendering(UNIT_LIGHT_SPOT))
                {
                        Vector4d lightPosition(spotLight.getPosition(),
                                               spotLight.getRadius());

                        Vector4d lightDirection(spotLight.getDirection(),
                                                spotLight.getCosTheta());

                        d3dDevice_->SetVertexShaderConstantF(12, (float*)lightPosition, 1);
                        d3dDevice_->SetVertexShaderConstantF(13, (float*)lightDirection, 1);

                        lightsGeometry_.render(1);
                        lightsGeometry_.endRendering();
                }

                d3dDevice_->SetRenderTarget(0, d3dRenderTargetSurfaces_[RENDER_TARGET_LIGHTS]);
        }

        //--------------------------------------------------------------------------------------------
        void D3d9Renderer::renderLights(uint8_t lightsListType)
        {
                d3dDevice_->SetStreamSource(1, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(2, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(3, nullptr, 0, 0);

                if(lightsListType == LIGHTS_LIST_WITH_SHADOWS)
                {
                        // render with shadows
                        LightsList& lightsList = lightsLists_[lightsListType];

                        for(bool result = lightsList.readFirstElement(UNIT_LIGHT_SPOT); result; result = lightsList.readNextElement())
                        {
                                auto lightData = lightsList.getCurrentData();
                                auto spotLight = static_cast<SpotLight*>(lightsList.getCurrentKey());

                                if(lightData == nullptr || spotLight == nullptr)
                                        break;

                                // create shadow map
                                createShadowMap(lightData->getShadowsList(), *spotLight);

                                // setup light accumulation
                                setupLightAccumulationPass();

                                // set shadow map at sampler 2
                                d3dDevice_->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
                                d3dDevice_->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                                d3dDevice_->SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
                                d3dDevice_->SetSamplerState(2, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
                                d3dDevice_->SetSamplerState(2, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
                                d3dDevice_->SetTexture(2, d3dRenderTargetTextures_[RENDER_TARGET_SSAO]);

                                d3dDevice_->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
                                d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                                d3dDevice_->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATEREQUAL);

                                vertexShaders_[VERTEX_SHADER_SPOT_LIGHT_ACCUMULATION].set();
                                pixelShaders_[PIXEL_SHADER_SPOT_LIGHT_ACCUMULATION_WITH_SHADOWS].set();

                                // render with shadows
                                if(!lightsGeometry_.beginRendering(UNIT_LIGHT_SPOT))
                                        break;

                                Vector4d position(spotLight->getPosition(), spotLight->getRadius());
                                Vector4d direction(spotLight->getDirection(), spotLight->getCosTheta());
                                Vector4d color(spotLight->getColor(), spotLight->getIntensity());

                                d3dDevice_->SetVertexShaderConstantF(12, (float*)position,  1);
                                d3dDevice_->SetVertexShaderConstantF(42, (float*)direction, 1);
                                d3dDevice_->SetVertexShaderConstantF(72, (float*)color, 1);

                                lightsGeometry_.render(1);
                                lightsGeometry_.endRendering();
                        }

                        return;
                }

                // render without shadows
                setupLightAccumulationPass();
                LightsList& lightsList = lightsLists_[lightsListType];

                uint8_t shaderNo = 0;
                for(uint8_t i = UNIT_LIGHT_DIRECTIONAL; i < NUM_OF_LIGHT_UNITS; ++i)
                {
                        d3dDevice_->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
                        d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                        d3dDevice_->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATER);

                        vertexShaders_[VERTEX_SHADER_DIRECTIONAL_LIGHT_ACCUMULATION + shaderNo].set();
                        pixelShaders_[PIXEL_SHADER_DIRECTIONAL_LIGHT_ACCUMULATION + shaderNo].set();

                        if(!lightsGeometry_.beginRendering(i))
                                return;

                        uint32_t numLights = 0;

                        for(bool result = lightsList.readFirstElement(i); result; result = lightsList.readNextElement())
                        {
                                auto light = lightsList.getCurrentKey();

                                if(light == nullptr)
                                        break;

                                Vector4d color(light->getColor(), light->getIntensity());

                                switch(light->getRenderingUnit())
                                {
                                        case UNIT_LIGHT_DIRECTIONAL:
                                        {
                                                DirectionalLight* directionalLight = static_cast<DirectionalLight*>(light);

                                                Vector4d direction(directionalLight->getDirection(), directionalLight->getSize());

                                                d3dDevice_->SetVertexShaderConstantF(12 + numLights, static_cast<float*>(direction), 1);
                                                d3dDevice_->SetVertexShaderConstantF(42 + numLights, static_cast<float*>(color), 1);
                                                break;
                                        }
                                        case UNIT_LIGHT_POINT:
                                        {
                                                PointLight* pointLight = static_cast<PointLight*>(light);

                                                Vector4d position(pointLight->getPosition(), pointLight->getRadius());

                                                d3dDevice_->SetVertexShaderConstantF(12 + numLights, static_cast<float*>(position), 1);
                                                d3dDevice_->SetVertexShaderConstantF(42 + numLights, static_cast<float*>(color), 1);
                                                break;
                                        }

                                        case UNIT_LIGHT_SPOT:
                                        {
                                                SpotLight* spotLight = static_cast<SpotLight*>(light);

                                                Vector4d position(spotLight->getPosition(),   spotLight->getRadius());
                                                Vector4d direction(spotLight->getDirection(), spotLight->getCosTheta());

                                                d3dDevice_->SetVertexShaderConstantF(12 + numLights, static_cast<float*>(position),  1);
                                                d3dDevice_->SetVertexShaderConstantF(42 + numLights, static_cast<float*>(direction), 1);
                                                d3dDevice_->SetVertexShaderConstantF(72 + numLights, static_cast<float*>(color), 1);
                                                break;
                                        }

                                        default:
                                                break;
                                }

                                ++numLights;

                                if(numLights == lightsGeometry_.getBatchSize())
                                {
                                        lightsGeometry_.render(numLights);
                                        numLights = 0;
                                }
                        }

                        if(numLights > 0)
                                lightsGeometry_.render(numLights);

                        lightsGeometry_.endRendering();
                        ++shaderNo;
                }
        }

        //--------------------------------------------------------------------------------------------
        void D3d9Renderer::renderPositionsAndNormals()
        {
                d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                d3dDevice_->SetVertexDeclaration(d3dMeshVertexDeclaration_);
                d3dDevice_->SetStreamSource(3, nullptr, 0, 0);

                if(isMultipleRenderTargetSupported_)
                {
                        DWORD    d3dClearFlags[]   = {D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCLEAR_TARGET};
                        D3DCOLOR d3dClearColors[]  = {D3DCOLOR_XRGB(0, 0, 0), D3DCOLOR_XRGB(128, 128, 128)};
                        uint8_t renderTargetNo[]   = {RENDER_TARGET_POSITIONS, RENDER_TARGET_NORMALS};

                        d3dDevice_->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
                        d3dDevice_->SetRenderState(D3DRS_ZFUNC,        D3DCMP_LESSEQUAL);

                        for(uint8_t i = 0; i < sizeof(renderTargetNo) / sizeof(renderTargetNo[0]); ++i)
                        {
                                d3dDevice_->SetRenderTarget(0, d3dRenderTargetSurfaces_[renderTargetNo[i]]);
                                d3dDevice_->Clear(0, nullptr, d3dClearFlags[i], d3dClearColors[i], 1.0f, 0);
                        }

                        d3dDevice_->SetRenderTarget(0, d3dRenderTargetSurfaces_[RENDER_TARGET_POSITIONS]);
                        d3dDevice_->SetRenderTarget(1, d3dRenderTargetSurfaces_[RENDER_TARGET_NORMALS]);

                        d3dDevice_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
                        d3dDevice_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                        d3dDevice_->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
                        d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP);
                        d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP);

                        d3dDevice_->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
                        d3dDevice_->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                        d3dDevice_->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
                        d3dDevice_->SetSamplerState(1, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP);
                        d3dDevice_->SetSamplerState(1, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP);

                        optionalPixelShaders_[OPTIONAL_PIXEL_SHADER_POSITIONS_AND_NORMALS_PASS].set();
                        d3dDevice_->SetPixelShaderConstantF(0, static_cast<const float*>(projectionParameters_), 1);

                        // walk through all meshes
                        for(uint8_t i = 0; i < NUM_OF_MESH_UNITS; ++i)
                        {
                                optionalVertexShaders_[OPTIONAL_VERTEX_SHADER_POSITIONS_AND_NORMALS_PASS + i].set();

                                for(bool result = actorsList_.readFirstElement(i); result; result = actorsList_.readNextElement())
                                {
                                        D3d9Mesh* d3dMesh = static_cast<D3d9Mesh*>(actorsList_.getCurrentKey());

                                        if(d3dMesh == nullptr)
                                                break;

                                        const Mesh::Data& meshData = d3dMesh->getData();

                                        d3dDevice_->SetStreamSource(0, d3dMesh->d3dVertexBuffers_[Mesh::VERTEX_STREAM_POSITIONS], 0,
                                                                    meshData.vertices[Mesh::VERTEX_STREAM_POSITIONS].getStride());
                                        d3dDevice_->SetStreamSource(1, d3dMesh->d3dVertexBuffers_[Mesh::VERTEX_STREAM_TBN_BASES], 0,
                                                                    meshData.vertices[Mesh::VERTEX_STREAM_TBN_BASES].getStride());
                                        d3dDevice_->SetStreamSource(2, d3dMesh->d3dVertexBuffers_[Mesh::VERTEX_STREAM_TEXTURE_COORDINATES], 0,
                                                                    meshData.vertices[Mesh::VERTEX_STREAM_TEXTURE_COORDINATES].getStride());

                                        if(i == UNIT_MESH_SKIN)
                                                d3dDevice_->SetStreamSource(3, d3dMesh->d3dVertexBuffers_[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS], 0,
                                                                            meshData.vertices[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS].getStride());

                                        d3dDevice_->SetIndices(d3dMesh->d3dIndexBuffer_);

                                        MeshSubsetsList* meshSubsetsList = actorsList_.getCurrentData();

                                        if(meshSubsetsList == nullptr)
                                                break;

                                        DWORD cullModes[] = {D3DCULL_CCW, D3DCULL_NONE};

                                        for(uint8_t j = 0; j < NUM_OF_MATERIAL_UNITS; ++j)
                                        {
                                                d3dDevice_->SetRenderState(D3DRS_CULLMODE, cullModes[j]);

                                                for(bool result1 = meshSubsetsList->readFirstElement(j); result1; result1 = meshSubsetsList->readNextElement())
                                                {
                                                        auto meshSubset = meshSubsetsList->getCurrentKey();
                                                        auto actors     = meshSubsetsList->getCurrentData();

                                                        if(meshSubset == nullptr || actors == nullptr)
                                                                break;

                                                        renderActors(*meshSubset, *actors, i, RENDERING_PASS_POSITIONS_AND_NORMALS);
                                                }
                                        }
                                }
                        }

                        d3dDevice_->SetRenderTarget(1, nullptr);
                }
                else
                {
                        DWORD    d3dClearFlags[]   = {D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCLEAR_TARGET};
                        D3DCOLOR d3dClearColors[]  = {D3DCOLOR_XRGB(0, 0, 0), D3DCOLOR_XRGB(128, 128, 128)};
                        DWORD d3dDepthWriteFlags[] = {TRUE, FALSE};
                        DWORD d3dDepthFunctions[]  = {D3DCMP_LESSEQUAL, D3DCMP_EQUAL};

                        uint8_t pixelShaderNo[]  = {PIXEL_SHADER_POSITIONS_PASS,  PIXEL_SHADER_NORMALS_PASS};
                        uint8_t vertexShaderNo[] = {VERTEX_SHADER_POSITIONS_PASS, VERTEX_SHADER_NORMALS_PASS};
                        uint8_t renderTargetNo[] = {RENDER_TARGET_POSITIONS,      RENDER_TARGET_NORMALS};

                        uint8_t vertexStreamIndices[][3] =
                        {
                                {Mesh::VERTEX_STREAM_POSITIONS, 0, 0},
                                {
                                        Mesh::VERTEX_STREAM_POSITIONS,
                                        Mesh::VERTEX_STREAM_TBN_BASES,
                                        Mesh::VERTEX_STREAM_TEXTURE_COORDINATES
                                }
                        };
                        uint8_t numVertexStreams[] = {1, 3};

                        for(uint8_t pass = RENDERING_PASS_POSITIONS; pass <= RENDERING_PASS_NORMALS; ++pass)
                        {
                                d3dDevice_->SetRenderState(D3DRS_ZWRITEENABLE, d3dDepthWriteFlags[pass]);
                                d3dDevice_->SetRenderState(D3DRS_ZFUNC,        d3dDepthFunctions[pass]);

                                d3dDevice_->SetRenderTarget(0, d3dRenderTargetSurfaces_[renderTargetNo[pass]]);
                                d3dDevice_->Clear(0, nullptr, d3dClearFlags[pass], d3dClearColors[pass], 1.0f, 0);

                                d3dDevice_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
                                d3dDevice_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                                d3dDevice_->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
                                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP);
                                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP);

                                d3dDevice_->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
                                d3dDevice_->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                                d3dDevice_->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
                                d3dDevice_->SetSamplerState(1, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP);
                                d3dDevice_->SetSamplerState(1, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP);

                                pixelShaders_[pixelShaderNo[pass]].set();
                                d3dDevice_->SetPixelShaderConstantF(0, (const float*)projectionParameters_, 1);

                                // walk through all meshes
                                for(uint8_t i = 0; i < NUM_OF_MESH_UNITS; ++i)
                                {
                                        vertexShaders_[vertexShaderNo[pass] + i].set();

                                        for(bool result = actorsList_.readFirstElement(i); result; result = actorsList_.readNextElement())
                                        {
                                                D3d9Mesh* d3dMesh = static_cast<D3d9Mesh*>(actorsList_.getCurrentKey());

                                                if(d3dMesh == nullptr)
                                                        break;

                                                const Mesh::Data& meshData = d3dMesh->getData();

                                                for(uint8_t vertexStream = 0; vertexStream < numVertexStreams[pass]; ++vertexStream)
                                                {
                                                        UINT streamNo = vertexStreamIndices[pass][vertexStream];
                                                        d3dDevice_->SetStreamSource(streamNo, d3dMesh->d3dVertexBuffers_[streamNo], 0,
                                                                                    meshData.vertices[streamNo].getStride());
                                                }

                                                if(i == UNIT_MESH_SKIN)
                                                        d3dDevice_->SetStreamSource(3, d3dMesh->d3dVertexBuffers_[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS], 0,
                                                                                    meshData.vertices[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS].getStride());

                                                d3dDevice_->SetIndices(d3dMesh->d3dIndexBuffer_);

                                                MeshSubsetsList* meshSubsetsList = actorsList_.getCurrentData();

                                                if(meshSubsetsList == nullptr)
                                                        break;

                                                DWORD cullModes[] = {D3DCULL_CCW, D3DCULL_NONE};

                                                for(uint8_t j = 0; j < NUM_OF_MATERIAL_UNITS; ++j)
                                                {
                                                        d3dDevice_->SetRenderState(D3DRS_CULLMODE, cullModes[j]);

                                                        for(bool result1 = meshSubsetsList->readFirstElement(j); result1; result1 = meshSubsetsList->readNextElement())
                                                        {
                                                                auto meshSubset = meshSubsetsList->getCurrentKey();
                                                                auto actors     = meshSubsetsList->getCurrentData();

                                                                if(meshSubset == nullptr || actors == nullptr)
                                                                        break;

                                                                renderActors(*meshSubset, *actors, i, pass);
                                                        }
                                                }
                                        }
                                }
                        }
                }
        }

        //--------------------------------------------------------------------------------------------
        void D3d9Renderer::accumulateLighting()
        {
                d3dDevice_->SetRenderTarget(0, d3dRenderTargetSurfaces_[RENDER_TARGET_LIGHTS]);
                d3dDevice_->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

                renderLights();
                renderLights(LIGHTS_LIST_WITH_SHADOWS);
                d3dDevice_->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        }

        //--------------------------------------------------------------------------------------------
        void D3d9Renderer::blurSsao(const Vector4d& edgeDetectionParameters, bool shouldUpscale)
        {
                d3dDevice_->SetRenderTarget(0, d3dRenderTargetSurfaces_[RENDER_TARGET_BLURRED_SSAO]);
                d3dDevice_->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

                vertexShaders_[VERTEX_SHADER_SSAO_BLUR_X_PASS].set();
                pixelShaders_[PIXEL_SHADER_SSAO_BLUR_X_PASS].set();

                d3dDevice_->SetPixelShaderConstantF(0, (const float*)unprojectionVector_, 1);
                d3dDevice_->SetPixelShaderConstantF(1, (const float*)projectionParameters_, 1);
                d3dDevice_->SetPixelShaderConstantF(2, (const float*)edgeDetectionParameters, 1);
                d3dDevice_->SetPixelShaderConstantF(3, (const float*)screenSize_, 1);
                d3dDevice_->SetPixelShaderConstantF(4, (const float*)textureCoordinatesAdjustment_, 1);

                // set positions at sampler 0
                d3dDevice_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(0, d3dRenderTargetTextures_[RENDER_TARGET_POSITIONS]);

                // set normals at sampler 1
                d3dDevice_->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
                d3dDevice_->SetSamplerState(1, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetSamplerState(1, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(1, d3dRenderTargetTextures_[RENDER_TARGET_NORMALS]);

                // set SSAO at sampler 2
                if(shouldUpscale)
                {
                        d3dDevice_->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
                        d3dDevice_->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                        d3dDevice_->SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
                        d3dDevice_->SetSamplerState(2, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
                        d3dDevice_->SetSamplerState(2, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
                        d3dDevice_->SetTexture(2, d3dHalfSizeRenderTargetTextures_[HALF_SIZE_RENDER_TARGET_SSAO]);
                }
                else
                {
                        d3dDevice_->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
                        d3dDevice_->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                        d3dDevice_->SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
                        d3dDevice_->SetSamplerState(2, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
                        d3dDevice_->SetSamplerState(2, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
                        d3dDevice_->SetTexture(2, d3dRenderTargetTextures_[RENDER_TARGET_SSAO]);
                }

                fullScreenQuad_.render();

                d3dDevice_->SetRenderTarget(0, d3dRenderTargetSurfaces_[RENDER_TARGET_SSAO]);
                d3dDevice_->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

                vertexShaders_[VERTEX_SHADER_SSAO_BLUR_Y_PASS].set();
                pixelShaders_[PIXEL_SHADER_SSAO_BLUR_Y_PASS].set();

                d3dDevice_->SetPixelShaderConstantF(0, (const float*)unprojectionVector_, 1);
                d3dDevice_->SetPixelShaderConstantF(1, (const float*)projectionParameters_, 1);
                d3dDevice_->SetPixelShaderConstantF(2, (const float*)edgeDetectionParameters, 1);
                d3dDevice_->SetPixelShaderConstantF(3, (const float*)screenSize_, 1);
                d3dDevice_->SetPixelShaderConstantF(4, (const float*)textureCoordinatesAdjustment_, 1);

                // set positions at sampler 0
                d3dDevice_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(0, d3dRenderTargetTextures_[RENDER_TARGET_POSITIONS]);

                // set normals at sampler 1
                d3dDevice_->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
                d3dDevice_->SetSamplerState(1, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetSamplerState(1, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(1, d3dRenderTargetTextures_[RENDER_TARGET_NORMALS]);

                // set SSAO at sampler 2
                d3dDevice_->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
                d3dDevice_->SetSamplerState(2, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetSamplerState(2, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(2, d3dRenderTargetTextures_[RENDER_TARGET_BLURRED_SSAO]);
                fullScreenQuad_.render();
        }

        //--------------------------------------------------------------------------------------------
        void D3d9Renderer::renderSsao()
        {
                d3dDevice_->SetStreamSource(1, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(2, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(3, nullptr, 0, 0);

                if(isThirdShaderModelSupported_)
                        d3dDevice_->SetRenderTarget(0, d3dRenderTargetSurfaces_[RENDER_TARGET_SSAO]);
                else
                        d3dDevice_->SetRenderTarget(0, d3dHalfSizeRenderTargetSurfaces_[HALF_SIZE_RENDER_TARGET_SSAO]);

                d3dDevice_->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

                d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                d3dDevice_->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

                Vector4d ssaoParameters(2.5f, -0.2f, 1.0f, 1.0f);
                Vector4d edgeDetectionParameters(2.5f, 0.99f, 4.0f / screenSize_.x, 4.0f / screenSize_.y);

                if(isThirdShaderModelSupported_)
                {
                        optionalVertexShaders_[OPTIONAL_VERTEX_SHADER_SSAO_PASS].set();
                        optionalPixelShaders_[OPTIONAL_PIXEL_SHADER_SSAO_PASS].set();
                }
                else
                {
                        vertexShaders_[VERTEX_SHADER_SSAO_PASS].set();
                        pixelShaders_[PIXEL_SHADER_SSAO_PASS].set();
                }

                d3dDevice_->SetPixelShaderConstantF(0, (const float*)unprojectionVector_, 1);
                d3dDevice_->SetPixelShaderConstantF(1, (const float*)projectionParameters_, 1);
                d3dDevice_->SetPixelShaderConstantF(2, (const float*)ssaoParameters, 1);

                d3dDevice_->SetVertexShaderConstantF(0, (const float*)screenSize_, 1);

                // set positions at sampler 0
                d3dDevice_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(0, d3dRenderTargetTextures_[RENDER_TARGET_POSITIONS]);

                // set normals at sampler 1
                d3dDevice_->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
                d3dDevice_->SetSamplerState(1, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetSamplerState(1, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(1, d3dRenderTargetTextures_[RENDER_TARGET_NORMALS]);

                // set helper texture at sampler 2
                d3dDevice_->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
                d3dDevice_->SetSamplerState(2, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP);
                d3dDevice_->SetSamplerState(2, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP);
                d3dDevice_->SetTexture(2, d3dRandomTexture_);

                if(isThirdShaderModelSupported_)
                {
                        fullScreenQuad_.render();
                }
                else
                {
                        // setup alpha blend parameters
                        d3dDevice_->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ONE);
                        d3dDevice_->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
                        d3dDevice_->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

                        // render SSAO
                        ssaoGeometry_.render();

                        // disable alpha blend
                        d3dDevice_->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
                }

                // blur SSAO
                blurSsao(edgeDetectionParameters, !isThirdShaderModelSupported_);
                edgeDetectionParameters.z = 2.0f / screenSize_.x;
                edgeDetectionParameters.w = 2.0f / screenSize_.y;
                blurSsao(edgeDetectionParameters);
                edgeDetectionParameters.z = 1.0f / screenSize_.x;
                edgeDetectionParameters.w = 1.0f / screenSize_.y;
                blurSsao(edgeDetectionParameters);
        }

        //--------------------------------------------------------------------------------------------
        void D3d9Renderer::shade()
        {
                d3dDevice_->SetRenderTarget(0, d3dRenderTargetSurfaces_[RENDER_TARGET_NORMALS]);
                d3dDevice_->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

                d3dDevice_->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
                d3dDevice_->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);

                // set samplers
                d3dDevice_->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, d3dMaxTextureAnisotropy_);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP);

                d3dDevice_->SetSamplerState(1, D3DSAMP_MAXANISOTROPY, d3dMaxTextureAnisotropy_);
                d3dDevice_->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
                d3dDevice_->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
                d3dDevice_->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
                d3dDevice_->SetSamplerState(1, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP);
                d3dDevice_->SetSamplerState(1, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP);

                d3dDevice_->SetSamplerState(2, D3DSAMP_MAXANISOTROPY, d3dMaxTextureAnisotropy_);
                d3dDevice_->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
                d3dDevice_->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
                d3dDevice_->SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
                d3dDevice_->SetSamplerState(2, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP);
                d3dDevice_->SetSamplerState(2, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP);

                // set light accumulation at sampler 3
                d3dDevice_->SetSamplerState(3, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(3, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(3, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(3, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetSamplerState(3, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(3, d3dRenderTargetTextures_[RENDER_TARGET_LIGHTS]);

                // set SSAO buffer at sampler 4 if needed
                if(is(RENDERING_SSAO_ENABLED))
                {
                        d3dDevice_->SetSamplerState(4, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
                        d3dDevice_->SetSamplerState(4, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                        d3dDevice_->SetSamplerState(4, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
                        d3dDevice_->SetSamplerState(4, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
                        d3dDevice_->SetSamplerState(4, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
                        d3dDevice_->SetTexture(4, d3dRenderTargetTextures_[RENDER_TARGET_SSAO]);

                        pixelShaders_[PIXEL_SHADER_SHADING_PASS_WITH_SSAO].set();
                }
                else
                        pixelShaders_[PIXEL_SHADER_SHADING_PASS].set();

                d3dDevice_->SetPixelShaderConstantF(4, (const float*)textureCoordinatesAdjustment_, 1);

                d3dDevice_->SetVertexDeclaration(d3dMeshVertexDeclaration_);
                d3dDevice_->SetStreamSource(1, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(3, nullptr, 0, 0);
                // walk through all meshes
                for(uint8_t i = 0; i < NUM_OF_MESH_UNITS; ++i)
                {
                        vertexShaders_[VERTEX_SHADER_SHADING_PASS + i].set();

                        for(bool result = actorsList_.readFirstElement(i); result; result = actorsList_.readNextElement())
                        {
                                D3d9Mesh* d3dMesh = static_cast<D3d9Mesh*>(actorsList_.getCurrentKey());

                                if(d3dMesh == nullptr)
                                        break;

                                const Mesh::Data& meshData = d3dMesh->getData();

                                d3dDevice_->SetStreamSource(0, d3dMesh->d3dVertexBuffers_[Mesh::VERTEX_STREAM_POSITIONS], 0,
                                                            meshData.vertices[Mesh::VERTEX_STREAM_POSITIONS].getStride());
                                d3dDevice_->SetStreamSource(2, d3dMesh->d3dVertexBuffers_[Mesh::VERTEX_STREAM_TEXTURE_COORDINATES], 0,
                                                            meshData.vertices[Mesh::VERTEX_STREAM_TEXTURE_COORDINATES].getStride());
                                if(i == UNIT_MESH_SKIN)
                                        d3dDevice_->SetStreamSource(3, d3dMesh->d3dVertexBuffers_[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS], 0,
                                                                    meshData.vertices[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS].getStride());

                                d3dDevice_->SetIndices(d3dMesh->d3dIndexBuffer_);

                                MeshSubsetsList* meshSubsetsList = actorsList_.getCurrentData();

                                if(meshSubsetsList == nullptr)
                                        break;

                                DWORD cullModes[] = {D3DCULL_CCW, D3DCULL_NONE};

                                for(uint8_t j = 0; j < NUM_OF_MATERIAL_UNITS; ++j)
                                {
                                        d3dDevice_->SetRenderState(D3DRS_CULLMODE, cullModes[j]);

                                        for(bool result1 = meshSubsetsList->readFirstElement(j); result1; result1 = meshSubsetsList->readNextElement())
                                        {
                                                auto meshSubset = meshSubsetsList->getCurrentKey();
                                                auto actors     = meshSubsetsList->getCurrentData();

                                                if(meshSubset == nullptr || actors == nullptr)
                                                        break;

                                                renderActors(*meshSubset, *actors, i, RENDERING_PASS_SHADING);
                                        }
                                }
                        }
                }
        }

        //--------------------------------------------------------------------------------------------
        void D3d9Renderer::renderParticles()
        {
                d3dDevice_->SetStreamSource(1, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(2, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(3, nullptr, 0, 0);

                d3dDevice_->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
                d3dDevice_->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
                d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

                vertexShaders_[VERTEX_SHADER_PARTICLES_PASS].set();
                pixelShaders_[PIXEL_SHADER_PARTICLES_PASS].set();

                d3dDevice_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(0, d3dRenderTargetTextures_[RENDER_TARGET_POSITIONS]);

                d3dDevice_->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
                d3dDevice_->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
                d3dDevice_->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(1, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP);
                d3dDevice_->SetSamplerState(1, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP);

                d3dDevice_->SetVertexShaderConstantF(0, (const float*)projectionMatrix_, 4);
                d3dDevice_->SetVertexShaderConstantF(4, (const float*)viewMatrix_, 4);

                d3dDevice_->SetPixelShaderConstantF(0, (const float*)textureCoordinatesAdjustment_, 1);
                d3dDevice_->SetPixelShaderConstantF(1, (const float*)unprojectionVector_, 1);
                d3dDevice_->SetPixelShaderConstantF(2, (const float*)projectionParameters_, 1);

                d3dDevice_->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
                d3dDevice_->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
                d3dDevice_->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

                for(bool result = particleSystemsList_.readFirstElement(UNIT_PARTICLE_SYSTEM); result; result = particleSystemsList_.readNextElement())
                {
                        auto texture = particleSystemsList_.getCurrentKey();
                        auto particleSystems = particleSystemsList_.getCurrentData();

                        if(particleSystems == nullptr)
                                continue;

                        setTexture(texture, 1, DUMMY_TEXTURE_WHITE);

                        for(auto it = particleSystems->begin(); it != particleSystems->end(); ++it)
                        {
                                particlesRenderer_.setParticleSystem(*it);

                                if(particlesRenderer_.prepare())
                                        particlesRenderer_.render();
                        }
                }

                d3dDevice_->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        }

        //--------------------------------------------------------------------------------------------
        void D3d9Renderer::blurBloom(const Vector4d& kernelSize)
        {
                // blur x
                d3dDevice_->SetRenderTarget(0, d3dHalfSizeRenderTargetSurfaces_
                                            [HALF_SIZE_RENDER_TARGET_BLURRED_BLOOM]);
                d3dDevice_->Clear(0, nullptr, D3DCLEAR_TARGET,
                                  D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

                d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                d3dDevice_->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

                d3dDevice_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(0, d3dHalfSizeRenderTargetTextures_
                                       [HALF_SIZE_RENDER_TARGET_BLOOM]);

                vertexShaders_[VERTEX_SHADER_BLOOM_PASS].set();
                pixelShaders_[PIXEL_SHADER_BLOOM_X_PASS].set();

                d3dDevice_->SetPixelShaderConstantF(0, (const float*)kernelSize, 1);
                d3dDevice_->SetPixelShaderConstantF(1, (const float*)textureCoordinatesAdjustment_, 1);

                fullScreenQuad_.render();

                // blur y
                d3dDevice_->SetRenderTarget(0, d3dHalfSizeRenderTargetSurfaces_
                                            [HALF_SIZE_RENDER_TARGET_BLOOM]);
                d3dDevice_->Clear(0, nullptr, D3DCLEAR_TARGET,
                                  D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

                d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                d3dDevice_->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

                d3dDevice_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(0, d3dHalfSizeRenderTargetTextures_
                                       [HALF_SIZE_RENDER_TARGET_BLURRED_BLOOM]);

                vertexShaders_[VERTEX_SHADER_BLOOM_PASS].set();
                pixelShaders_[PIXEL_SHADER_BLOOM_Y_PASS].set();

                d3dDevice_->SetPixelShaderConstantF(0, (const float*)kernelSize, 1);
                d3dDevice_->SetPixelShaderConstantF(1, (const float*)textureCoordinatesAdjustment_, 1);

                fullScreenQuad_.render();
        }

        //--------------------------------------------------------------------------------------------
        void D3d9Renderer::renderBloom()
        {
                d3dDevice_->SetStreamSource(1, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(2, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(3, nullptr, 0, 0);

                Vector4d kernelSize(1.0f / screenSize_.z, 1.0f / screenSize_.w);
                Vector4d luminance(0.08f, 0.08f, 0.08f, 0.08f);
                Vector4d parameters(0.18f, 0.64f);

                // bright pass
                d3dDevice_->SetRenderTarget(0, d3dHalfSizeRenderTargetSurfaces_
                                            [HALF_SIZE_RENDER_TARGET_BLOOM]);
                d3dDevice_->Clear(0, nullptr, D3DCLEAR_TARGET,
                                  D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

                d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                d3dDevice_->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

                d3dDevice_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(0, d3dRenderTargetTextures_[RENDER_TARGET_NORMALS]);

                vertexShaders_[VERTEX_SHADER_BRIGHT_PASS].set();
                pixelShaders_[PIXEL_SHADER_BRIGHT_PASS].set();

                d3dDevice_->SetPixelShaderConstantF(0, (const float*)luminance, 1);
                d3dDevice_->SetPixelShaderConstantF(1, (const float*)textureCoordinatesAdjustment_, 1);
                d3dDevice_->SetPixelShaderConstantF(2, (const float*)parameters, 1);

                fullScreenQuad_.render();

                blurBloom(kernelSize);
                blurBloom(kernelSize);

                // combine
                d3dDevice_->SetRenderTarget(0, d3dRenderTargetSurfaces_
                                            [RENDER_TARGET_BLURRED_SSAO]);
                d3dDevice_->Clear(0, nullptr, D3DCLEAR_TARGET,
                                  D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

                d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                d3dDevice_->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

                vertexShaders_[VERTEX_SHADER_COMBINE_PASS].set();
                pixelShaders_[PIXEL_SHADER_COMBINE_PASS].set();

                d3dDevice_->SetPixelShaderConstantF(0, (const float*)textureCoordinatesAdjustment_, 1);

                d3dDevice_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(0, d3dRenderTargetTextures_[RENDER_TARGET_NORMALS]);

                d3dDevice_->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
                d3dDevice_->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
                d3dDevice_->SetSamplerState(1, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetSamplerState(1, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(1, d3dHalfSizeRenderTargetTextures_[HALF_SIZE_RENDER_TARGET_BLOOM]);

                fullScreenQuad_.render();
        }

}
