// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "D3D9LightingRenderer.h"
#include "../D3D9Renderer.h"

namespace selene
{

        static float lightVolumeGeometryBuffer[] =
        {
                // Directional light
                -1.0f, -1.0f, 1.0f, 0.0f,
                -1.0f, 1.0f, 1.0f, 0.0f,
                1.0f, -1.0f, 1.0f, 0.0f,
                1.0f, 1.0f, 1.0f, 0.0f,

                // Point light
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.707107f, 0.0f, 0.707107f, 0.0f,
                0.0f, 0.707107f, 0.707107f, 0.0f,
                0.707107f, 0.707107f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.707107f, -0.707107f, 0.0f,
                -0.707107f, 0.707107f, 0.0f, 0.0f,
                -0.707107f, 0.0f, -0.707107f, 0.0f,
                -1.0f, 0.0f, 0.0f, 0.0f,
                -0.707107f, -0.707107f, 0.0f, 0.0f,
                -0.707107f, 0.0f, 0.707107f, 0.0f,
                0.0f, -0.707107f, 0.707107f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, -0.707107f, 0.707107f, 0.0f,
                0.707107f, 0.0f, 0.707107f, 0.0f,
                0.707107f, -0.707107f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f, 0.0f,
                0.707107f, 0.0f, -0.707107f, 0.0f,
                0.707107f, 0.707107f, 0.0f, 0.0f,
                0.707107f, 0.0f, -0.707107f, 0.0f,
                0.0f, 0.707107f, -0.707107f, 0.0f,
                0.0f, 0.0f, -1.0f, 0.0f,
                0.0f, 0.0f, -1.0f, 0.0f,
                0.707107f, 0.0f, 0.707107f, 0.0f,
                0.707107f, 0.0f, 0.707107f, 0.0f,
                1.0f, 0.0f, 0.0f, 0.0f,
                0.707107f, 0.707107f, 0.0f, 0.0f,
                0.707107f, 0.707107f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.707107f, 0.707107f, 0.0f,
                -0.707107f, 0.0f, 0.707107f, 0.0f,
                -0.707107f, 0.707107f, 0.0f, 0.0f,
                -1.0f, 0.0f, 0.0f, 0.0f,
                -1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.707107f, 0.707107f, 0.0f,
                0.0f, 0.707107f, 0.707107f, 0.0f,
                0.0f, 0.707107f, 0.707107f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                -0.707107f, 0.707107f, 0.0f, 0.0f,
                -0.707107f, 0.707107f, 0.0f, 0.0f,
                0.0f, 0.707107f, -0.707107f, 0.0f,
                0.0f, 0.707107f, -0.707107f, 0.0f,
                0.0f, 0.707107f, -0.707107f, 0.0f,
                0.0f, 0.0f, -1.0f, 0.0f,
                -0.707107f, 0.0f, -0.707107f, 0.0f,
                0.0f, -0.707107f, -0.707107f, 0.0f,
                -0.707107f, -0.707107f, 0.0f, 0.0f,
                0.0f, -1.0f, 0.0f, 0.0f,
                0.0f, -0.707107f, 0.707107f, 0.0f,
                0.0f, -1.0f, 0.0f, 0.0f,
                0.707107f, -0.707107f, 0.0f, 0.0f,
                0.0f, -0.707107f, -0.707107f, 0.0f,
                0.707107f, 0.0f, -0.707107f, 0.0f,
                0.0f, 0.0f, -1.0f, 0.0f,
                0.0f, 0.0f, -1.0f, 0.0f,

                // Spot light
                0.707107f, 0.707107f, 1.0f, 0.0f,
                0.707107f, 0.707107f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                1.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.707107f, -0.707107f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, -1.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                -0.707107f, -0.707107f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                -1.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                -0.707107f, 0.707107f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 1.0f, 1.0f, 0.0f,
                0.707107f, 0.707107f, 1.0f, 0.0f,
                0.707107f, 0.707107f, 1.0f, 0.0f,
                1.0f, 0.0f, 1.0f, 0.0f,
                1.0f, 0.0f, 1.0f, 0.0f,
                1.0f, 0.0f, 1.0f, 0.0f,
                1.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.707107f, 0.707107f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                -0.707107f, 0.707107f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                -1.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                -0.707107f, -0.707107f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, -1.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.707107f, -0.707107f, 1.0f, 0.0f,
                1.0f, 0.0f, 1.0f, 0.0f,
                1.0f, 0.0f, 1.0f, 0.0f
        };

