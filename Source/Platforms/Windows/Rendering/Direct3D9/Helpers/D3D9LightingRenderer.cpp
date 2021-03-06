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
                -1.0f,  1.0f, 1.0f, 0.0f,
                 1.0f, -1.0f, 1.0f, 0.0f,
                 1.0f,  1.0f, 1.0f, 0.0f,

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

        D3d9LightingRenderer::D3d9LightingRenderer():
                d3dVertexDeclaration_(nullptr), d3dDevice_(nullptr),
                renderTargetContainer_(nullptr), frameParameters_(nullptr),
                actorsRenderer_(nullptr), textureHandler_(nullptr), capabilities_(nullptr)
        {
                for(uint8_t i = 0; i < NUM_OF_LIGHT_TYPES; ++i)
                        d3dVertexBuffers_[i] = nullptr;
        }
        D3d9LightingRenderer::~D3d9LightingRenderer()
        {
                destroy();
        }

        //------------------------------------------------------------------------------------------------------------
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
                        if(FAILED(d3dDevice_->CreateVertexBuffer(lightVolumeGeometryBufferSizes[l],
                                                                 0, 0, D3DPOOL_DEFAULT,
                                                                 &d3dVertexBuffers_[l],
                                                                 nullptr)))
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

                                destinationBuffer +=
                                        lightVolumeGeometryVertexStride * lightVolumeGeometryNumVertices[l];
                        }

                        d3dVertexBuffers_[l]->Unlock();
                }

                static const char* vertexShaderDirectionalLightAccumulationPass =
                        "float4x4 normalsMatrix: register(c4);"
                        ""
                        "float4 lightDirections[30]: register(c12);"
                        "float4 lightColors[30]:     register(c42);"
                        ""
                        "struct Input"
                        "{"
                        "        float4 position: POSITION0;"
                        "};"
                        ""
                        "struct Output"
                        "{"
                        "        float4 position:           POSITION;"
                        "        float4 lightColor:         TEXCOORD0;"
                        "        float4 lightDirection:     TEXCOORD1;"
                        "        float4 textureCoordinates: TEXCOORD2;"
                        "};"
                        ""
                        "Output main(Input input)"
                        "{"
                        "        Output output;"
                        ""
                        "        output.position = float4(input.position.xyz, 1.0);"
                        "        output.lightColor = lightColors[input.position.w];"
                        "        output.lightDirection = mul(normalsMatrix, lightDirections[input.position.w]);"
                        "        output.textureCoordinates = output.position;"
                        ""
                        "        return output;"
                        "}";

                static const char* pixelShaderDirectionalLightAccumulationPass =
                        "float4 textureCoordinatesAdjustment: register(c0);"
                        ""
                        "float4 unprojectionVector:   register(c1);"
                        "float4 projectionParameters: register(c2);"
                        ""
                        "sampler2D positionsBuffer: register(s0);"
                        "sampler2D normalsBuffer:   register(s1);"
                        ""
                        "struct Input"
                        "{"
                        "        float4 lightColor:         TEXCOORD0;"
                        "        float4 lightDirection:     TEXCOORD1;"
                        "        float4 textureCoordinates: TEXCOORD2;"
                        "};"
                        ""
                        "float4 main(Input input): COLOR0"
                        "{"
                        "        input.textureCoordinates.xy /= input.textureCoordinates.w;"
                        "        input.textureCoordinates.xy += textureCoordinatesAdjustment.xy;"
                        ""
                        "        float2 textureCoordinates = 0.5f * (float2( input.textureCoordinates.x,"
                        "                                                   -input.textureCoordinates.y) + 1.0);"
                        ""
                        "        float4 encodedNormalAndSpecularPower = tex2D(normalsBuffer, textureCoordinates);"
                        "        encodedNormalAndSpecularPower.w *= 200.0;"
                        ""
                        "        float3 position = decodePosition(tex2D(positionsBuffer, textureCoordinates),"
                        "                                         input.textureCoordinates.xy,"
                        "                                         projectionParameters,"
                        "                                         unprojectionVector);"
                        "        float3 normal = normalize(decodeNormal(encodedNormalAndSpecularPower.xyz));"
                        ""
                        "        float3 toLight = -input.lightDirection.xyz;"
                        "        float3 toLightNormalized = normalize(toLight);"
                        ""
                        "        float3 v = normalize(-position);"
                        "        float3 h = normalize(toLightNormalized + v);"
                        "        float nDotL = dot(normal, toLightNormalized);"
                        ""
                        "        return float4((input.lightColor.xyz * nDotL * input.lightColor.w),"
                        "                      nDotL * pow(dot(normal, h), encodedNormalAndSpecularPower.w));"
                        "}";

                static const char* vertexShaderPointLightAccumulationPass =
                        "float4x4 viewProjectionMatrix: register(c0);"
                        "float4x4 viewMatrix:           register(c8);"
                        ""
                        "float4 lightPositions[30]: register(c12);"
                        "float4 lightColors[30]:    register(c42);"
                        ""
                        "struct Input"
                        "{"
                        "        float4 position: POSITION0;"
                        "};"
                        ""
                        "struct Output"
                        "{"
                        "        float4 position:           POSITION;"
                        "        float4 lightColor:         TEXCOORD0;"
                        "        float4 lightPosition:      TEXCOORD1;"
                        "        float4 textureCoordinates: TEXCOORD2;"
                        "};"
                        ""
                        "Output main(Input input)"
                        "{"
                        "        Output output;"
                        ""
                        "        float4 lightPosition  = lightPositions[input.position.w];"
                        "        float3 vertexPosition = lightPosition.xyz + input.position.xyz *"
                        "                                lightPosition.w * 1.268;"
                        ""
                        "        output.position = mul(viewProjectionMatrix, float4(vertexPosition, 1.0));"
                        "        output.lightColor = lightColors[input.position.w];"
                        "        output.lightPosition = mul(viewMatrix, float4(lightPosition.xyz, 1.0));"
                        "        output.lightPosition.w = 1.0 / (lightPosition.w * lightPosition.w);"
                        "        output.textureCoordinates = output.position;"
                        ""
                        "        return output;"
                        "}";

                static const char* pixelShaderPointLightAccumulationPass =
                        "float4 textureCoordinatesAdjustment: register(c0);"
                        ""
                        "float4 unprojectionVector:   register(c1);"
                        "float4 projectionParameters: register(c2);"
                        ""
                        "sampler2D positionsBuffer: register(s0);"
                        "sampler2D normalsBuffer:   register(s1);"
                        ""
                        "struct Input"
                        "{"
                        "        float4 lightColor:         TEXCOORD0;"
                        "        float4 lightPosition:      TEXCOORD1;"
                        "        float4 textureCoordinates: TEXCOORD2;"
                        "};"
                        ""
                        "float4 main(Input input): COLOR0"
                        "{"
                        "        input.textureCoordinates.xy /= input.textureCoordinates.w;"
                        "        input.textureCoordinates.xy += textureCoordinatesAdjustment.xy;"
                        ""
                        "        float2 textureCoordinates = 0.5f * (float2( input.textureCoordinates.x,"
                        "                                                   -input.textureCoordinates.y) + 1.0);"
                        ""
                        "        float4 encodedNormalAndSpecularPower = tex2D(normalsBuffer, textureCoordinates);"
                        "        encodedNormalAndSpecularPower.w *= 200.0;"
                        ""
                        "        float3 position = decodePosition(tex2D(positionsBuffer, textureCoordinates),"
                        "                                         input.textureCoordinates.xy,"
                        "                                         projectionParameters,"
                        "                                         unprojectionVector);"
                        "        float3 normal = normalize(decodeNormal(encodedNormalAndSpecularPower.xyz));"
                        ""
                        "        float3 toLight = input.lightPosition.xyz - position;"
                        "        float3 toLightNormalized = normalize(toLight);"
                        ""
                        "        float attenuation = clamp(1.0 - dot(toLight, toLight) *"
                        "                                  input.lightPosition.w, 0.0, 1.0);"
                        ""
                        "        float3 v = normalize(-position);"
                        "        float3 h = normalize(toLightNormalized + v);"
                        "        float nDotL = dot(normal, toLightNormalized) * attenuation;"
                        ""
                        "        return float4((input.lightColor.xyz * nDotL * input.lightColor.w),"
                        "                      nDotL * pow(dot(normal, h), encodedNormalAndSpecularPower.w));"
                        "}";

                static const char* vertexShaderSpotLightAccumulationPass =
                        "float4x4 viewProjectionMatrix: register(c0);"
                        "float4x4 normalsMatrix:        register(c4);"
                        "float4x4 viewMatrix:           register(c8);"
                        ""
                        "float4 lightPositions[30]:  register(c12);"
                        "float4 lightDirections[30]: register(c42);"
                        "float4 lightColors[30]:     register(c72);"
                        ""
                        "struct Input"
                        "{"
                        "        float4 position: POSITION0;"
                        "};"
                        ""
                        "struct Output"
                        "{"
                        "        float4 position:           POSITION;"
                        "        float4 lightColor:         TEXCOORD0;"
                        "        float4 lightPosition:      TEXCOORD1;"
                        "        float4 lightDirection:     TEXCOORD2;"
                        "        float4 textureCoordinates: TEXCOORD3;"
                        "};"
                        ""
                        "Output main(Input input)"
                        "{"
                        "        Output output;"
                        ""
                        "        float4 lightPosition  = lightPositions[input.position.w];"
                        "        float4 lightDirection = lightDirections[input.position.w];"
                        ""
                        "        float3 newZ = normalize(lightDirection.xyz);"
                        "        float3 newX, newY;"
                        "        if(abs(newZ.x) < 0.001 && abs(newZ.y) < 0.001)"
                        "                newX = cross(float3(0.0, 1.0, 0.0), newZ);"
                        "        else"
                        "                newX = cross(float3(0.0, 0.0, 1.0), newZ);"
                        "        newY = cross(newZ, newX);"
                        "        float3x3 conversionMatrix = {newX, newY, newZ};"
                        ""
                        "        float attenuationFactor = (1.0 - lightDirection.w);"
                        "        attenuationFactor *= attenuationFactor;"
                        "        attenuationFactor = 1.0 / attenuationFactor;"
                        ""
                        "        output.lightDirection = float4(mul(normalsMatrix, -newZ).xyz,"
                        "                                       attenuationFactor);"
                        ""
                        "        float3 position = float3(input.position.xy, 0.0);"
                        "        float3 newPosition = mul(position, conversionMatrix);"
                        ""
                        "        position = lightPosition.xyz + lightDirection.xyz * input.position.z +"
                        "                   newPosition * lightPosition.w * 1.268;"
                        ""
                        "        output.position = mul(viewProjectionMatrix, float4(position, 1.0));"
                        "        output.lightColor = lightColors[input.position.w];"
                        "        output.lightPosition = mul(viewMatrix, float4(lightPosition.xyz, 1.0));"
                        "        output.lightPosition.w = 1.0 / dot(lightDirection.xyz, lightDirection.xyz);"
                        "        output.textureCoordinates = output.position;"
                        ""
                        "        return output;"
                        "}";

                static const char* pixelShaderSpotLightAccumulationPass =
                        "float4 textureCoordinatesAdjustment: register(c0);"
                        ""
                        "float4 unprojectionVector:   register(c1);"
                        "float4 projectionParameters: register(c2);"
                        ""
                        "sampler2D positionsBuffer: register(s0);"
                        "sampler2D normalsBuffer:   register(s1);"
                        ""
                        "struct Input"
                        "{"
                        "        float4 lightColor:         TEXCOORD0;"
                        "        float4 lightPosition:      TEXCOORD1;"
                        "        float4 lightDirection:     TEXCOORD2;"
                        "        float4 textureCoordinates: TEXCOORD3;"
                        "};"
                        ""
                        "float4 main(Input input): COLOR0"
                        "{"
                        "        input.textureCoordinates.xy /= input.textureCoordinates.w;"
                        "        input.textureCoordinates.xy += textureCoordinatesAdjustment.xy;"
                        ""
                        "        float2 textureCoordinates = 0.5f * (float2( input.textureCoordinates.x,"
                        "                                                   -input.textureCoordinates.y) + 1.0);"
                        ""
                        "        float4 encodedNormalAndSpecularPower = tex2D(normalsBuffer, textureCoordinates);"
                        "        encodedNormalAndSpecularPower.w *= 200.0;"
                        ""
                        "        float3 position = decodePosition(tex2D(positionsBuffer, textureCoordinates),"
                        "                                         input.textureCoordinates.xy,"
                        "                                         projectionParameters,"
                        "                                         unprojectionVector);"
                        "        float3 normal = normalize(decodeNormal(encodedNormalAndSpecularPower.xyz));"
                        ""
                        "        float3 toLight = input.lightPosition.xyz - position;"
                        "        float3 toLightNormalized = normalize(toLight);"
                        ""
                        "        float factor = 1.0 - dot(toLightNormalized,"
                        "                                 normalize(input.lightDirection.xyz));"
                        "        float attenuation = clamp(1.0 - factor * factor *"
                        "                                  input.lightDirection.w, 0.0, 1.0);"
                        "        attenuation = min(attenuation,"
                        "                          clamp(1.0 - dot(toLight, toLight) *"
                        "                                input.lightPosition.w, 0.0, 1.0));"
                        ""
                        "        float3 v = normalize(-position);"
                        "        float3 h = normalize(toLightNormalized + v);"
                        "        float nDotL = dot(normal, toLightNormalized) * attenuation;"
                        ""
                        "        return float4((input.lightColor.xyz * nDotL * input.lightColor.w),"
                        "                      nDotL * pow(dot(normal, h), encodedNormalAndSpecularPower.w));"
                        "}";

                static const char* pixelShaderSpotLightAccumulationPassWithShadows =
                        "float4 textureCoordinatesAdjustment: register(c0);"
                        ""
                        "float4 unprojectionVector:   register(c1);"
                        "float4 projectionParameters: register(c2);"
                        ""
                        "sampler2D positionsBuffer: register(s0);"
                        "sampler2D normalsBuffer:   register(s1);"
                        "sampler2D shadowsBuffer:   register(s2);"
                        ""
                        "struct Input"
                        "{"
                        "        float4 lightColor:         TEXCOORD0;"
                        "        float4 lightPosition:      TEXCOORD1;"
                        "        float4 lightDirection:     TEXCOORD2;"
                        "        float4 textureCoordinates: TEXCOORD3;"
                        "};"
                        ""
                        "float4 main(Input input): COLOR0"
                        "{"
                        "        input.textureCoordinates.xy /= input.textureCoordinates.w;"
                        "        input.textureCoordinates.xy += textureCoordinatesAdjustment.xy;"
                        ""
                        "        float2 textureCoordinates = 0.5f * (float2( input.textureCoordinates.x,"
                        "                                                   -input.textureCoordinates.y) + 1.0);"
                        ""
                        "        float4 shadow = tex2D(shadowsBuffer, textureCoordinates);"
                        ""
                        "        float4 encodedNormalAndSpecularPower = tex2D(normalsBuffer, textureCoordinates);"
                        "        encodedNormalAndSpecularPower.w *= 200.0;"
                        ""
                        "        float3 position = decodePosition(tex2D(positionsBuffer, textureCoordinates),"
                        "                                         input.textureCoordinates.xy,"
                        "                                         projectionParameters,"
                        "                                         unprojectionVector);"
                        "        float3 normal = normalize(decodeNormal(encodedNormalAndSpecularPower.xyz));"
                        ""
                        "        float3 toLight = input.lightPosition.xyz - position;"
                        "        float3 toLightNormalized = normalize(toLight);"
                        ""
                        "        float factor = 1.0 - dot(toLightNormalized,"
                        "                                 normalize(input.lightDirection.xyz));"
                        "        float attenuation = clamp(1.0 - factor * factor *"
                        "                                  input.lightDirection.w, 0.0, 1.0);"
                        "        attenuation = min(attenuation,"
                        "                          clamp(1.0 - dot(toLight, toLight) *"
                        "                                input.lightPosition.w, 0.0, 1.0));"
                        ""
                        "        float3 v = normalize(-position);"
                        "        float3 h = normalize(toLightNormalized + v);"
                        "        float nDotL = dot(normal, toLightNormalized) * attenuation * shadow.x;"
                        ""
                        "        return float4((input.lightColor.xyz * nDotL * input.lightColor.w),"
                        "                      nDotL * pow(dot(normal, h), encodedNormalAndSpecularPower.w));"
                        "}";

                static const char* vertexShaderSpotLightShadowsPass =
                        "float4x4 viewProjectionMatrix: register(c0);"
                        ""
                        "float4 lightPosition:  register(c12);"
                        "float4 lightDirection: register(c13);"
                        ""
                        "struct Input"
                        "{"
                        "        float4 position: POSITION0;"
                        "};"
                        ""
                        "struct Output"
                        "{"
                        "        float4 position:           POSITION;"
                        "        float4 textureCoordinates: TEXCOORD0;"
                        "};"
                        ""
                        "Output main(Input input)"
                        "{"
                        "        Output output;"
                        ""
                        "        float3 newZ = normalize(lightDirection.xyz);"
                        "        float3 newX, newY;"
                        "        if(abs(newZ.x) < 0.001 && abs(newZ.y) < 0.001)"
                        "                newX = cross(float3(0.0, 1.0, 0.0), newZ);"
                        "        else"
                        "                newX = cross(float3(0.0, 0.0, 1.0), newZ);"
                        "        newY = cross(newZ, newX);"
                        "        float3x3 conversionMatrix = {newX, newY, newZ};"
                        ""
                        "        float attenuationFactor = (1.0 - lightDirection.w);"
                        "        attenuationFactor *= attenuationFactor;"
                        "        attenuationFactor = 1.0 / attenuationFactor;"
                        ""
                        "        float3 position = float3(input.position.xy, 0.0);"
                        "        float3 newPosition = mul(position, conversionMatrix);"
                        ""
                        "        position = lightPosition.xyz + lightDirection.xyz * input.position.z +"
                        "                   newPosition * lightPosition.w * 1.268;"
                        ""
                        "        output.position = mul(viewProjectionMatrix, float4(position, 1.0));"
                        "        output.textureCoordinates = output.position;"
                        ""
                        "        return output;"
                        "}";

                static const char* pixelShaderSpotLightShadowsPass =
                        "float4 textureCoordinatesAdjustment: register(c0);"
                        ""
                        "float4 unprojectionVector:        register(c1);"
                        "float4 projectionParameters:      register(c2);"
                        "float4 lightProjectionParameters: register(c3);"
                        ""
                        "float4x4 lightViewMatrix:    register(c4);"
                        "float4x4 lightTextureMatrix: register(c8);"
                        ""
                        "float4 depthBias:           register(c12);"
                        "float4 shadowMapKernelSize: register(c13);"
                        ""
                        "sampler2D positionsBuffer: register(s0);"
                        "sampler2D shadowMap:       register(s1);"
                        ""
                        "struct Input"
                        "{"
                        "        float4 textureCoordinates: TEXCOORD0;"
                        "};"
                        ""
                        "float computeShadow(in float2 textureCoordinates, in float z)"
                        "{"
                        "        float shadowMapZ = decodeEyeZ(tex2D(shadowMap, textureCoordinates.xy),"
                        "                                      lightProjectionParameters) + depthBias.x;"
                        "        return (z >= shadowMapZ) ? 0.0 : 1.0;"
                        "}"
                        ""
                        "float4 main(Input input): COLOR0"
                        "{"
                        "        input.textureCoordinates.xy /= input.textureCoordinates.w;"
                        "        input.textureCoordinates.xy += textureCoordinatesAdjustment.xy;"
                        ""
                        "        float2 textureCoordinates = 0.5f * (float2( input.textureCoordinates.x,"
                        "                                                   -input.textureCoordinates.y) + 1.0);"
                        ""
                        "        float3 position = decodePosition(tex2D(positionsBuffer, textureCoordinates),"
                        "                                         input.textureCoordinates.xy,"
                        "                                         projectionParameters,"
                        "                                         unprojectionVector);"
                        ""
                        "        float4 lightSpacePosition = mul(lightViewMatrix, float4(position, 1.0));"
                        "        lightSpacePosition.xyz /= lightSpacePosition.w;"
                        ""
                        "        float4 lightSpaceTextureCoordinates ="
                        "                mul(lightTextureMatrix, float4(position, 1.0));"
                        "        lightSpaceTextureCoordinates.xyz /= lightSpaceTextureCoordinates.w;"
                        "        lightSpaceTextureCoordinates.xy = 0.5 * (float2( lightSpaceTextureCoordinates.x,"
                        "                                                        -lightSpaceTextureCoordinates.y) +"
                        "                                                 1.0);"
                        ""
                        "        float shadow = computeShadow(lightSpaceTextureCoordinates.xy,"
                        "                                     lightSpacePosition.z);"
                        "        return shadow.xxxx;"
                        "}";

                // load vertex and pixel shaders
                D3d9Shader d3dVertexShaders[NUM_OF_VERTEX_SHADERS] =
                {
                        D3d9Shader(vertexShaderDirectionalLightAccumulationPass, "vs_1_1", 0,
                                   D3d9Shader::LIBRARY_EMPTY, *capabilities_),
                        D3d9Shader(vertexShaderPointLightAccumulationPass,       "vs_1_1", 0,
                                   D3d9Shader::LIBRARY_EMPTY, *capabilities_),
                        D3d9Shader(vertexShaderSpotLightAccumulationPass,        "vs_2_0", 0,
                                   D3d9Shader::LIBRARY_EMPTY, *capabilities_),
                        D3d9Shader(vertexShaderSpotLightShadowsPass,             "vs_1_1", 0,
                                   D3d9Shader::LIBRARY_EMPTY, *capabilities_)
                };

                D3d9Shader d3dPixelShaders[NUM_OF_PIXEL_SHADERS] =
                {
                        D3d9Shader(pixelShaderDirectionalLightAccumulationPass,     "ps_2_0", 0,
                                   D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_),
                        D3d9Shader(pixelShaderPointLightAccumulationPass,           "ps_2_0", 0,
                                   D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_),
                        D3d9Shader(pixelShaderSpotLightAccumulationPass,            "ps_2_0", 0,
                                   D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_),
                        D3d9Shader(pixelShaderSpotLightAccumulationPassWithShadows, "ps_2_0", 0,
                                   D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_),
                        D3d9Shader(pixelShaderSpotLightShadowsPass,                 "ps_2_0", 0,
                                   D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_)
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

        //------------------------------------------------------------------------------------------------------------
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

        //------------------------------------------------------------------------------------------------------------
        void D3d9LightingRenderer::renderLighting(Renderer::Data::LightNode& lightNode)
        {
                if(d3dDevice_ == nullptr)
                        return;

                const auto& lightBuffer = renderTargetContainer_->getRenderTarget(RENDER_TARGET_LIGHT_BUFFER);
                d3dDevice_->SetRenderTarget(0, lightBuffer.getSurface());
                d3dDevice_->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

                d3dDevice_->SetVertexDeclaration(d3dVertexDeclaration_);
                d3dDevice_->SetStreamSource(1, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(2, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(3, nullptr, 0, 0);
                d3dDevice_->SetIndices(nullptr);

                // render without shadows
                renderLightingWithoutShadows(lightNode, Renderer::Data::UNIT_LIGHT_NO_SHADOWS_DIRECTIONAL);

                if(frameParameters_->shadowsQuality == 0)
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
                        const auto& shadowMap = renderTargetContainer_->getRenderTarget(RENDER_TARGET_RESULT);
                        textureHandler_->setStageState(2, D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE,
                                                       D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
                        d3dDevice_->SetTexture(2, shadowMap.getTexture());

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

        //------------------------------------------------------------------------------------------------------------
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
                                                DirectionalLight* directionalLight =
                                                        static_cast<DirectionalLight*>(light);
                                                directions[numLights].define(directionalLight->getDirection(),
                                                                             directionalLight->getSize());
                                                break;
                                        }
                                        case Renderer::Data::UNIT_LIGHT_NO_SHADOWS_POINT:
                                        case Renderer::Data::UNIT_LIGHT_POINT:
                                        {
                                                PointLight* pointLight = static_cast<PointLight*>(light);
                                                positions[numLights].define(pointLight->getPosition(),
                                                                            pointLight->getRadius());
                                                break;
                                        }

                                        case Renderer::Data::UNIT_LIGHT_NO_SHADOWS_SPOT:
                                        case Renderer::Data::UNIT_LIGHT_SPOT:
                                        {
                                                SpotLight* spotLight = static_cast<SpotLight*>(light);

                                                positions[numLights].define(spotLight->getPosition(),
                                                                            spotLight->getRadius());
                                                directions[numLights].define(spotLight->getDirection(),
                                                                             spotLight->getCosTheta());
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

        //------------------------------------------------------------------------------------------------------------
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
                                                     frameParameters_->viewProjectionMatrix,
                                                     4);
                d3dDevice_->SetVertexShaderConstantF(LOCATION_NORMALS_MATRIX,
                                                     frameParameters_->normalsMatrix,
                                                     4);
                d3dDevice_->SetVertexShaderConstantF(LOCATION_VIEW_MATRIX,
                                                     frameParameters_->viewMatrix,
                                                     4);

                d3dDevice_->SetPixelShaderConstantF(LOCATION_TEXTURE_COORDINATES_ADJUSTMENT,
                                                    frameParameters_->textureCoordinatesAdjustment,
                                                    1);
                d3dDevice_->SetPixelShaderConstantF(LOCATION_UNPROJECTION_VECTOR,
                                                    frameParameters_->unprojectionVector,
                                                    1);
                d3dDevice_->SetPixelShaderConstantF(LOCATION_PROJECTION_PARAMETERS,
                                                    frameParameters_->projectionParameters,
                                                    1);

                d3dDevice_->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ONE);
                d3dDevice_->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
                d3dDevice_->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        }

        //------------------------------------------------------------------------------------------------------------
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

        //------------------------------------------------------------------------------------------------------------
        void D3d9LightingRenderer::renderShadowMap(Renderer::Data::ActorNode& actorNode, const SpotLight& spotLight)
        {
                actorsRenderer_->renderShadowMap(actorNode, spotLight.getProjectionParameters());

                // render shadow
                Matrix lightTextureMatrix, lightViewMatrix;

                lightViewMatrix    = frameParameters_->viewInvMatrix * spotLight.getViewMatrix();
                lightTextureMatrix = frameParameters_->viewInvMatrix * spotLight.getViewProjectionMatrix();

                // restore original depth stencil surface
                const auto& renderTarget = renderTargetContainer_->getRenderTarget(RENDER_TARGET_RESULT);
                d3dDevice_->SetDepthStencilSurface(renderTargetContainer_->getBackBuffer().getDepthStencilSurface());
                d3dDevice_->SetRenderTarget(0, renderTarget.getSurface());

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
                                                     frameParameters_->viewProjectionMatrix,
                                                     4);
                d3dDevice_->SetVertexShaderConstantF(LOCATION_NORMALS_MATRIX,
                                                     frameParameters_->normalsMatrix,
                                                     4);
                d3dDevice_->SetVertexShaderConstantF(LOCATION_VIEW_MATRIX,
                                                     frameParameters_->viewMatrix,
                                                     4);

                d3dDevice_->SetPixelShaderConstantF(LOCATION_TEXTURE_COORDINATES_ADJUSTMENT,
                                                    frameParameters_->textureCoordinatesAdjustment,
                                                    1);
                d3dDevice_->SetPixelShaderConstantF(LOCATION_UNPROJECTION_VECTOR,
                                                    frameParameters_->unprojectionVector,
                                                    1);
                d3dDevice_->SetPixelShaderConstantF(LOCATION_PROJECTION_PARAMETERS,
                                                    frameParameters_->projectionParameters,
                                                    1);

                d3dDevice_->SetPixelShaderConstantF(LOCATION_LIGHT_PROJECTION_PARAMETERS,
                                                    lightProjectionParameters,
                                                    1);
                d3dDevice_->SetPixelShaderConstantF(LOCATION_LIGHT_VIEW_MATRIX,
                                                    lightViewMatrix,
                                                    4);
                d3dDevice_->SetPixelShaderConstantF(LOCATION_LIGHT_TEXTURE_MATRIX,
                                                    lightTextureMatrix,
                                                    4);
                d3dDevice_->SetPixelShaderConstantF(LOCATION_SHADOW_MAP_BIAS, bias, 1);
                d3dDevice_->SetPixelShaderConstantF(LOCATION_SHADOW_MAP_KERNEL_SIZE,
                                                    frameParameters_->shadowMapKernelSize,
                                                    1);

                d3dDevice_->SetVertexDeclaration(d3dVertexDeclaration_);
                d3dDevice_->SetStreamSource(0, d3dVertexBuffers_[LIGHT_SPOT],
                                            0, lightVolumeGeometryVertexStride);
                d3dDevice_->SetStreamSource(1, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(2, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(3, nullptr, 0, 0);
                d3dDevice_->SetIndices(nullptr);

                Vector4d lightPosition(spotLight.getPosition(), spotLight.getRadius());
                Vector4d lightDirection(spotLight.getDirection(), spotLight.getCosTheta());

                d3dDevice_->SetVertexShaderConstantF(LOCATION_LIGHT_POSITION, lightPosition, 1);
                d3dDevice_->SetVertexShaderConstantF(LOCATION_LIGHT_DIRECTION, lightDirection, 1);

                d3dDevice_->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, lightVolumeGeometryNumVertices[LIGHT_SPOT] - 2);

                const auto& lightBuffer = renderTargetContainer_->getRenderTarget(RENDER_TARGET_LIGHT_BUFFER);
                d3dDevice_->SetRenderTarget(0, lightBuffer.getSurface());
        }

}
