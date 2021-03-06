// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "D3D9Renderer.h"

#include "../../../../Engine/Scene/Nodes/Camera.h"
#include "../../../../Engine/Scene/Nodes/Actor.h"
#include "../../Application/WindowsApplication.h"
#include "../../../../Engine/GUI/GUI.h"

#include "Resources/D3D9Texture.h"
#include "Resources/D3D9Mesh.h"

namespace selene
{

        LPDIRECT3DDEVICE9 D3d9Renderer::d3dDevice_ = nullptr;

        //----------------------------------------------------------------------------------------------------------
        bool D3d9Renderer::initialize(const Renderer::Parameters& parameters)
        {
                destroy();

                parameters_ = parameters;

                writeLogEntry("initializing Direct3D 9 renderer");

                // check parameters
                if(parameters_.getWidth()  < 128 || parameters_.getWidth()  > 4096 ||
                   parameters_.getHeight() < 128 || parameters_.getHeight() > 4096)
                {
                        writeLogEntry("error: width and height of the rendering area must be in range [128; 4096]");
                        return false;
                }

                // create d3d
                d3d_ = Direct3DCreate9(D3D_SDK_VERSION);
                if(d3d_ == nullptr)
                {
                        writeLogEntry("error: could not initialize Direct3D");
                        return false;
                }

                if(!capabilities_.createCompatibleDevice(d3d_, parameters_, effectsList_,
                                                         d3dPresentParameters_,
                                                         d3dDevice_))
                {
                        destroy();
                        writeLogEntry("error: could not create compatible device");
                        return false;
                }

                if(capabilities_.isThirdShaderModelSupported())
                        writeLogEntry("note: shader model 3.0 is supported");

                if(capabilities_.isMultipleRenderTargetSupported())
                        writeLogEntry("note: multiple render targets are supported");

                if(capabilities_.isR32fRenderTargetFormatSupported())
                        writeLogEntry("note: R32F texture format is supported");

                return initializeHelpers();
        }

        //----------------------------------------------------------------------------------------------------------
        void D3d9Renderer::destroy()
        {
                destroyHelpers();

                SAFE_RELEASE(d3dDevice_);
                SAFE_RELEASE(d3d_);

                effectsList_.clear();
        }

        //----------------------------------------------------------------------------------------------------------
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

                const auto& ssao    = camera.getEffect("SSAO");
                const auto& bloom   = camera.getEffect("Bloom");
                const auto& shadows = camera.getEffect("Shadows");

                frameParameters_.ssaoParameters.x = ssao.getParameter("Radius").getValue();
                frameParameters_.ssaoParameters.y = ssao.getParameter("Normal influence bias").getValue();
                frameParameters_.ssaoParameters.z = ssao.getParameter("Minimum cosine alpha").getValue();
                frameParameters_.ssaoQuality = ssao.getQuality();

                frameParameters_.bloomParameters.define(bloom.getParameter("Luminance").getValue(),
                                                        bloom.getParameter("Scale").getValue(),
                                                        0.18f, 0.64f);
                frameParameters_.bloomQuality = bloom.getQuality();

                frameParameters_.shadowsQuality = shadows.getQuality();

                // begin rendering
                if(FAILED(d3dDevice_->BeginScene()))
                        return;

                uint8_t resultRenderTarget = RENDER_TARGET_RESULT;
                actorsRenderer_.renderPositionsAndNormals(renderingData.getActorNode());
                lightingRenderer_.renderLighting(renderingData.getLightNode());

                bool isSsaoEnabled = (frameParameters_.ssaoQuality != 0);
                if(isSsaoEnabled)
                        ssaoRenderer_.renderSsao();

                actorsRenderer_.renderShading(renderingData.getActorNode(), isSsaoEnabled);

                if(frameParameters_.bloomQuality != 0)
                {
                        bloomRenderer_.renderBloom();
                        resultRenderTarget = RENDER_TARGET_HELPER_1;
                }