        static const uint32_t lightVolumeGeometryNumVertices[D3d9LightingRenderer::NUM_OF_LIGHT_TYPES] = {4, 58, 38};
        static const uint32_t lightVolumeGeometryVertexStride = sizeof(Vector4d);

        static const uint32_t lightVolumeGeometryBufferOffsets[D3d9LightingRenderer::NUM_OF_LIGHT_TYPES] =
        {
                // Directional light
                0,

                // Point light
                4 * lightVolumeGeometryNumVertices[D3d9LightingRenderer::LIGHT_DIRECTIONAL],

                // Spot light
                4 * lightVolumeGeometryNumVertices[D3d9LightingRenderer::LIGHT_DIRECTIONAL] +
                4 * lightVolumeGeometryNumVertices[D3d9LightingRenderer::LIGHT_POINT]
        };

        static const uint32_t lightVolumeGeometryBufferSizes[D3d9LightingRenderer::NUM_OF_LIGHT_TYPES] =
        {
                // Directional light
                lightVolumeGeometryVertexStride *
                lightVolumeGeometryNumVertices[D3d9LightingRenderer::LIGHT_DIRECTIONAL] *
                D3d9LightingRenderer::BATCH_SIZE,

                // Point light
                lightVolumeGeometryVertexStride *
                lightVolumeGeometryNumVertices[D3d9LightingRenderer::LIGHT_POINT] *
                D3d9LightingRenderer::BATCH_SIZE,

                // Spot light
                lightVolumeGeometryVertexStride *
                lightVolumeGeometryNumVertices[D3d9LightingRenderer::LIGHT_SPOT] *
                D3d9LightingRenderer::BATCH_SIZE
        };

        D3d9LightingRenderer::D3d9LightingRenderer()
        {
                for(uint8_t i = 0; i < NUM_OF_LIGHT_TYPES; ++i)
                        d3dVertexBuffers_[i] = nullptr;
                d3dVertexDeclaration_ = nullptr;

                d3dDevice_ = nullptr;

                renderTargetContainer_ = nullptr;
                frameParameters_ = nullptr;
                actorsRenderer_ = nullptr;
                textureHandler_ = nullptr;
                capabilities_ = nullptr;
        }
        D3d9LightingRenderer::~D3d9LightingRenderer()
        {
                destroy();
        }

