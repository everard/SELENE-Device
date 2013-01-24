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

        LPDIRECT3DDEVICE9 D3d9Renderer::d3dDevice_ = nullptr;

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

                // create d3d
                d3d_ = Direct3DCreate9(D3D_SDK_VERSION);
                if(d3d_ == nullptr)
                {
                        writeLogEntry("ERROR: Could not initialize Direct3D");
                        return false;
                }

                // get display mode
                D3DDISPLAYMODE d3dDisplayMode;
                if(FAILED(d3d_->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3dDisplayMode)))
                {
                        writeLogEntry("ERROR: Could not get display mode");
                        destroy();
                        return false;
                }

                memset(&d3dPresentParameters_, 0, sizeof(D3DPRESENT_PARAMETERS));

                d3dPresentParameters_.SwapEffect = D3DSWAPEFFECT_DISCARD;
                d3dPresentParameters_.BackBufferFormat = d3dDisplayMode.Format;
                if(is(RENDERING_FULL_SCREEN_ENABLED))
                        d3dPresentParameters_.Windowed = FALSE;
                else
                        d3dPresentParameters_.Windowed = TRUE;

                d3dPresentParameters_.BackBufferWidth  = parameters_.getWidth();
                d3dPresentParameters_.BackBufferHeight = parameters_.getHeight();
                d3dPresentParameters_.EnableAutoDepthStencil = TRUE;
                d3dPresentParameters_.AutoDepthStencilFormat = D3DFMT_D24S8;
                d3dPresentParameters_.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
                d3dPresentParameters_.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

                // check device capabilities
                D3DCAPS9 d3dCaps;
                d3d_->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dCaps);
                capabilities_.maxTextureAnisotropy = d3dCaps.MaxAnisotropy;
                if(capabilities_.maxTextureAnisotropy < 1)
                        capabilities_.maxTextureAnisotropy = 1;
                else if(capabilities_.maxTextureAnisotropy > 8)
                        capabilities_.maxTextureAnisotropy = 8;

                if(d3dCaps.PixelShaderVersion < D3DPS_VERSION(2, 0))
                {
                        destroy();
                        return false;
                }

                capabilities_.isThirdShaderModelSupported = false;
                if(d3dCaps.PixelShaderVersion >= D3DPS_VERSION(3, 0))
                {
                        writeLogEntry("PS 3.0 is supported");
                        capabilities_.isThirdShaderModelSupported = true;
                }

                capabilities_.isMultipleRenderTargetSupported = false;
                if(d3dCaps.NumSimultaneousRTs > 1)
                {
                        writeLogEntry("MRT is supported");
                        capabilities_.isMultipleRenderTargetSupported = true;
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
                if(FAILED(d3d_->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, windowsApplication->getWindowHandle(),
                                             d3dFlags, &d3dPresentParameters_, &d3dDevice_)))
                {
                        writeLogEntry("ERROR: Could not create Direct3D device");
                        destroy();
                        return false;
                }

                if(FAILED(d3d_->CheckDeviceFormat(d3dCaps.AdapterOrdinal, d3dCaps.DeviceType,
                                                  d3dDisplayMode.Format, D3DUSAGE_RENDERTARGET,
                                                  D3DRTYPE_TEXTURE, D3DFMT_R32F)))
                {
                        capabilities_.isR32fRenderTargetFormatSupported = false;
                }
                else
                {
                        writeLogEntry("R32F texture format is supported");
                        capabilities_.isR32fRenderTargetFormatSupported = true;
                }

                if(d3dDevice_ == nullptr)
                {
                        writeLogEntry("ERROR: Direct3D device is not created");
                        destroy();
                        return false;
                }

                return initializeHelpers();
        }

        //--------------------------------------------------------------------------------------------
        void D3d9Renderer::destroy()
        {
                destroyHelpers();

                SAFE_RELEASE(d3dDevice_);
                SAFE_RELEASE(d3d_);
        }

        //--------------------------------------------------------------------------------------------
        void D3d9Renderer::render(const Camera& camera)
        {
                if(d3dDevice_ == nullptr)
                        return;

                if(isDeviceLost_)
                {
                        ResourceManager::discardResources();
                        destroyHelpers();
                        if(SUCCEEDED(d3dDevice_->Reset(&d3dPresentParameters_)))
                        {
                                if(initializeHelpers())
                                        ResourceManager::retainResources();

                                isDeviceLost_ = false;
                        }
                }

                auto& renderingData = const_cast<Renderer::Data&>(camera.getRenderingData());
                Gui* gui = camera.getGui();
                guiRenderer_.setGui(gui);

                // get matrices
                frameParameters_.viewProjectionMatrix = camera.getViewProjectionMatrix();
                frameParameters_.projectionMatrix = camera.getProjectionMatrix();
                frameParameters_.viewMatrix = camera.getViewMatrix();

                frameParameters_.normalsMatrix = frameParameters_.viewMatrix;
                frameParameters_.normalsMatrix.invert();
                frameParameters_.viewInvMatrix = frameParameters_.normalsMatrix;
                frameParameters_.normalsMatrix.transpose();

                // get vectors
                const Matrix& projectionInvMatrix = camera.getProjectionInvMatrix();
                frameParameters_.projectionParameters = camera.getProjectionParameters();
                frameParameters_.unprojectionVector.define(projectionInvMatrix.a[0][0],
                                                           projectionInvMatrix.a[1][1],
                                                           1.0, 0.0);

                // begin rendering
                if(FAILED(d3dDevice_->BeginScene()))
                        return;

                actorsRenderer_.renderPositionsAndNormals(renderingData.getActorNode());
                lightingRenderer_.renderLighting(renderingData.getLightNode());

                bool isSsaoEnabled = false;
                /*if(is(RENDERING_SSAO_ENABLED))
                {
                        renderSsao();
                        isSsaoEnabled = true;
                }*/

                actorsRenderer_.renderShading(renderingData.getActorNode(), isSsaoEnabled);
                /*renderParticles(renderingData.getParticleSystemNode());

                uint8_t resultRenderTarget = RENDER_TARGET_NORMALS;
                if(is(RENDERING_BLOOM_ENABLED))
                {
                        renderBloom();
                        resultRenderTarget = RENDER_TARGET_BLURRED_SSAO;
                }*/

                // render result
                d3dDevice_->SetStreamSource(1, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(2, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(3, nullptr, 0, 0);

                d3dDevice_->SetRenderTarget(0, renderTargetContainer_.getBackBuffer().getSurface());
                d3dDevice_->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

                vertexShaders_[VERTEX_SHADER_RESULT_PASS].set();
                pixelShaders_[PIXEL_SHADER_RESULT_PASS].set();

                d3dDevice_->SetVertexShaderConstantF(0, static_cast<const float*>(frameParameters_.textureCoordinatesAdjustment), 1);

                d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                d3dDevice_->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

                d3dDevice_->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                d3dDevice_->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetSamplerState(0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(0, renderTargetContainer_.getRenderTarget
                                       (D3d9RenderTargetContainer::RENDER_TARGET_RESULT).getTexture());

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

                        if(!gui->is(GUI_DISABLED))
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
                d3dDevice_->SetIndices(nullptr);

                d3dDevice_->SetTexture(0, nullptr);
                d3dDevice_->SetTexture(1, nullptr);
                d3dDevice_->SetTexture(2, nullptr);
                d3dDevice_->SetTexture(3, nullptr);

                d3dDevice_->SetVertexDeclaration(nullptr);
                d3dDevice_->SetVertexShader(nullptr);
                d3dDevice_->SetPixelShader(nullptr);

                d3dDevice_->EndScene();
                if(d3dDevice_->Present(nullptr, nullptr, 0, nullptr) == D3DERR_DEVICELOST)
                        isDeviceLost_ = true;
        }

        //--------------------------------------------------------------------------------------------
        LPDIRECT3DDEVICE9 D3d9Renderer::getDevice()
        {
                return d3dDevice_;
        }

        D3d9Renderer::D3d9Renderer(): parameters_(nullptr, nullptr, 0, 0, nullptr, 0),
                                      renderTargetContainer_(parameters_, capabilities_, frameParameters_),
                                      lightingRenderer_(renderTargetContainer_, frameParameters_, capabilities_,
                                                        actorsRenderer_, textureHandler_),
                                      actorsRenderer_(renderTargetContainer_, frameParameters_,
                                                      capabilities_, textureHandler_)
        {
                d3dDevice_ = nullptr;
                d3d_ = nullptr;

                d3dRandomTexture_ = nullptr;
                memset(&d3dPresentParameters_, 0, sizeof(d3dPresentParameters_));

                isDeviceLost_ = false;
        }
        D3d9Renderer::D3d9Renderer(const D3d9Renderer&): Renderer(), Status(),
                                                         parameters_(nullptr, nullptr, 0, 0, nullptr, 0),
                                                         renderTargetContainer_(parameters_, capabilities_, frameParameters_),
                                                         lightingRenderer_(renderTargetContainer_, frameParameters_, capabilities_,
                                                                           actorsRenderer_, textureHandler_),
                                                         actorsRenderer_(renderTargetContainer_, frameParameters_,
                                                                         capabilities_, textureHandler_) {}
        D3d9Renderer::~D3d9Renderer()
        {
                destroy();
        }
        D3d9Renderer& D3d9Renderer::operator =(const D3d9Renderer&)
        {
                return *this;
        }

        //--------------------------------------------------------------------------------------------
        bool D3d9Renderer::initializeHelpers()
        {
                // create shaders
                D3d9Shader d3dVertexShaders[NUM_OF_VERTEX_SHADERS] =
                {
                        D3d9Shader("ResultPass.vsh",    "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, capabilities_),
                        D3d9Shader("SSAOPass.vsh",      "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, capabilities_),
                        D3d9Shader("SSAOBlurX.vsh",     "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, capabilities_),
                        D3d9Shader("SSAOBlurY.vsh",     "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, capabilities_),
                        D3d9Shader("EdgeDetect.vsh",    "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, capabilities_),
                        D3d9Shader("GUIFramesPass.vsh", "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, capabilities_),
                        D3d9Shader("GUITextPass.vsh",   "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, capabilities_),
                        D3d9Shader("GUICursorPass.vsh", "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, capabilities_),
                        D3d9Shader("BrightPass.vsh",    "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, capabilities_),
                        D3d9Shader("Bloom.vsh",         "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, capabilities_),
                        D3d9Shader("CombinePass.vsh",   "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, capabilities_),
                        D3d9Shader("ParticlesPass.vsh", "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, capabilities_)
                };

                D3d9Shader d3dPixelShaders[NUM_OF_PIXEL_SHADERS] =
                {
                        D3d9Shader("ResultPass.psh",    "ps_1_1", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, capabilities_),
                        D3d9Shader("SSAOPass.psh",      "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, capabilities_),
                        D3d9Shader("SSAOBlurX.psh",     "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, capabilities_),
                        D3d9Shader("SSAOBlurY.psh",     "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, capabilities_),
                        D3d9Shader("EdgeDetect.psh",    "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, capabilities_),
                        D3d9Shader("GUIFramesPass.psh", "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, capabilities_),
                        D3d9Shader("GUITextPass.psh",   "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, capabilities_),
                        D3d9Shader("GUICursorPass.psh", "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, capabilities_),
                        D3d9Shader("BrightPass.psh",    "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, capabilities_),
                        D3d9Shader("BloomX.psh",        "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, capabilities_),
                        D3d9Shader("BloomY.psh",        "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, capabilities_),
                        D3d9Shader("CombinePass.psh",   "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, capabilities_),
                        D3d9Shader("ParticlesPass.psh", "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, capabilities_)
                };

                for(uint32_t i = 0; i < NUM_OF_VERTEX_SHADERS; ++i)
                {
                        if(!vertexShaders_[i].create(d3dVertexShaders[i]))
                        {
                                writeLogEntry("ERROR: Could not create vertex shader");
                                return false;
                        }
                }

                for(uint32_t i = 0; i < NUM_OF_PIXEL_SHADERS; ++i)
                {
                        if(!pixelShaders_[i].create(d3dPixelShaders[i]))
                        {
                                writeLogEntry("ERROR: Could not create pixel shader");
                                return false;
                        }
                }

                // create optional shaders
                if(capabilities_.isThirdShaderModelSupported)
                {
                        D3d9Shader d3dOptionalVertexShader("SSAO30Pass.vsh", "vs_1_1", 0, D3d9Shader::LIBRARY_VERTEX_SHADER, capabilities_);
                        D3d9Shader d3dOptionalPixelShader("SSAO30Pass.psh",  "ps_3_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, capabilities_);

                        if(!optionalVertexShaders_[OPTIONAL_VERTEX_SHADER_SSAO_PASS].create(d3dOptionalVertexShader))
                        {
                                writeLogEntry("ERROR: Could not create optional vertex shader");
                                return false;
                        }

                        if(!optionalPixelShaders_[OPTIONAL_PIXEL_SHADER_SSAO_PASS].create(d3dOptionalPixelShader))
                        {
                                writeLogEntry("ERROR: Could not create optional pixel shader");
                                return false;
                        }
                }

                // create random texture
                if(FAILED(d3dDevice_->CreateTexture(64, 64, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
                                                    &d3dRandomTexture_, nullptr)))
                {
                        writeLogEntry("ERROR: Could not create random texture");
                        return false;
                }

                srand(0);
                if(FAILED(D3DXFillTexture(d3dRandomTexture_, fillRandomTexture, nullptr)))
                {
                        writeLogEntry("ERROR: Could not fill random texture");
                        return false;
                }

                // create helpers
                FileManager* fileManager = parameters_.getFileManager();

                if(!renderTargetContainer_.initialize() ||
                   !particlesRenderer_.initialize() ||
                   !lightingRenderer_.initialize() ||
                   !actorsRenderer_.initialize() ||
                   !textureHandler_.initialize() ||
                   !fullScreenQuad_.initialize() ||
                   !ssaoGeometry_.initialize()   ||
                   !guiRenderer_.initialize(fileManager))
                {
                        writeLogEntry("ERROR: Could not initialize helpers");
                        return false;
                }

                // set screen size
                uint32_t halfWidth  = parameters_.getWidth()  / 2;
                uint32_t halfHeight = parameters_.getHeight() / 2;
                frameParameters_.screenSize.define(static_cast<float>(parameters_.getWidth()),
                                                   static_cast<float>(parameters_.getHeight()),
                                                   static_cast<float>(halfWidth),
                                                   static_cast<float>(halfHeight));

                // compute texture coordinates adjustment to directly map texels to pixels
                frameParameters_.textureCoordinatesAdjustment.define( 1.0f / static_cast<float>(parameters_.getWidth()),
                                                                     -1.0f / static_cast<float>(parameters_.getHeight()),
                                                                      0.5f / static_cast<float>(parameters_.getWidth()),
                                                                      0.5f / static_cast<float>(parameters_.getHeight()));

                d3dDevice_->SetRenderState(D3DRS_LIGHTING, FALSE);
                d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                d3dDevice_->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

                return true;
        }

        //--------------------------------------------------------------------------------------------
        void D3d9Renderer::destroyHelpers()
        {
                for(uint32_t i = 0; i < NUM_OF_OPTIONAL_VERTEX_SHADERS; ++i)
                        optionalVertexShaders_[i].destroy();

                for(uint32_t i = 0; i < NUM_OF_OPTIONAL_PIXEL_SHADERS; ++i)
                        optionalPixelShaders_[i].destroy();

                for(uint32_t i = 0; i < NUM_OF_VERTEX_SHADERS; ++i)
                        vertexShaders_[i].destroy();

                for(uint32_t i = 0; i < NUM_OF_PIXEL_SHADERS; ++i)
                        pixelShaders_[i].destroy();

                renderTargetContainer_.destroy();
                particlesRenderer_.destroy();
                lightingRenderer_.destroy();
                actorsRenderer_.destroy();
                textureHandler_.destroy();
                fullScreenQuad_.destroy();
                ssaoGeometry_.destroy();
                guiRenderer_.destroy();

                SAFE_RELEASE(d3dRandomTexture_);
        }

        //--------------------------------------------------------------------------------------------
        void D3d9Renderer::writeLogEntry(const char* entry)
        {
                if(parameters_.getLog() != nullptr)
                        (*parameters_.getLog()) << entry << std::endl;
        }

        /*//--------------------------------------------------------------------------------------------
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

                if(capabilities_.isThirdShaderModelSupported)
                        d3dDevice_->SetRenderTarget(0, d3dRenderTargetSurfaces_[RENDER_TARGET_SSAO]);
                else
                        d3dDevice_->SetRenderTarget(0, d3dHalfSizeRenderTargetSurfaces_[HALF_SIZE_RENDER_TARGET_SSAO]);

                d3dDevice_->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

                d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                d3dDevice_->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

                Vector4d ssaoParameters(2.5f, -0.2f, 1.0f, 1.0f);
                Vector4d edgeDetectionParameters(2.5f, 0.99f, 4.0f / screenSize_.x, 4.0f / screenSize_.y);

                if(capabilities_.isThirdShaderModelSupported)
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

                if(capabilities_.isThirdShaderModelSupported)
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
                blurSsao(edgeDetectionParameters, !capabilities_.isThirdShaderModelSupported);
                edgeDetectionParameters.z = 2.0f / screenSize_.x;
                edgeDetectionParameters.w = 2.0f / screenSize_.y;
                blurSsao(edgeDetectionParameters);
                edgeDetectionParameters.z = 1.0f / screenSize_.x;
                edgeDetectionParameters.w = 1.0f / screenSize_.y;
                blurSsao(edgeDetectionParameters);
        }*/

        /*//--------------------------------------------------------------------------------------------
        void D3d9Renderer::renderParticles(Renderer::Data::ParticleSystemNode& particleSystemNode)
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

                for(bool result = particleSystemNode.readFirstElement(Renderer::Data::UNIT_PARTICLE_SYSTEM); result;
                         result = particleSystemNode.readNextElement())
                {
                        auto texture = particleSystemNode.getCurrentKey();
                        auto particleSystems = particleSystemNode.getCurrentData();

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
        }*/

}
