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

                // load GLSL programs
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
                        "        vec2 xAndY = vTextureCoordinates.xy / vTextureCoordinates.w;\n"
                        "        vec2 textureCoordinates = 0.5 * (xAndY + vec2(1.0, 1.0));\n"
                        "        textureCoordinates *= textureCoordinatesAdjustment.xy;\n"
                        "        vec4 encodedNormal = texture2D(normalsBuffer, textureCoordinates);\n"
                        "        encodedNormal.w *= 200.0;\n"
                        "        vec3 normal = normalize(decodeNormal(encodedNormal.xyz));\n"
                        "        vec3 position = decodePosition(xAndY, 2.0 * texture2D(depthBuffer, textureCoordinates).x - 1.0,\n"
                        "                                       conversionParameters, unprojectionVector);\n"
                        "        vec3 toLight = vLightPosition.xyz - position;\n"
                        "        vec3 toLightNormalized = normalize(toLight);\n"
                        "        float attenuation = clamp(1.0 - dot(toLight, toLight) * vLightPosition.w, 0.0, 1.0);\n"
                        "        vec3 v = normalize(-position);\n"
                        "        vec3 h = normalize(toLightNormalized + v);\n"
                        "        float nDotL = dot(normal, toLightNormalized) * attenuation;\n"
                        "        gl_FragColor = vec4(vLightColor.xyz * nDotL * vLightColor.w, nDotL * pow(dot(normal, h), encodedNormal.w));\n"
                        "}\n";

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
                        "        vec2 xAndY = vTextureCoordinates.xy / vTextureCoordinates.w;\n"
                        "        vec2 textureCoordinates = 0.5 * (xAndY + vec2(1.0, 1.0));\n"
                        "        textureCoordinates *= textureCoordinatesAdjustment.xy;\n"
                        "        vec4 encodedNormal = texture2D(normalsBuffer, textureCoordinates);\n"
                        "        encodedNormal.w *= 200.0;\n"
                        "        vec3 normal = normalize(decodeNormal(encodedNormal.xyz));\n"
                        "        vec3 position = decodePosition(xAndY, 2.0 * texture2D(depthBuffer, textureCoordinates).x - 1.0,\n"
                        "                                       conversionParameters, unprojectionVector);\n"
                        "        vec3 toLight = vLightPosition.xyz - position;\n"
                        "        vec3 toLightNormalized = normalize(toLight);\n"
                        "        vec3 lightDirectionNormalized = normalize(vLightDirection.xyz);\n"
                        "        float factor = 1.0 - dot(toLightNormalized, lightDirectionNormalized);"
                        "        float attenuation = clamp(1.0 - factor * factor * vLightDirection.w, 0.0, 1.0);\n"
                        "        attenuation = min(attenuation, clamp(1.0 - dot(toLight, toLight) * vLightPosition.w, 0.0, 1.0));\n"
                        "        clamp(1.0 - dot(toLight, toLight) * vLightPosition.w, 0.0, 1.0);\n"
                        "        vec3 v = normalize(-position);\n"
                        "        vec3 h = normalize(toLightNormalized + v);\n"
                        "        float nDotL = dot(normal, toLightNormalized) * attenuation;\n"
                        "        gl_FragColor = vec4(vLightColor.xyz * nDotL * vLightColor.w, nDotL * pow(dot(normal, h), encodedNormal.w));\n"
                        "}\n";

                GlesGlslProgram::VertexAttribute vertexAttributes[] =
                {
                        GlesGlslProgram::VertexAttribute("vertexPosition", LOCATION_ATTRIBUTE_POSITION)
                };
                const uint8_t numVertexAttributes = sizeof(vertexAttributes) / sizeof(vertexAttributes[0]);

                static const char* vertexShaderSources[NUM_OF_GLSL_PROGRAMS] =
                {
                        vertexShaderPointLightAccumulation,
                        vertexShaderPointLightAccumulation,
                        vertexShaderSpotLightAccumulation
                };

                static const char* fragmentShaderSources[NUM_OF_GLSL_PROGRAMS] =
                {
                        fragmentShaderPointLightAccumulation,
                        fragmentShaderPointLightAccumulation,
                        fragmentShaderSpotLightAccumulation
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
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting: glClear");

                glDisable(GL_DEPTH_TEST);
                CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting: glDisable");

                glCullFace(GL_FRONT);
                CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting: glCullFace");

                glEnable(GL_BLEND);
                CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting: glEnable");

                glBlendFunc(GL_ONE, GL_ONE);
                CHECK_GLES_ERROR("GlesGuiRenderer::renderGui: glBlendFunc");

                // render without shadows
                uint8_t programNo = GLSL_PROGRAM_POINT_LIGHT_ACCUMULATION;
                uint8_t lightUnit = Renderer::Data::UNIT_LIGHT_NO_SHADOWS_POINT;

                static Vector4d colors[BATCH_SIZE];
                static Vector4d positions[BATCH_SIZE];
                static Vector4d directions[BATCH_SIZE];

                for(uint8_t lightType = LIGHT_POINT; lightType <= LIGHT_SPOT; ++lightType, ++lightUnit, ++programNo)
                {
                        const auto& variables = variables_[programNo];
                        programs_[programNo].set();
                        prepareLightAccumulation(variables);

                        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers_[lightType]);
                        CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting glBindBuffer");

                        glEnableVertexAttribArray(LOCATION_ATTRIBUTE_POSITION);
                        CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting glEnableVertexAttribArray");

                        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, lightVolumeGeometryVertexStride, nullptr);
                        CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting glVertexAttribPointer");

                        glBindBuffer(GL_ARRAY_BUFFER, 0);
                        CHECK_GLES_ERROR("GlesLightingRenderer::renderLighting glBindBuffer");

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
                                        {
                                                DirectionalLight* directionalLight = static_cast<DirectionalLight*>(light);
                                                directions[numLights].define(directionalLight->getDirection(), directionalLight->getSize());
                                                break;
                                        }
                                        case Renderer::Data::UNIT_LIGHT_NO_SHADOWS_POINT:
                                        {
                                                PointLight* pointLight = static_cast<PointLight*>(light);
                                                positions[numLights].define(pointLight->getPosition(), pointLight->getRadius());
                                                break;
                                        }

                                        case Renderer::Data::UNIT_LIGHT_NO_SHADOWS_SPOT:
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

                // unbind textures
                textureHandler_->setTexture(0, 0);
                textureHandler_->setTexture(0, 1);

                glDisable(GL_BLEND);
                CHECK_GLES_ERROR("GlesGuiRenderer::renderGui: glBlendFunc");

                // render with shadows
                /*for(bool result = lightNode.readFirstElement(Renderer::Data::UNIT_LIGHT_SPOT); result;
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

                        positions[0].define(spotLight->getPosition(), spotLight->getRadius());
                        directions[0].define(spotLight->getDirection(), spotLight->getCosTheta());
                        colors[0].define(spotLight->getColor(), spotLight->getIntensity());

                        renderLightGeometry(LIGHT_SPOT, 1, positions, directions, colors);
                }

                d3dDevice_->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);*/
        }

        GlesLightingRenderer::Variables::Variables()
        {
                locationViewProjectionMatrix = -1;
                locationNormalsMatrix = -1;
                locationViewMatrix = -1;

                locationLightColors = -1;
                locationLightPositions = -1;
                locationLightDirections = -1;

                locationTextureCoordinatesAdjustment = -1;
                locationConversionParameters = -1;
                locationUnprojectionVector = -1;

                locationDepthBuffer = -1;
                locationNormalsBuffer = -1;
        }
        GlesLightingRenderer::Variables::~Variables() {}

        //------------------------------------------------------------------------------------------------------------
        void GlesLightingRenderer::Variables::obtainLocations(GlesGlslProgram& program)
        {
                locationViewProjectionMatrix = program.getUniformLocation("viewProjectionMatrix");
                locationNormalsMatrix        = program.getUniformLocation("normalsMatrix");
                locationViewMatrix           = program.getUniformLocation("viewMatrix");

                locationLightColors     = program.getUniformLocation("lightColors");
                locationLightPositions  = program.getUniformLocation("lightPositions");
                locationLightDirections = program.getUniformLocation("lightDirections");

                locationTextureCoordinatesAdjustment = program.getUniformLocation("textureCoordinatesAdjustment");
                locationConversionParameters         = program.getUniformLocation("conversionParameters");
                locationUnprojectionVector           = program.getUniformLocation("unprojectionVector");

                locationDepthBuffer   = program.getUniformLocation("depthBuffer");
                locationNormalsBuffer = program.getUniformLocation("normalsBuffer");

                /*LOGI("locationViewProjectionMatrix = %d", locationViewProjectionMatrix);
                LOGI("locationNormalsMatrix = %d", locationNormalsMatrix);
                LOGI("locationViewMatrix = %d", locationViewMatrix);

                LOGI("locationLightColors = %d", locationLightColors);
                LOGI("locationLightPositions = %d", locationLightPositions);
                LOGI("locationLightDirections = %d", locationLightDirections);

                LOGI("locationTextureCoordinatesAdjustment = %d", locationTextureCoordinatesAdjustment);
                LOGI("locationConversionParameters = %d", locationConversionParameters);
                LOGI("locationUnprojectionVector = %d", locationUnprojectionVector);

                LOGI("locationDepthBuffer = %d", locationDepthBuffer);
                LOGI("locationNormalsBuffer = %d", locationNormalsBuffer);*/
        }

        //----------------------------------------------------------------------------------------------------------
        void GlesLightingRenderer::prepareLightAccumulation(const GlesLightingRenderer::Variables& variables)
        {
                glUniform1i(variables.locationDepthBuffer, 0);
                glUniform1i(variables.locationNormalsBuffer, 1);
                CHECK_GLES_ERROR("GlesLightingRenderer::prepareLightAccumulation: glUniform1i");

                textureHandler_->setTexture(renderTargetContainer_->getDepthBuffer(DEPTH_BUFFER_NORMALS_PASS), 0);
                textureHandler_->setTexture(renderTargetContainer_->getRenderTarget(RENDER_TARGET_NORMALS), 1);

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
        /*void D3d9LightingRenderer::renderShadowMap(Renderer::Data::ActorNode& actorNode, const SpotLight& spotLight)
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
        }*/

}