        //----------------------------------------------------------------------------------------------------------------------------------
        bool D3d9LightingRenderer::initialize(D3d9RenderTargetContainer& renderTargetContainer,
                                              D3d9FrameParameters& frameParameters,
                                              D3d9ActorsRenderer& actorsRenderer,
                                              D3d9TextureHandler& textureHandler,
                                              D3d9Capabilities& capabilities)
        {
                destroy();

                renderTargetContainer_ = &renderTargetContainer;
                frameParameters_ = &frameParameters;
                actorsRenderer_ = &actorsRenderer;
                textureHandler_ = &textureHandler;
                capabilities_ = &capabilities;

                d3dDevice_ = D3d9Renderer::getDevice();
                if(d3dDevice_ == nullptr)
                        return false;

                // create vertex declaration
                static D3DVERTEXELEMENT9 d3dVertexElements[] =
                {
                        {0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
                        D3DDECL_END()
                };

                if(FAILED(d3dDevice_->CreateVertexDeclaration(d3dVertexElements,
                                                              &d3dVertexDeclaration_)))
                {
                        destroy();
                        return false;
                }

                // create vertex buffers
                for(uint8_t l = 0; l < NUM_OF_LIGHT_TYPES; ++l)
                {
                        if(FAILED(d3dDevice_->CreateVertexBuffer(lightVolumeGeometryBufferSizes[l], 0, 0, D3DPOOL_DEFAULT,
                                                                 &d3dVertexBuffers_[l], nullptr)))
                        {
                                destroy();
                                return false;
                        }

                        uint8_t* destinationBuffer = nullptr;
                        if(FAILED(d3dVertexBuffers_[l]->Lock(0, lightVolumeGeometryBufferSizes[l],
                                                             reinterpret_cast<void**>(&destinationBuffer), 0)))
                        {
                                destroy();
                                return false;
                        }

                        float* vertices = lightVolumeGeometryBuffer + lightVolumeGeometryBufferOffsets[l];
                        for(uint32_t i = 0; i < BATCH_SIZE; ++i)
                        {
                                float index = static_cast<float>(i);
                                for(uint32_t j = 0; j < lightVolumeGeometryNumVertices[l]; ++j)
                                        vertices[4 * j + 3] = index;

                                memcpy(reinterpret_cast<void*>(destinationBuffer), reinterpret_cast<void*>(vertices),
                                       lightVolumeGeometryVertexStride * lightVolumeGeometryNumVertices[l]);

                                destinationBuffer += lightVolumeGeometryVertexStride * lightVolumeGeometryNumVertices[l];
                        }

                        d3dVertexBuffers_[l]->Unlock();
                }

                // load vertex and pixel shaders
                D3d9Shader d3dVertexShaders[NUM_OF_VERTEX_SHADERS] =
                {
                        D3d9Shader("DirectionalLightAccPass.vsh", "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, *capabilities_),
                        D3d9Shader("PointLightAccPass.vsh",       "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, *capabilities_),
                        D3d9Shader("SpotLightAccPass.vsh",        "vs_2_0", 0, D3d9Shader::LIBRARY_EMPTY, *capabilities_),
                        D3d9Shader("SpotLightShadowPass.vsh",     "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, *capabilities_)
                };

                D3d9Shader d3dPixelShaders[NUM_OF_PIXEL_SHADERS] =
                {
                        D3d9Shader("DirectionalLightAccPass.psh", "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_),
                        D3d9Shader("PointLightAccPass.psh",       "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_),
                        D3d9Shader("SpotLightAccPass.psh",        "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_),
                        D3d9Shader("SpotLightAccPassWithShadows.psh", "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_),
                        D3d9Shader("SpotLightShadowPass.psh",         "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_)
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

        //----------------------------------------------------------------------------------------------------------------------------------
        void D3d9LightingRenderer::destroy()
        {
                for(uint8_t i = 0; i < NUM_OF_VERTEX_SHADERS; ++i)
                        vertexShaders_[i].destroy();

                for(uint8_t i = 0; i < NUM_OF_PIXEL_SHADERS; ++i)
                        pixelShaders_[i].destroy();

                for(uint8_t i = 0; i < NUM_OF_LIGHT_TYPES; ++i)
                {
                        SAFE_RELEASE(d3dVertexBuffers_[i]);
                }
                SAFE_RELEASE(d3dVertexDeclaration_);

                d3dDevice_ = nullptr;

                renderTargetContainer_ = nullptr;
                frameParameters_ = nullptr;
                actorsRenderer_ = nullptr;
                textureHandler_ = nullptr;
                capabilities_ = nullptr;
        }

        //----------------------------------------------------------------------------------------------------------------------------------
        void D3d9LightingRenderer::renderLighting(Renderer::Data::LightNode& lightNode)
        {
                if(d3dDevice_ == nullptr)
                        return;

                d3dDevice_->SetRenderTarget(0, renderTargetContainer_->getRenderTarget(RENDER_TARGET_LIGHT_BUFFER).getSurface());
                d3dDevice_->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

                d3dDevice_->SetVertexDeclaration(d3dVertexDeclaration_);
                d3dDevice_->SetStreamSource(1, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(2, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(3, nullptr, 0, 0);
                d3dDevice_->SetIndices(nullptr);

                // render without shadows
                renderLightingWithoutShadows(lightNode, Renderer::Data::UNIT_LIGHT_NO_SHADOWS_DIRECTIONAL);

                if(!IS_SET(frameParameters_->renderingFlags, RENDERING_SHADOWS_ENABLED))
                {
                        renderLightingWithoutShadows(lightNode, Renderer::Data::UNIT_LIGHT_DIRECTIONAL);
                        d3dDevice_->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
                        return;
                }

                // render with shadows
                for(bool result = lightNode.readFirstElement(Renderer::Data::UNIT_LIGHT_SPOT); result;
                         result = lightNode.readNextElement())
                {
                        auto actorNode = lightNode.getCurrentData();
                        auto spotLight = static_cast<SpotLight*>(lightNode.getCurrentKey());

                        if(actorNode == nullptr || spotLight == nullptr)
                                break;

                        renderShadowMap(*actorNode, *spotLight);
                        prepareLightAccumulation();

                        // set shadow map at sampler 2
                        textureHandler_->setStageState(2, D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE,
                                                       D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
                        d3dDevice_->SetTexture(2, renderTargetContainer_->getRenderTarget(RENDER_TARGET_RESULT).getTexture());

                        d3dDevice_->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
                        d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                        d3dDevice_->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATEREQUAL);

                        vertexShaders_[VERTEX_SHADER_SPOT_LIGHT_ACCUMULATION].set();
                        pixelShaders_[PIXEL_SHADER_SPOT_LIGHT_ACCUMULATION_WITH_SHADOWS].set();

                        d3dDevice_->SetVertexDeclaration(d3dVertexDeclaration_);
                        d3dDevice_->SetStreamSource(0, d3dVertexBuffers_[LIGHT_SPOT],
                                                    0, lightVolumeGeometryVertexStride);
                        d3dDevice_->SetStreamSource(1, nullptr, 0, 0);
                        d3dDevice_->SetStreamSource(2, nullptr, 0, 0);
                        d3dDevice_->SetStreamSource(3, nullptr, 0, 0);
                        d3dDevice_->SetIndices(nullptr);

                        Vector4d position(spotLight->getPosition(), spotLight->getRadius());
                        Vector4d direction(spotLight->getDirection(), spotLight->getCosTheta());
                        Vector4d color(spotLight->getColor(), spotLight->getIntensity());

                        renderLightGeometry(LIGHT_SPOT, 1, &position, &direction, &color);
                }

                d3dDevice_->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        }

        //----------------------------------------------------------------------------------------------------------------------------------
        void D3d9LightingRenderer::renderLightingWithoutShadows(Renderer::Data::LightNode& lightNode,
                                                                uint8_t lightUnit)
        {
                prepareLightAccumulation();
                uint8_t shaderNo = 0;

                static Vector4d positions[BATCH_SIZE];
                static Vector4d directions[BATCH_SIZE];
                static Vector4d colors[BATCH_SIZE];

                d3dDevice_->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
                d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                d3dDevice_->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATER);

                for(uint8_t lightType = 0; lightType < NUM_OF_LIGHT_TYPES; ++lightType, ++lightUnit)
                {
                        vertexShaders_[VERTEX_SHADER_DIRECTIONAL_LIGHT_ACCUMULATION + shaderNo].set();
                        pixelShaders_[PIXEL_SHADER_DIRECTIONAL_LIGHT_ACCUMULATION + shaderNo].set();

                        d3dDevice_->SetStreamSource(0, d3dVertexBuffers_[lightType],
                                                    0, lightVolumeGeometryVertexStride);

                        uint32_t numLights = 0;

                        for(bool result = lightNode.readFirstElement(lightUnit); result;
                                 result = lightNode.readNextElement())
                        {
                                auto light = lightNode.getCurrentKey();

                                if(light == nullptr)
                                        break;

                                colors[numLights].define(light->getColor(), light->getIntensity());

                                switch(light->getRenderingUnit())
                                {
                                        case Renderer::Data::UNIT_LIGHT_NO_SHADOWS_DIRECTIONAL:
                                        case Renderer::Data::UNIT_LIGHT_DIRECTIONAL:
                                        {
                                                DirectionalLight* directionalLight = static_cast<DirectionalLight*>(light);
                                                directions[numLights].define(directionalLight->getDirection(), directionalLight->getSize());
                                                break;
                                        }
                                        case Renderer::Data::UNIT_LIGHT_NO_SHADOWS_POINT:
                                        case Renderer::Data::UNIT_LIGHT_POINT:
                                        {
                                                PointLight* pointLight = static_cast<PointLight*>(light);
                                                positions[numLights].define(pointLight->getPosition(), pointLight->getRadius());
                                                break;
                                        }

                                        case Renderer::Data::UNIT_LIGHT_NO_SHADOWS_SPOT:
                                        case Renderer::Data::UNIT_LIGHT_SPOT:
                                        {
                                                SpotLight* spotLight = static_cast<SpotLight*>(light);

                                                positions[numLights].define(spotLight->getPosition(),   spotLight->getRadius());
                                                directions[numLights].define(spotLight->getDirection(), spotLight->getCosTheta());
                                                break;
                                        }

                                        default:
                                                break;
                                }

                                ++numLights;

                                if(numLights == BATCH_SIZE)
                                {
                                        renderLightGeometry(lightType, numLights, positions, directions, colors);
                                        numLights = 0;
                                }
                        }

                        if(numLights > 0)
                                renderLightGeometry(lightType, numLights, positions, directions, colors);

                        ++shaderNo;
                }
        }

        //----------------------------------------------------------------------------------------------------------------------------------
        void D3d9LightingRenderer::prepareLightAccumulation()
        {
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

                d3dDevice_->SetVertexShaderConstantF(LOCATION_VIEW_PROJECTION_MATRIX,
                                                     static_cast<const float*>(frameParameters_->viewProjectionMatrix), 4);
                d3dDevice_->SetVertexShaderConstantF(LOCATION_NORMALS_MATRIX,
                                                     static_cast<const float*>(frameParameters_->normalsMatrix), 4);
                d3dDevice_->SetVertexShaderConstantF(LOCATION_VIEW_MATRIX,
                                                     static_cast<const float*>(frameParameters_->viewMatrix), 4);

                d3dDevice_->SetPixelShaderConstantF(LOCATION_TEXTURE_COORDINATES_ADJUSTMENT,
                                                    static_cast<const float*>(frameParameters_->textureCoordinatesAdjustment), 1);
                d3dDevice_->SetPixelShaderConstantF(LOCATION_UNPROJECTION_VECTOR,
                                                    static_cast<const float*>(frameParameters_->unprojectionVector), 1);
                d3dDevice_->SetPixelShaderConstantF(LOCATION_PROJECTION_PARAMETERS,
                                                    static_cast<const float*>(frameParameters_->projectionParameters), 1);

                d3dDevice_->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ONE);
                d3dDevice_->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
                d3dDevice_->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        }

        //----------------------------------------------------------------------------------------------------------------------------------
        void D3d9LightingRenderer::renderLightGeometry(uint8_t type, uint32_t numLights,
                                                       Vector4d* positions,
                                                       Vector4d* directions,
                                                       Vector4d* colors)
        {
                switch(type)
                {
                        case LIGHT_DIRECTIONAL:
                                d3dDevice_->SetVertexShaderConstantF(LOCATION_DIRECTIONAL_LIGHT_DIRECTION,
                                                                     reinterpret_cast<const float*>(directions),
                                                                     numLights);
                                d3dDevice_->SetVertexShaderConstantF(LOCATION_DIRECTIONAL_LIGHT_COLOR,
                                                                     reinterpret_cast<const float*>(colors),
                                                                     numLights);
                                break;

                        case LIGHT_POINT:
                                d3dDevice_->SetVertexShaderConstantF(LOCATION_POINT_LIGHT_POSITION,
                                                                     reinterpret_cast<const float*>(positions),
                                                                     numLights);
                                d3dDevice_->SetVertexShaderConstantF(LOCATION_POINT_LIGHT_COLOR,
                                                                     reinterpret_cast<const float*>(colors),
                                                                     numLights);
                                break;

                        case LIGHT_SPOT:
                                d3dDevice_->SetVertexShaderConstantF(LOCATION_SPOT_LIGHT_POSITION,
                                                                     reinterpret_cast<const float*>(positions),
                                                                     numLights);
                                d3dDevice_->SetVertexShaderConstantF(LOCATION_SPOT_LIGHT_DIRECTION,
                                                                     reinterpret_cast<const float*>(directions),
                                                                     numLights);
                                d3dDevice_->SetVertexShaderConstantF(LOCATION_SPOT_LIGHT_COLOR,
                                                                     reinterpret_cast<const float*>(colors),
                                                                     numLights);
                                break;

                        default:
                                return;
                }

                d3dDevice_->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0,
                                          numLights * lightVolumeGeometryNumVertices[type] - 2);
        }

        //----------------------------------------------------------------------------------------------------------------------------------
        void D3d9LightingRenderer::renderShadowMap(Renderer::Data::ActorNode& actorNode, const SpotLight& spotLight)
        {
                actorsRenderer_->renderShadowMap(actorNode, spotLight.getProjectionParameters());

                // render shadow
                Matrix lightTextureMatrix, lightViewMatrix;

                lightViewMatrix    = frameParameters_->viewInvMatrix * spotLight.getViewMatrix();
                lightTextureMatrix = frameParameters_->viewInvMatrix * spotLight.getViewProjectionMatrix();

                // restore original depth stencil surface
                d3dDevice_->SetDepthStencilSurface(renderTargetContainer_->getBackBuffer().getDepthStencilSurface());
                d3dDevice_->SetRenderTarget(0, renderTargetContainer_->getRenderTarget(RENDER_TARGET_RESULT).getSurface());

                d3dDevice_->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
                d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                d3dDevice_->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATEREQUAL);

                textureHandler_->setStageState(LOCATION_POSITIONS_MAP,
                                               D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE,
                                               D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(LOCATION_POSITIONS_MAP,
                                       renderTargetContainer_->getRenderTarget(RENDER_TARGET_POSITIONS).getTexture());

                textureHandler_->setStageState(LOCATION_SHADOW_MAP,
                                               D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE,
                                               D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(LOCATION_SHADOW_MAP,
                                       renderTargetContainer_->getShadowMap().getTexture());

                vertexShaders_[VERTEX_SHADER_SPOT_LIGHT_SHADOW_PASS].set();
                pixelShaders_[PIXEL_SHADER_SPOT_LIGHT_SHADOW_PASS].set();

                const Vector4d& lightProjectionParameters = spotLight.getProjectionParameters();
                Vector4d bias(lightProjectionParameters.w * 0.0375f);

                d3dDevice_->SetVertexShaderConstantF(LOCATION_VIEW_PROJECTION_MATRIX,
                                                     static_cast<const float*>(frameParameters_->viewProjectionMatrix), 4);
                d3dDevice_->SetVertexShaderConstantF(LOCATION_NORMALS_MATRIX,
                                                     static_cast<const float*>(frameParameters_->normalsMatrix), 4);
                d3dDevice_->SetVertexShaderConstantF(LOCATION_VIEW_MATRIX,
                                                     static_cast<const float*>(frameParameters_->viewMatrix), 4);

                d3dDevice_->SetPixelShaderConstantF(LOCATION_TEXTURE_COORDINATES_ADJUSTMENT,
                                                    static_cast<const float*>(frameParameters_->textureCoordinatesAdjustment), 1);
                d3dDevice_->SetPixelShaderConstantF(LOCATION_UNPROJECTION_VECTOR,
                                                    static_cast<const float*>(frameParameters_->unprojectionVector), 1);
                d3dDevice_->SetPixelShaderConstantF(LOCATION_PROJECTION_PARAMETERS,
                                                    static_cast<const float*>(frameParameters_->projectionParameters), 1);

                d3dDevice_->SetPixelShaderConstantF(LOCATION_LIGHT_PROJECTION_PARAMETERS,
                                                    static_cast<const float*>(lightProjectionParameters), 1);
                d3dDevice_->SetPixelShaderConstantF(LOCATION_LIGHT_VIEW_MATRIX,
                                                    static_cast<const float*>(lightViewMatrix), 4);
                d3dDevice_->SetPixelShaderConstantF(LOCATION_LIGHT_TEXTURE_MATRIX,
                                                    static_cast<const float*>(lightTextureMatrix), 4);
                d3dDevice_->SetPixelShaderConstantF(LOCATION_SHADOW_MAP_BIAS,
                                                    static_cast<const float*>(bias), 1);
                d3dDevice_->SetPixelShaderConstantF(LOCATION_SHADOW_MAP_KERNEL_SIZE,
                                                    static_cast<const float*>(frameParameters_->shadowMapKernelSize), 1);

                d3dDevice_->SetVertexDeclaration(d3dVertexDeclaration_);
                d3dDevice_->SetStreamSource(0, d3dVertexBuffers_[LIGHT_SPOT],
                                            0, lightVolumeGeometryVertexStride);
                d3dDevice_->SetStreamSource(1, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(2, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(3, nullptr, 0, 0);
                d3dDevice_->SetIndices(nullptr);

                Vector4d lightPosition(spotLight.getPosition(), spotLight.getRadius());
                Vector4d lightDirection(spotLight.getDirection(), spotLight.getCosTheta());

                d3dDevice_->SetVertexShaderConstantF(LOCATION_LIGHT_POSITION,
                                                     static_cast<const float*>(lightPosition),  1);
                d3dDevice_->SetVertexShaderConstantF(LOCATION_LIGHT_DIRECTION,
                                                     static_cast<const float*>(lightDirection), 1);

                d3dDevice_->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, lightVolumeGeometryNumVertices[LIGHT_SPOT] - 2);

                d3dDevice_->SetRenderTarget(0, renderTargetContainer_->getRenderTarget(RENDER_TARGET_LIGHT_BUFFER).getSurface());
        }

}