                // render result
                d3dDevice_->SetStreamSource(1, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(2, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(3, nullptr, 0, 0);

                d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                d3dDevice_->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
                d3dDevice_->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

                d3dDevice_->SetRenderTarget(0, renderTargetContainer_.getBackBuffer().getSurface());
                d3dDevice_->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

                resultVertexShader_.set();
                resultPixelShader_.set();

                d3dDevice_->SetPixelShaderConstantF(0, frameParameters_.textureCoordinatesAdjustment, 1);

                textureHandler_.setStageState(0, D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE,
                                              D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(0, renderTargetContainer_.getRenderTarget(resultRenderTarget).getTexture());

                fullScreenQuad_.render();

                // render GUI
                guiRenderer_.renderGui(camera.getGui());

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

        //----------------------------------------------------------------------------------------------------------
        LPDIRECT3DDEVICE9 D3d9Renderer::getDevice()
        {
                return d3dDevice_;
        }

        D3d9Renderer::D3d9Renderer():
                d3d_(nullptr), resultVertexShader_(), resultPixelShader_(), renderTargetContainer_(),
                lightingRenderer_(), actorsRenderer_(), textureHandler_(), fullScreenQuad_(),
                bloomRenderer_(), ssaoRenderer_(), guiRenderer_(), d3dPresentParameters_(),
                frameParameters_(), capabilities_(), isDeviceLost_(false)
        {
                d3dDevice_ = nullptr;
                memset(&d3dPresentParameters_, 0, sizeof(d3dPresentParameters_));
        }
        D3d9Renderer::~D3d9Renderer()
        {
                destroy();
        }

        //----------------------------------------------------------------------------------------------------------
        bool D3d9Renderer::initializeHelpers()
        {
                static const char* vertexShaderResultPass =
                        "struct Input"
                        "{"
                        "        float4 position: POSITION0;"
                        "};"
                        ""
                        "struct Output"
                        "{"
                        "        float4 position:           POSITION;"
                        "        float2 textureCoordinates: TEXCOORD0;"
                        "};"
                        ""
                        "Output main(Input input)"
                        "{"
                        "        Output output;"
                        ""
                        "        output.position = float4(input.position.xy, 0.0, 1.0);"
                        "        output.textureCoordinates = input.position.zw;"
                        ""
                        "        return output;"
                        "}";

                static const char* pixelShaderResultPass =
                        "float4 textureCoordinatesAdjustment: register(c0);"
                        ""
                        "sampler2D sourceTexture: register(s0);"
                        ""
                        "struct Input"
                        "{"
                        "        float2 textureCoordinates: TEXCOORD0;"
                        "};"
                        ""
                        "float4 main(Input input): COLOR0"
                        "{"
                        "        input.textureCoordinates += textureCoordinatesAdjustment.zw;"
                        "        return tex2D(sourceTexture, input.textureCoordinates);"
                        "}";

                D3d9Shader d3dVertexShader(vertexShaderResultPass, "vs_1_1", 0,
                                           D3d9Shader::LIBRARY_EMPTY, capabilities_);
                D3d9Shader d3dPixelShader(pixelShaderResultPass,   "ps_2_0", 0,
                                          D3d9Shader::LIBRARY_EMPTY, capabilities_);

                if(!resultVertexShader_.create(d3dVertexShader))
                {
                        writeLogEntry("error: could not create vertex shader");
                        return false;
                }

                if(!resultPixelShader_.create(d3dPixelShader))
                {
                        writeLogEntry("error: could not create pixel shader");
                        return false;
                }

                // initialize helpers
                FileManager* fileManager = parameters_.getFileManager();

                if(!renderTargetContainer_.initialize(frameParameters_, parameters_, capabilities_))
                {
                        writeLogEntry("error: could not initialize render target container");
                        return false;
                }

                if(!lightingRenderer_.initialize(renderTargetContainer_,
                                                 frameParameters_,
                                                 actorsRenderer_,
                                                 textureHandler_,
                                                 capabilities_))
                {
                        writeLogEntry("error: could not initialize lighting renderer");
                        return false;
                }

                if(!actorsRenderer_.initialize(renderTargetContainer_,
                                               frameParameters_,
                                               textureHandler_,
                                               capabilities_))
                {
                        writeLogEntry("error: could not initialize actors renderer");
                        return false;
                }

                if(!textureHandler_.initialize())
                {
                        writeLogEntry("error: could not initialize texture handler");
                        return false;
                }

                if(!fullScreenQuad_.initialize())
                {
                        writeLogEntry("error: could not initialize full-screen quad");
                        return false;
                }

                if(!bloomRenderer_.initialize(renderTargetContainer_,
                                              frameParameters_,
                                              fullScreenQuad_,
                                              textureHandler_,
                                              capabilities_))
                {
                        writeLogEntry("error: could not initialize bloom renderer");
                        return false;
                }

                if(!ssaoRenderer_.initialize(renderTargetContainer_,
                                             frameParameters_,
                                             fullScreenQuad_,
                                             textureHandler_,
                                             capabilities_))
                {
                        writeLogEntry("error: could not initialize SSAO renderer");
                        return false;
                }

                if(!guiRenderer_.initialize(fullScreenQuad_,
                                            textureHandler_,
                                            capabilities_,
                                            fileManager))
                {
                        writeLogEntry("error: could not initialize GUI renderer");
                        return false;
                }

                d3dDevice_->SetRenderState(D3DRS_LIGHTING, FALSE);
                d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                d3dDevice_->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

                return true;
        }

        //----------------------------------------------------------------------------------------------------------
        void D3d9Renderer::destroyHelpers()
        {
                resultVertexShader_.destroy();
                resultPixelShader_.destroy();

                renderTargetContainer_.destroy();
                lightingRenderer_.destroy();
                actorsRenderer_.destroy();
                textureHandler_.destroy();
                fullScreenQuad_.destroy();
                bloomRenderer_.destroy();
                ssaoRenderer_.destroy();
                guiRenderer_.destroy();
        }

        //----------------------------------------------------------------------------------------------------------
        void D3d9Renderer::writeLogEntry(const char* entry)
        {
                if(parameters_.getLog() != nullptr)
                        (*parameters_.getLog()) << entry << std::endl;
        }

}
