// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLESLightingRenderer.h"

#include "../../../Platform.h"
#include "../GLESRenderer.h"

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

        const uint32_t lightVolumeGeometryNumVertices[GlesLightingRenderer::NUM_OF_LIGHT_TYPES] = {4, 58, 38};
        const uint32_t lightVolumeGeometryVertexStride = sizeof(Vector4d);

        static const uint32_t lightVolumeGeometryBufferOffsets[GlesLightingRenderer::NUM_OF_LIGHT_TYPES] =
        {
                // Directional light
                0,

                // Point light
                4 * lightVolumeGeometryNumVertices[GlesLightingRenderer::LIGHT_DIRECTIONAL],

                // Spot light
                4 * lightVolumeGeometryNumVertices[GlesLightingRenderer::LIGHT_DIRECTIONAL] +
                4 * lightVolumeGeometryNumVertices[GlesLightingRenderer::LIGHT_POINT]
        };

        static const uint32_t lightVolumeGeometryBufferSizes[GlesLightingRenderer::NUM_OF_LIGHT_TYPES] =
        {
                // Directional light
                lightVolumeGeometryVertexStride *
                lightVolumeGeometryNumVertices[GlesLightingRenderer::LIGHT_DIRECTIONAL] *
                GlesLightingRenderer::BATCH_SIZE,

                // Point light
                lightVolumeGeometryVertexStride *
                lightVolumeGeometryNumVertices[GlesLightingRenderer::LIGHT_POINT] *
                GlesLightingRenderer::BATCH_SIZE,

                // Spot light
                lightVolumeGeometryVertexStride *
                lightVolumeGeometryNumVertices[GlesLightingRenderer::LIGHT_SPOT] *
                GlesLightingRenderer::BATCH_SIZE
        };

        GlesLightingRenderer::GlesLightingRenderer()
        {
                for(uint8_t i = 0; i < NUM_OF_LIGHT_TYPES; ++i)
                        vertexBuffers_[i] = 0;

                renderTargetContainer_ = nullptr;
                frameParameters_ = nullptr;
                actorsRenderer_ = nullptr;
                textureHandler_ = nullptr;
        }
        GlesLightingRenderer::~GlesLightingRenderer()
        {
                destroy();
        }

        //----------------------------------------------------------------------------------------------------------
        bool GlesLightingRenderer::initialize(GlesRenderTargetContainer& renderTargetContainer,
                                              GlesFrameParameters& frameParameters,
                                              GlesActorsRenderer& actorsRenderer,
                                              GlesTextureHandler& textureHandler)
        {
                destroy();

                renderTargetContainer_ = &renderTargetContainer;
                frameParameters_ = &frameParameters;
                actorsRenderer_ = &actorsRenderer;
                textureHandler_ = &textureHandler;

                const uint32_t maxLightVolumeGeometryBufferSize = 27840;
                static uint8_t temporaryBuffer[maxLightVolumeGeometryBufferSize];

                // create vertex buffers
                glGenBuffers(NUM_OF_LIGHT_TYPES, vertexBuffers_);
                CHECK_GLES_ERROR("GlesFullScreenQuad::initialize: glGenBuffers");

                for(uint8_t l = 0; l < NUM_OF_LIGHT_TYPES; ++l)
                {
                        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers_[l]);
                        CHECK_GLES_ERROR("GlesFullScreenQuad::initialize: glBindBuffer");

                        uint8_t* destinationBuffer = temporaryBuffer;

                        float* vertices = lightVolumeGeometryBuffer + lightVolumeGeometryBufferOffsets[l];
                        uint32_t chunkSize = lightVolumeGeometryVertexStride * lightVolumeGeometryNumVertices[l];

                        for(uint32_t i = 0; i < BATCH_SIZE; ++i)
                        {
                                float index = static_cast<float>(i);
                                for(uint32_t j = 0; j < lightVolumeGeometryNumVertices[l]; ++j)
                                        vertices[4 * j + 3] = index;

                                memcpy(reinterpret_cast<void*>(destinationBuffer), reinterpret_cast<void*>(vertices), chunkSize);

                                destinationBuffer += chunkSize;
                        }

                        glBufferData(GL_ARRAY_BUFFER, lightVolumeGeometryBufferSizes[l], temporaryBuffer, GL_STATIC_DRAW);
                        CHECK_GLES_ERROR("GlesFullScreenQuad::initialize: glBufferData");
                }

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                CHECK_GLES_ERROR("GlesFullScreenQuad::initialize: glBindBuffer");

                // directional light shaders
                static const char vertexShaderDirectionalLightAccumulation[] =
                        "invariant gl_Position;\n"
                        "attribute vec4 vertexPosition;\n"
                        "uniform mat4 normalsMatrix;\n"
                        "uniform vec4 lightColors[30];\n"
                        "uniform vec4 lightDirections[30];\n"
                        "varying vec4 vTextureCoordinates;\n"
                        "varying vec4 vLightColor;\n"
                        "varying vec4 vLightDirection;\n"
                        "void main()\n"
                        "{\n"
                        "        int lightIndex = int(vertexPosition.w);\n"
                        "        vLightColor = lightColors[lightIndex];\n"
                        "        vec4 lightDirection = lightDirections[lightIndex];\n"
                        "        gl_Position = vTextureCoordinates = vec4(vertexPosition.xyz, 1.0);\n"
                        "        vLightDirection = vec4(mat3(normalsMatrix) * lightDirection.xyz, 1.0);\n"
                        "}\n";

                static const char fragmentShaderDirectionalLightAccumulation[] =
                        "uniform sampler2D depthBuffer;\n"
                        "uniform sampler2D normalsBuffer;\n"
                        "uniform vec4 textureCoordinatesAdjustment;\n"
                        "uniform vec4 conversionParameters;\n"
                        "uniform vec4 unprojectionVector;\n"
                        "varying vec4 vTextureCoordinates;\n"
                        "varying vec4 vLightColor;\n"
                        "varying vec4 vLightDirection;\n"
                        "void main()\n"
                        "{\n"
                        "        vec3 postProjectionCoordinates = vTextureCoordinates.xyz / vTextureCoordinates.w;\n"
                        "        vec2 textureCoordinates = 0.5 * (postProjectionCoordinates.xy + vec2(1.0, 1.0));\n"
                        "        textureCoordinates *= textureCoordinatesAdjustment.xy;\n"
                        "        float depth = 2.0 * texture2D(depthBuffer, textureCoordinates).x - 1.0;"
                        "        if(depth >= postProjectionCoordinates.z)\n"
                        "                discard;\n"
                        "        vec4 encodedNormal = texture2D(normalsBuffer, textureCoordinates);\n"
                        "        encodedNormal.w *= 200.0;\n"
                        "        vec3 normal = normalize(decodeNormal(encodedNormal.xyz));\n"
                        "        vec3 position = decodePosition(postProjectionCoordinates.xy, depth,\n"
                        "                                       conversionParameters, unprojectionVector);\n"
                        "        vec3 toLightNormalized = normalize(-vLightDirection.xyz);\n"
                        "        float nDotL = dot(normal, toLightNormalized);\n"
                        "        vec3 v = normalize(-position);\n"
                        "        vec3 h = normalize(toLightNormalized + v);\n"
                        "        gl_FragColor = vec4(vLightColor.xyz * nDotL * vLightColor.w, nDotL * pow(dot(normal, h), encodedNormal.w));\n"
                        "}\n";

                // point light shaders
                static const char vertexShaderPointLightAccumulation[] =
                        "invariant gl_Position;\n"
                        "attribute vec4 vertexPosition;\n"
                        "uniform mat4 viewProjectionMatrix;\n"
                        "uniform mat4 viewMatrix;\n"
                        "uniform vec4 lightColors[30];\n"
                        "uniform vec4 lightPositions[30];\n"
                        "varying vec4 vTextureCoordinates;\n"
                        "varying vec4 vLightColor;\n"
                        "varying vec4 vLightPosition;\n"
                        "void main()\n"
                        "{\n"
                        "        int lightIndex = int(vertexPosition.w);\n"
                        "        vLightColor = lightColors[lightIndex];\n"
                        "        vec4 lightPosition = lightPositions[lightIndex];\n"
                        "        vec3 position = lightPosition.xyz + vertexPosition.xyz * lightPosition.w * 1.268;\n"
                        "        gl_Position = vTextureCoordinates = viewProjectionMatrix * vec4(position, 1.0);\n"
                        "        vec4 viewSpaceLightPosition = viewMatrix * vec4(lightPosition.xyz, 1.0);\n"
                        "        viewSpaceLightPosition.xyz /= viewSpaceLightPosition.w;\n"
                        "        vLightPosition = vec4(viewSpaceLightPosition.xyz, 1.0 / (lightPosition.w * lightPosition.w));\n"
                        "}\n";

                static const char fragmentShaderPointLightAccumulation[] =
                        "uniform sampler2D depthBuffer;\n"
                        "uniform sampler2D normalsBuffer;\n"
                        "uniform vec4 textureCoordinatesAdjustment;\n"
                        "uniform vec4 conversionParameters;\n"
                        "uniform vec4 unprojectionVector;\n"
                        "varying vec4 vTextureCoordinates;\n"
                        "varying vec4 vLightColor;\n"
                        "varying vec4 vLightPosition;\n"
                        "void main()\n"
                        "{\n"
                        "        vec3 postProjectionCoordinates = vTextureCoordinates.xyz / vTextureCoordinates.w;\n"
                        "        vec2 textureCoordinates = 0.5 * (postProjectionCoordinates.xy + vec2(1.0, 1.0));\n"
                        "        textureCoordinates *= textureCoordinatesAdjustment.xy;\n"
                        "        float depth = 2.0 * texture2D(depthBuffer, textureCoordinates).x - 1.0;"
                        "        if(depth > postProjectionCoordinates.z)\n"
                        "                discard;\n"
                        "        vec4 encodedNormal = texture2D(normalsBuffer, textureCoordinates);\n"
                        "        encodedNormal.w *= 200.0;\n"
                        "        vec3 normal = normalize(decodeNormal(encodedNormal.xyz));\n"
                        "        vec3 position = decodePosition(postProjectionCoordinates.xy, depth,\n"
                        "                                       conversionParameters, unprojectionVector);\n"
                        "        vec3 toLight = vLightPosition.xyz - position;\n"
                        "        vec3 toLightNormalized = normalize(toLight);\n"
                        "        float attenuation = clamp(1.0 - dot(toLight, toLight) * vLightPosition.w, 0.0, 1.0);\n"
                        "        float nDotL = dot(normal, toLightNormalized) * attenuation;\n"
                        "        vec3 v = normalize(-position);\n"
                        "        vec3 h = normalize(toLightNormalized + v);\n"
                        "        gl_FragColor = vec4(vLightColor.xyz * nDotL * vLightColor.w, nDotL * pow(dot(normal, h), encodedNormal.w));\n"
                        "}\n";

                // spot light shaders
                static const char vertexShaderSpotLightAccumulation[] =
                        "attribute vec4 vertexPosition;\n"
                        "uniform mat4 viewProjectionMatrix;\n"
                        "uniform mat4 normalsMatrix;\n"
                        "uniform mat4 viewMatrix;\n"
                        "uniform vec4 lightColors[30];\n"
                        "uniform vec4 lightPositions[30];\n"
                        "uniform vec4 lightDirections[30];\n"
                        "varying vec4 vTextureCoordinates;\n"
                        "varying vec4 vLightColor;\n"
                        "varying vec4 vLightPosition;\n"
                        "varying vec4 vLightDirection;\n"
                        "void main()\n"
                        "{\n"
                        "        int lightIndex = int(vertexPosition.w);\n"
                        "        vLightColor = lightColors[lightIndex];\n"
                        "        vec4 lightPosition = lightPositions[lightIndex];\n"
                        "        vec4 lightDirection = lightDirections[lightIndex];\n"
                        "        vec3 axisZ = normalize(lightDirection.xyz);\n"
                        "        vec3 axisX, axisY;\n"
                        "        if(abs(axisZ.x) < 0.001 && abs(axisZ.y) < 0.001)\n"
                        "                axisX = cross(vec3(0.0, 1.0, 0.0), axisZ);\n"
                        "        else\n"
                        "                axisX = cross(vec3(0.0, 0.0, 1.0), axisZ);\n"
                        "        axisY = cross(axisZ, axisX);\n"
                        "        float factor = (1.0 - lightDirection.w);\n"
                        "        factor = factor * factor;\n"
                        "        factor = 1.0 / factor;\n"
                        "        vLightDirection = vec4((mat3(normalsMatrix) * -axisZ), factor);\n"
                        "        mat3 conversionMatrix = mat3(axisX, axisY, axisZ);\n"
                        "        vec3 position = vec3(vertexPosition.xy, 0.0);\n"
                        "        vec3 newPosition = position * conversionMatrix;\n"
                        "        position = lightPosition.xyz + lightDirection.xyz * vertexPosition.z + newPosition * lightPosition.w * 1.268;\n"
                        "        gl_Position = vTextureCoordinates = viewProjectionMatrix * vec4(position, 1.0);\n"
                        "        vLightPosition = viewMatrix * vec4(lightPosition.xyz, 1.0);\n"
                        "        vLightPosition.w = 1.0 / dot(lightDirection.xyz, lightDirection.xyz);\n"
                        "}\n";

                static const char fragmentShaderSpotLightAccumulation[] =
                        "uniform sampler2D depthBuffer;\n"
                        "uniform sampler2D normalsBuffer;\n"
                        "uniform vec4 textureCoordinatesAdjustment;\n"
                        "uniform vec4 conversionParameters;\n"
                        "uniform vec4 unprojectionVector;\n"
                        "varying vec4 vTextureCoordinates;\n"
                        "varying vec4 vLightColor;\n"
                        "varying vec4 vLightPosition;\n"
                        "varying vec4 vLightDirection;\n"
                        "void main()\n"
                        "{\n"
                        "        vec3 postProjectionCoordinates = vTextureCoordinates.xyz / vTextureCoordinates.w;\n"
                        "        vec2 textureCoordinates = 0.5 * (postProjectionCoordinates.xy + vec2(1.0, 1.0));\n"
                        "        textureCoordinates *= textureCoordinatesAdjustment.xy;\n"
                        "        float depth = 2.0 * texture2D(depthBuffer, textureCoordinates).x - 1.0;"
                        "        if(depth > postProjectionCoordinates.z)\n"
                        "                discard;\n"
                        "        vec4 encodedNormal = texture2D(normalsBuffer, textureCoordinates);\n"
                        "        encodedNormal.w *= 200.0;\n"
                        "        vec3 normal = normalize(decodeNormal(encodedNormal.xyz));\n"
                        "        vec3 position = decodePosition(postProjectionCoordinates.xy, depth,\n"
                        "                                       conversionParameters, unprojectionVector);\n"
                        "        vec3 toLight = vLightPosition.xyz - position;\n"
                        "        vec3 toLightNormalized = normalize(toLight);\n"
                        "        vec3 lightDirectionNormalized = normalize(vLightDirection.xyz);\n"
                        "        float factor = 1.0 - dot(toLightNormalized, lightDirectionNormalized);"
                        "        float attenuation = clamp(1.0 - factor * factor * vLightDirection.w, 0.0, 1.0);\n"
                        "        attenuation = min(attenuation, clamp(1.0 - dot(toLight, toLight) * vLightPosition.w, 0.0, 1.0));\n"
                        "        vec3 v = normalize(-position);\n"
                        "        vec3 h = normalize(toLightNormalized + v);\n"
                        "        float nDotL = dot(normal, toLightNormalized) * attenuation;\n"
                        "        gl_FragColor = vec4(vLightColor.xyz * nDotL * vLightColor.w, nDotL * pow(dot(normal, h), encodedNormal.w));\n"
                        "}\n";

                static const char fragmentShaderSpotLightAccumulationWithShadows[] =
                        "uniform sampler2D depthBuffer;\n"
                        "uniform sampler2D normalsBuffer;\n"
                        "uniform sampler2D shadowsBuffer;\n"
                        "uniform vec4 textureCoordinatesAdjustment;\n"
                        "uniform vec4 conversionParameters;\n"
                        "uniform vec4 unprojectionVector;\n"
                        "varying vec4 vTextureCoordinates;\n"
                        "varying vec4 vLightColor;\n"
                        "varying vec4 vLightPosition;\n"
                        "varying vec4 vLightDirection;\n"
                        "void main()\n"
                        "{\n"
                        "        vec3 postProjectionCoordinates = vTextureCoordinates.xyz / vTextureCoordinates.w;\n"
                        "        vec2 textureCoordinates = 0.5 * (postProjectionCoordinates.xy + vec2(1.0, 1.0));\n"
                        "        textureCoordinates *= textureCoordinatesAdjustment.xy;\n"
                        "        float depth = 2.0 * texture2D(depthBuffer, textureCoordinates).x - 1.0;"
                        "        if(depth > postProjectionCoordinates.z)\n"
                        "                discard;\n"
                        "        vec4 encodedNormal = texture2D(normalsBuffer, textureCoordinates);\n"
                        "        encodedNormal.w *= 200.0;\n"
                        "        vec3 normal = normalize(decodeNormal(encodedNormal.xyz));\n"
                        "        vec3 position = decodePosition(postProjectionCoordinates.xy, depth,\n"
                        "                                       conversionParameters, unprojectionVector);\n"
                        "        vec3 toLight = vLightPosition.xyz - position;\n"
                        "        vec3 toLightNormalized = normalize(toLight);\n"
                        "        vec3 lightDirectionNormalized = normalize(vLightDirection.xyz);\n"
                        "        float factor = 1.0 - dot(toLightNormalized, lightDirectionNormalized);"
                        "        float attenuation = clamp(1.0 - factor * factor * vLightDirection.w, 0.0, 1.0);\n"
                        "        attenuation = min(attenuation, clamp(1.0 - dot(toLight, toLight) * vLightPosition.w, 0.0, 1.0));\n"
                        "        vec3 v = normalize(-position);\n"
                        "        vec3 h = normalize(toLightNormalized + v);\n"
                        "        float nDotL = dot(normal, toLightNormalized) * attenuation * texture2D(shadowsBuffer, textureCoordinates).x;\n"
                        "        gl_FragColor = vec4(vLightColor.xyz * nDotL * vLightColor.w, nDotL * pow(dot(normal, h), encodedNormal.w));\n"
                        "}\n";

                static const char fragmentShaderSpotLightShadowsPass[] =
                        "uniform sampler2D depthBuffer;\n"
                        "uniform sampler2D shadowMap;\n"
                        "uniform vec4 shadowMapConversionParameters;\n"
                        "uniform vec4 textureCoordinatesAdjustment;\n"
                        "uniform vec4 conversionParameters;\n"
                        "uniform vec4 unprojectionVector;\n"
                        "uniform vec4 bias;\n"
                        "uniform mat4 lightTextureMatrix;\n"
                        "uniform mat4 lightViewMatrix;\n"
                        "varying vec4 vTextureCoordinates;\n"
                        "varying vec4 vLightColor;\n"
                        "varying vec4 vLightPosition;\n"
                        "varying vec4 vLightDirection;\n"
                        "float computeShadow(vec2 textureCoordinates, float eyeZ)\n"
                        "{\n"
                        "        float shadowMapEyeZ = convertDepthToEyeZ(2.0 * texture2D(shadowMap, textureCoordinates).x - 1.0,\n"
                        "                                                 shadowMapConversionParameters) + bias.x;\n"
                        "        return (eyeZ >= shadowMapEyeZ) ? 0.0 : 1.0;\n"
                        "}\n"
                        "void main()\n"
                        "{\n"
                        "        vec3 postProjectionCoordinates = vTextureCoordinates.xyz / vTextureCoordinates.w;\n"
                        "        vec2 textureCoordinates = 0.5 * (postProjectionCoordinates.xy + vec2(1.0, 1.0));\n"
                        "        textureCoordinates *= textureCoordinatesAdjustment.xy;\n"
                        "        float depth = 2.0 * texture2D(depthBuffer, textureCoordinates).x - 1.0;"
                        "        if(depth > postProjectionCoordinates.z)\n"
                        "                discard;\n"
                        "        vec3 position = decodePosition(postProjectionCoordinates.xy, depth,\n"
                        "                                       conversionParameters, unprojectionVector);\n"
                        "        vec4 lightTextureCoordinates = lightTextureMatrix * vec4(position, 1.0);\n"
                        "        lightTextureCoordinates.xyz /= lightTextureCoordinates.w;\n"
                        "        lightTextureCoordinates.xy = 0.5 * (lightTextureCoordinates.xy + vec2(1.0, 1.0));\n"
                        "        vec4 lightSpacePosition = lightViewMatrix * vec4(position, 1.0);\n"
                        "        lightSpacePosition.xyz /= lightSpacePosition.w;\n"
                        "        gl_FragColor = vec4(computeShadow(lightTextureCoordinates.xy, lightSpacePosition.z));"
                        "}\n";

                // load GLSL programs
                GlesGlslProgram::VertexAttribute vertexAttributes[] =
                {
                        GlesGlslProgram::VertexAttribute("vertexPosition", LOCATION_ATTRIBUTE_POSITION)
                };
                const uint8_t numVertexAttributes = sizeof(vertexAttributes) / sizeof(vertexAttributes[0]);

                static const char* vertexShaderSources[NUM_OF_GLSL_PROGRAMS] =
                {
                        vertexShaderDirectionalLightAccumulation,
                        vertexShaderPointLightAccumulation,
                        vertexShaderSpotLightAccumulation,
                        vertexShaderSpotLightAccumulation,
                        vertexShaderSpotLightAccumulation
                };

                static const char* fragmentShaderSources[NUM_OF_GLSL_PROGRAMS] =
                {
                        fragmentShaderDirectionalLightAccumulation,
                        fragmentShaderPointLightAccumulation,
                        fragmentShaderSpotLightAccumulation,
                        fragmentShaderSpotLightAccumulationWithShadows,
                        fragmentShaderSpotLightShadowsPass
                };

                for(uint8_t i = 0; i < NUM_OF_GLSL_PROGRAMS; ++i)
                {
                        if(!programs_[i].initialize(vertexShaderSources[i], fragmentShaderSources[i],
                                                    vertexAttributes, numVertexAttributes,
                                                    false, true))
                        {
                                destroy();
                                return false;
                        }

                        variables_[i].obtainLocations(programs_[i]);
                }

                return true;
        }

        //----------------------------------------------------------------------------------------------------------
        void GlesLightingRenderer::destroy()
        {
                for(uint8_t i = 0; i < NUM_OF_GLSL_PROGRAMS; ++i)
                        programs_[i].destroy();

                for(uint8_t i = 0; i < NUM_OF_LIGHT_TYPES; ++i)
                {
                        if(vertexBuffers_[i] != 0)
                        {
                                glDeleteBuffers(1, &vertexBuffers_[i]);
                                vertexBuffers_[i] = 0;
                        }
                }

                renderTargetContainer_ = nullptr;
                frameParameters_ = nullptr;
                actorsRenderer_ = nullptr;
                textureHandler_ = nullptr;
        }

        //----------------------------------------------------------------------------------------------------------
        void GlesLightingRenderer::renderLighting(Renderer::Data::LightNode& lightNode)
        {
                if(!renderTargetContainer_->setRenderTarget(RENDER_TARGET_LIGHT_BUFFER))
                {
                        LOGI("****************************** FAILED: GlesLightingRenderer::renderLighting: setRenderTarget(RENDER_TARGET_LIGHT_BUFFER)");
                        return;
                }

                glViewport(0, 0,
                           static_cast<GLsizei>(frameParameters_->screenSize.x),
                           static_cast<GLsizei>(frameParameters_->screenSize.y));

                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting: glClear");

                glDisable(GL_DEPTH_TEST);
                CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting: glDisable");

                glEnable(GL_CULL_FACE);
                CHECK_GLES_ERROR("GlesLightingRenderer::renderShadowMap: glEnable");

                glEnable(GL_BLEND);
                CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting: glEnable");

                glBlendFunc(GL_ONE, GL_ONE);
                CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting: glBlendFunc");

                renderLightingWithoutShadows(lightNode, Renderer::Data::UNIT_LIGHT_NO_SHADOWS_DIRECTIONAL);

                if(!IS_SET(frameParameters_->renderingFlags, RENDERING_SHADOWS_ENABLED))
                {
                        renderLightingWithoutShadows(lightNode, Renderer::Data::UNIT_LIGHT_DIRECTIONAL);
                        glDisable(GL_BLEND);
                        CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting: glDisable(GL_BLEND)");
                        return;
                }

                // render with shadows
                uint8_t programNo = GLSL_PROGRAM_SPOT_LIGHT_ACCUMULATION_WITH_SHADOWS;
                uint8_t lightUnit = Renderer::Data::UNIT_LIGHT_SPOT;

                static Vector4d position, direction, color;

                for(uint8_t lightType = LIGHT_SPOT; lightType <= LIGHT_SPOT; ++lightType, ++lightUnit, ++programNo)
                {
                        for(bool result = lightNode.readFirstElement(lightUnit); result;
                                 result = lightNode.readNextElement())
                        {
                                auto light = lightNode.getCurrentKey();
                                auto actorNode = lightNode.getCurrentData();

                                if(light == nullptr || actorNode == nullptr)
                                        break;

                                color.define(light->getColor(), light->getIntensity());

                                switch(light->getRenderingUnit())
                                {
                                        case Renderer::Data::UNIT_LIGHT_DIRECTIONAL:
                                        {
                                                DirectionalLight* directionalLight = static_cast<DirectionalLight*>(light);
                                                direction.define(directionalLight->getDirection(), directionalLight->getSize());
                                                break;
                                        }
                                        case Renderer::Data::UNIT_LIGHT_POINT:
                                        {
                                                PointLight* pointLight = static_cast<PointLight*>(light);
                                                position.define(pointLight->getPosition(), pointLight->getRadius());
                                                break;
                                        }

                                        case Renderer::Data::UNIT_LIGHT_SPOT:
                                        {
                                                SpotLight* spotLight = static_cast<SpotLight*>(light);

                                                position.define(spotLight->getPosition(),   spotLight->getRadius());
                                                direction.define(spotLight->getDirection(), spotLight->getCosTheta());

                                                renderShadowMap(*actorNode, *spotLight);
                                                break;
                                        }

                                        default:
                                                break;
                                }

                                const auto& variables = variables_[programNo];
                                programs_[programNo].set();
                                prepareLightAccumulation(variables);

                                glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers_[lightType]);
                                CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting: glBindBuffer");

                                glEnableVertexAttribArray(LOCATION_ATTRIBUTE_POSITION);
                                CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting: glEnableVertexAttribArray");

                                glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, lightVolumeGeometryVertexStride, nullptr);
                                CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting: glVertexAttribPointer");

                                glBindBuffer(GL_ARRAY_BUFFER, 0);
                                CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting: glBindBuffer");

                                renderLightGeometry(variables, lightType, 1, &color, &position, &direction);
                        }
                }

                glDisable(GL_BLEND);
                CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting: glDisable(GL_BLEND)");

                // unbind textures
                textureHandler_->setTexture(0, 0);
                textureHandler_->setTexture(0, 1);
                textureHandler_->setTexture(0, 2);
        }

        GlesLightingRenderer::Variables::Variables()
        {
                locationViewProjectionMatrix = -1;
                locationNormalsMatrix = -1;
                locationViewMatrix = -1;

                locationLightTextureMatrix = -1;
                locationLightViewMatrix    = -1;

                locationLightColors = -1;
                locationLightPositions = -1;
                locationLightDirections = -1;

                locationShadowMapConversionParameters = -1;
                locationTextureCoordinatesAdjustment = -1;
                locationConversionParameters = -1;
                locationUnprojectionVector = -1;
                locationBias = -1;

                locationDepthBuffer = -1;
                locationNormalsBuffer = -1;
                locationShadowsBuffer = -1;
                locationShadowMap = -1;
        }
        GlesLightingRenderer::Variables::~Variables() {}

        //------------------------------------------------------------------------------------------------------------
        void GlesLightingRenderer::Variables::obtainLocations(GlesGlslProgram& program)
        {
                locationViewProjectionMatrix = program.getUniformLocation("viewProjectionMatrix");
                locationNormalsMatrix        = program.getUniformLocation("normalsMatrix");
                locationViewMatrix           = program.getUniformLocation("viewMatrix");

                locationLightTextureMatrix = program.getUniformLocation("lightTextureMatrix");
                locationLightViewMatrix    = program.getUniformLocation("lightViewMatrix");

                locationLightColors     = program.getUniformLocation("lightColors");
                locationLightPositions  = program.getUniformLocation("lightPositions");
                locationLightDirections = program.getUniformLocation("lightDirections");

                locationShadowMapConversionParameters = program.getUniformLocation("shadowMapConversionParameters");
                locationTextureCoordinatesAdjustment  = program.getUniformLocation("textureCoordinatesAdjustment");
                locationConversionParameters          = program.getUniformLocation("conversionParameters");
                locationUnprojectionVector            = program.getUniformLocation("unprojectionVector");
                locationBias                          = program.getUniformLocation("bias");

                locationDepthBuffer   = program.getUniformLocation("depthBuffer");
                locationNormalsBuffer = program.getUniformLocation("normalsBuffer");
                locationShadowsBuffer = program.getUniformLocation("shadowsBuffer");
                locationShadowMap = program.getUniformLocation("shadowMap");
        }

        //----------------------------------------------------------------------------------------------------------
        void GlesLightingRenderer::renderLightingWithoutShadows(Renderer::Data::LightNode& lightNode,
                                                                uint8_t lightUnit)
        {
                // render without shadows
                uint8_t programNo = GLSL_PROGRAM_DIRECTIONAL_LIGHT_ACCUMULATION;

                static Vector4d colors[BATCH_SIZE];
                static Vector4d positions[BATCH_SIZE];
                static Vector4d directions[BATCH_SIZE];
                const GLenum cullFaceModes[] = {GL_FRONT, GL_FRONT, GL_BACK};

                for(uint8_t lightType = LIGHT_DIRECTIONAL; lightType <= LIGHT_SPOT; ++lightType, ++lightUnit, ++programNo)
                {
                        glCullFace(cullFaceModes[lightType]);
                        CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting: glCullFace");

                        const auto& variables = variables_[programNo];
                        programs_[programNo].set();
                        prepareLightAccumulation(variables);

                        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers_[lightType]);
                        CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting: glBindBuffer");

                        glEnableVertexAttribArray(LOCATION_ATTRIBUTE_POSITION);
                        CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting: glEnableVertexAttribArray");

                        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, lightVolumeGeometryVertexStride, nullptr);
                        CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting: glVertexAttribPointer");

                        glBindBuffer(GL_ARRAY_BUFFER, 0);
                        CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting: glBindBuffer");

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
                                        renderLightGeometry(variables, lightType, numLights, colors, positions, directions);
                                        numLights = 0;
                                }
                        }

                        if(numLights > 0)
                                renderLightGeometry(variables, lightType, numLights, colors, positions, directions);
                }

                // unbind everything
                glDisableVertexAttribArray(LOCATION_ATTRIBUTE_POSITION);
                CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting: glDisableVertexAttribArray");

                // unbind textures
                textureHandler_->setTexture(0, 0);
                textureHandler_->setTexture(0, 1);
                textureHandler_->setTexture(0, 2);
        }

        //----------------------------------------------------------------------------------------------------------
        void GlesLightingRenderer::prepareLightAccumulation(const GlesLightingRenderer::Variables& variables)
        {
                glUniform1i(variables.locationDepthBuffer, 0);
                glUniform1i(variables.locationNormalsBuffer, 1);
                glUniform1i(variables.locationShadowsBuffer, 2);
                CHECK_GLES_ERROR("GlesLightingRenderer::prepareLightAccumulation: glUniform1i");

                textureHandler_->setTexture(renderTargetContainer_->getDepthBuffer(DEPTH_BUFFER_NORMALS_PASS), 0);
                textureHandler_->setTexture(renderTargetContainer_->getRenderTarget(RENDER_TARGET_NORMALS),    1);
                textureHandler_->setTexture(renderTargetContainer_->getRenderTarget(RENDER_TARGET_HELPER_0),   2);

                glUniformMatrix4fv(variables.locationViewProjectionMatrix, 1, GL_FALSE,
                                   static_cast<const float*>(frameParameters_->viewProjectionMatrix));

                glUniformMatrix4fv(variables.locationNormalsMatrix, 1, GL_FALSE,
                                   static_cast<const float*>(frameParameters_->normalsMatrix));

                glUniformMatrix4fv(variables.locationViewMatrix, 1, GL_FALSE,
                                   static_cast<const float*>(frameParameters_->viewMatrix));
                CHECK_GLES_ERROR("GlesLightingRenderer::prepareLightAccumulation: glUniformMatrix4fv");

                glUniform4fv(variables.locationTextureCoordinatesAdjustment, 1,
                             static_cast<const float*>(frameParameters_->textureCoordinatesAdjustment));

                glUniform4fv(variables.locationConversionParameters, 1,
                             static_cast<const float*>(frameParameters_->conversionParameters));

                glUniform4fv(variables.locationUnprojectionVector, 1,
                             static_cast<const float*>(frameParameters_->unprojectionVector));
                CHECK_GLES_ERROR("GlesLightingRenderer::prepareLightAccumulation: glUniform4fv");
        }

        //----------------------------------------------------------------------------------------------------------
        void GlesLightingRenderer::prepareShadowsPass(const GlesLightingRenderer::Variables& variables)
        {
                glUniform1i(variables.locationDepthBuffer, 0);
                glUniform1i(variables.locationShadowMap, 1);
                CHECK_GLES_ERROR("GlesLightingRenderer::prepareShadowsPass: glUniform1i");

                textureHandler_->setTexture(renderTargetContainer_->getDepthBuffer(DEPTH_BUFFER_NORMALS_PASS), 0);
                textureHandler_->setTexture(renderTargetContainer_->getDepthBuffer(DEPTH_BUFFER_SHADOW_MAP), 1);

                glUniformMatrix4fv(variables.locationViewProjectionMatrix, 1, GL_FALSE,
                                   static_cast<const float*>(frameParameters_->viewProjectionMatrix));

                glUniformMatrix4fv(variables.locationNormalsMatrix, 1, GL_FALSE,
                                   static_cast<const float*>(frameParameters_->normalsMatrix));

                glUniformMatrix4fv(variables.locationViewMatrix, 1, GL_FALSE,
                                   static_cast<const float*>(frameParameters_->viewMatrix));
                CHECK_GLES_ERROR("GlesLightingRenderer::prepareShadowsPass: glUniformMatrix4fv");

                glUniform4fv(variables.locationTextureCoordinatesAdjustment, 1,
                             static_cast<const float*>(frameParameters_->textureCoordinatesAdjustment));

                glUniform4fv(variables.locationConversionParameters, 1,
                             static_cast<const float*>(frameParameters_->conversionParameters));

                glUniform4fv(variables.locationUnprojectionVector, 1,
                             static_cast<const float*>(frameParameters_->unprojectionVector));
                CHECK_GLES_ERROR("GlesLightingRenderer::prepareShadowsPass: glUniform4fv");
        }

        //----------------------------------------------------------------------------------------------------------
        void GlesLightingRenderer::renderLightGeometry(const GlesLightingRenderer::Variables& variables,
                                                       uint8_t type, uint32_t numLights,
                                                       Vector4d* colors,
                                                       Vector4d* positions,
                                                       Vector4d* directions)
        {
                glUniform4fv(variables.locationLightColors, numLights,
                             reinterpret_cast<const float*>(colors));
                switch(type)
                {
                        case LIGHT_DIRECTIONAL:
                                glUniform4fv(variables.locationLightDirections, numLights,
                                             reinterpret_cast<const float*>(directions));
                                break;

                        case LIGHT_POINT:
                                glUniform4fv(variables.locationLightPositions, numLights,
                                             reinterpret_cast<const float*>(positions));
                                break;

                        case LIGHT_SPOT:
                                glUniform4fv(variables.locationLightPositions, numLights,
                                             reinterpret_cast<const float*>(positions));
                                glUniform4fv(variables.locationLightDirections, numLights,
                                             reinterpret_cast<const float*>(directions));
                                break;

                        default:
                                return;
                }
                CHECK_GLES_ERROR("GlesLightingRenderer::renderLightGeometry: glUniform4fv");

                glDrawArrays(GL_TRIANGLE_STRIP, 0, lightVolumeGeometryNumVertices[type] * numLights);
                CHECK_GLES_ERROR("GlesFullScreenQuad::render: glDrawArrays");
        }

        //----------------------------------------------------------------------------------------------------------
        void GlesLightingRenderer::renderShadowMap(Renderer::Data::ActorNode& actorNode, const SpotLight& spotLight)
        {
                // render shadow map
                actorsRenderer_->renderShadowMap(actorNode);

                // render shadow
                if(!renderTargetContainer_->setRenderTarget(RENDER_TARGET_HELPER_0))
                {
                        LOGI("****************************** FAILED: GlesLightingRenderer::renderShadowMap: setRenderTarget(RENDER_TARGET_HELPER_0)");
                        return;
                }

                glViewport(0, 0,
                           static_cast<GLsizei>(frameParameters_->screenSize.x),
                           static_cast<GLsizei>(frameParameters_->screenSize.y));

                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                CHECK_GLES_ERROR("GlesLightingRenderer::renderShadowMap: glClear");

                glDisable(GL_DEPTH_TEST);
                CHECK_GLES_ERROR("GlesLightingRenderer::renderShadowMap: glDisable");

                glEnable(GL_CULL_FACE);
                CHECK_GLES_ERROR("GlesLightingRenderer::renderShadowMap: glEnable");

                glCullFace(GL_BACK);
                CHECK_GLES_ERROR("GlesLightingRenderer::renderShadowMap: glCullFace");

                glDisable(GL_BLEND);
                CHECK_GLES_ERROR("GlesLightingRenderer::renderShadowMap: glDisable");

                programs_[GLSL_PROGRAM_SPOT_LIGHT_SHADOWS_PASS].set();
                const auto& variables = variables_[GLSL_PROGRAM_SPOT_LIGHT_SHADOWS_PASS];
                prepareShadowsPass(variables);

                glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers_[LIGHT_SPOT]);
                CHECK_GLES_ERROR("GlesLightingRenderer::renderShadowMap: glBindBuffer");

                glEnableVertexAttribArray(LOCATION_ATTRIBUTE_POSITION);
                CHECK_GLES_ERROR("GlesLightingRenderer::renderShadowMap: glEnableVertexAttribArray");

                glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, lightVolumeGeometryVertexStride, nullptr);
                CHECK_GLES_ERROR("GlesLightingRenderer::renderShadowMap: glVertexAttribPointer");

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                CHECK_GLES_ERROR("GlesLightingRenderer::renderShadowMap: glBindBuffer");

                Matrix lightTextureMatrix, lightViewMatrix;

                lightTextureMatrix = frameParameters_->viewInvMatrix * spotLight.getViewProjectionMatrix();
                lightViewMatrix    = frameParameters_->viewInvMatrix * spotLight.getViewMatrix();

                glUniformMatrix4fv(variables.locationLightTextureMatrix, 1, GL_FALSE,
                                   static_cast<const float*>(lightTextureMatrix));

                glUniformMatrix4fv(variables.locationLightViewMatrix, 1, GL_FALSE,
                                   static_cast<const float*>(lightViewMatrix));
                CHECK_GLES_ERROR("GlesLightingRenderer::renderShadowMap: glUniformMatrix4fv");

                const auto& lightProjectionParameters = spotLight.getProjectionParameters();
                Vector4d shadowMapConversionParameters(lightProjectionParameters.w * lightProjectionParameters.z,
                                                       lightProjectionParameters.z - lightProjectionParameters.w,
                                                       lightProjectionParameters.w, 1.0f);
                Vector4d bias(lightProjectionParameters.w * 0.001f);

                glUniform4fv(variables.locationShadowMapConversionParameters, 1,
                             static_cast<const float*>(shadowMapConversionParameters));

                glUniform4fv(variables.locationBias, 1, static_cast<const float*>(bias));
                CHECK_GLES_ERROR("GlesLightingRenderer::renderShadowMap: glUniform4fv");

                Vector4d lightColor;
                Vector4d lightPosition(spotLight.getPosition(), spotLight.getRadius());
                Vector4d lightDirection(spotLight.getDirection(), spotLight.getCosTheta());

                renderLightGeometry(variables, LIGHT_SPOT, 1, &lightColor, &lightPosition, &lightDirection);

                // restore light buffer render target
                if(!renderTargetContainer_->setRenderTarget(RENDER_TARGET_LIGHT_BUFFER))
                {
                        LOGI("****************************** FAILED: GlesLightingRenderer::renderShadowMap: setRenderTarget(RENDER_TARGET_LIGHT_BUFFER)");
                        return;
                }

                glEnable(GL_BLEND);
                CHECK_GLES_ERROR("GlesLightingRenderer::renderShadowMap: glEnable");

                glBlendFunc(GL_ONE, GL_ONE);
                CHECK_GLES_ERROR("GlesLightingRenderer::renderShadowMap: glBlendFunc");
        }

}
