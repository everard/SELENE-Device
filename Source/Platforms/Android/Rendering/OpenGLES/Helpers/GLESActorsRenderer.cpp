// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLESActorsRenderer.h"

#include "GLESRenderTargetContainer.h"
#include "../Resources/GLESMesh.h"
#include "../../../Platform.h"
#include "../GLESRenderer.h"

namespace selene
{

        GlesActorsRenderer::GlesActorsRenderer()
        {
                renderTargetContainer_ = nullptr;
                frameParameters_ = nullptr;
                textureHandler_ = nullptr;
        }
        GlesActorsRenderer::~GlesActorsRenderer()
        {
                destroy();
        }

        //---------------------------------------------------------------------------------------------------------------------------------------------------------------
        bool GlesActorsRenderer::initialize(GlesRenderTargetContainer& renderTargetContainer,
                                            GlesFrameParameters& frameParameters,
                                            GlesTextureHandler& textureHandler)
        {
                destroy();

                renderTargetContainer_ = &renderTargetContainer;
                frameParameters_ = &frameParameters;
                textureHandler_ = &textureHandler;

                // normals pass shaders
                static const char vertexShaderNormalsPass[] =
                        "invariant gl_Position;\n"
                        "attribute vec4 vertexPosition;\n"
                        "attribute vec4 vertexNormal;\n"
                        "attribute vec4 vertexTangent;\n"
                        "attribute vec4 vertexTextureCoordinates;\n"
                        "uniform mat4 worldViewProjectionMatrix;\n"
                        "uniform mat4 normalsMatrix;\n"
                        "varying vec4 vTextureCoordinates;\n"
                        "varying vec3 vNormal;\n"
                        "varying vec3 vTangent;\n"
                        "varying vec3 vBitangent;\n"
                        "void main()\n"
                        "{\n"
                        "        gl_Position = worldViewProjectionMatrix * vertexPosition;\n"
                        "        vTextureCoordinates = vertexTextureCoordinates;\n"
                        "        vNormal  = mat3(normalsMatrix) * vertexNormal.xyz;\n"
                        "        vTangent = mat3(normalsMatrix) * vertexTangent.xyz;\n"
                        "        vBitangent = vertexTangent.w * cross(vTangent, vNormal);\n"
                        "}\n";

                static const char vertexShaderSkinNormalsPass[] =
                        "invariant gl_Position;\n"
                        "attribute vec4 vertexPosition;\n"
                        "attribute vec4 vertexNormal;\n"
                        "attribute vec4 vertexTangent;\n"
                        "attribute vec4 vertexTextureCoordinates;\n"
                        "attribute vec4 vertexBoneWeights;\n"
                        "attribute vec4 vertexBoneIndices;\n"
                        "uniform mat4 worldViewProjectionMatrix;\n"
                        "uniform mat4 normalsMatrix;\n"
                        "uniform vec4 bonePositions[50];\n"
                        "uniform vec4 boneRotations[50];\n"
                        "varying vec4 vTextureCoordinates;\n"
                        "varying vec3 vNormal;\n"
                        "varying vec3 vTangent;\n"
                        "varying vec3 vBitangent;\n"
                        "void main()\n"
                        "{\n"
                        "        vec3 position = vec3(0.0, 0.0, 0.0);\n"
                        "        ivec4 indices = ivec4(int(vertexBoneIndices.x), int(vertexBoneIndices.y), int(vertexBoneIndices.z), int(vertexBoneIndices.w));\n"
                        "        position += vertexBoneWeights.x * transformPosition(bonePositions[indices.x], boneRotations[indices.x], vertexPosition.xyz);\n"
                        "        position += vertexBoneWeights.y * transformPosition(bonePositions[indices.y], boneRotations[indices.y], vertexPosition.xyz);\n"
                        "        position += vertexBoneWeights.z * transformPosition(bonePositions[indices.z], boneRotations[indices.z], vertexPosition.xyz);\n"
                        "        position += vertexBoneWeights.w * transformPosition(bonePositions[indices.w], boneRotations[indices.w], vertexPosition.xyz);\n"
                        "        gl_Position = worldViewProjectionMatrix * vec4(position, 1.0);\n"
                        "        vec3 tangent  = vec3(0.0, 0.0, 0.0);\n"
                        "        vec3 normal   = vec3(0.0, 0.0, 0.0);\n"
                        "        tangent += vertexBoneWeights.x * rotateVector(vertexTangent.xyz, boneRotations[indices.x]).xyz;\n"
                        "        tangent += vertexBoneWeights.y * rotateVector(vertexTangent.xyz, boneRotations[indices.y]).xyz;\n"
                        "        tangent += vertexBoneWeights.z * rotateVector(vertexTangent.xyz, boneRotations[indices.z]).xyz;\n"
                        "        tangent += vertexBoneWeights.w * rotateVector(vertexTangent.xyz, boneRotations[indices.w]).xyz;\n"
                        "        normal += vertexBoneWeights.x * rotateVector(vertexNormal.xyz, boneRotations[indices.x]).xyz;\n"
                        "        normal += vertexBoneWeights.y * rotateVector(vertexNormal.xyz, boneRotations[indices.y]).xyz;\n"
                        "        normal += vertexBoneWeights.z * rotateVector(vertexNormal.xyz, boneRotations[indices.z]).xyz;\n"
                        "        normal += vertexBoneWeights.w * rotateVector(vertexNormal.xyz, boneRotations[indices.w]).xyz;\n"
                        "        vTextureCoordinates = vertexTextureCoordinates;\n"
                        "        vNormal  = mat3(normalsMatrix) * normal;\n"
                        "        vTangent = mat3(normalsMatrix) * tangent;\n"
                        "        vBitangent = vertexTangent.w * cross(vTangent, vNormal);\n"
                        "}\n";

                static const char fragmentShaderNormalsPass[] =
                        "uniform sampler2D normalMap;\n"
                        "uniform sampler2D specularMap;\n"
                        "uniform vec4 specularParameters;\n"
                        "varying vec4 vTextureCoordinates;\n"
                        "varying vec3 vNormal;\n"
                        "varying vec3 vTangent;\n"
                        "varying vec3 vBitangent;\n"
                        "void main()\n"
                        "{\n"
                        "        vec3 normal = 2.0 * texture2D(normalMap, vTextureCoordinates.xy).xyz - vec3(1.0, 1.0, 1.0);\n"
                        "        vec3 normalizedNormal = normalize(normal.x * vTangent + normal.y * vBitangent + normal.z * vNormal);\n"
                        "        gl_FragColor = vec4(encodeNormal(normalizedNormal), specularParameters.x * texture2D(specularMap, vTextureCoordinates.xy).a);\n"
                        "}\n";

                // shading pass shaders
                static const char vertexShaderShadingPass[] =
                        "invariant gl_Position;\n"
                        "attribute vec4 vertexPosition;\n"
                        "attribute vec4 vertexTextureCoordinates;\n"
                        "uniform mat4 worldViewProjectionMatrix;\n"
                        "varying vec4 vTextureCoordinates;\n"
                        "varying vec4 vPosition;\n"
                        "void main()\n"
                        "{\n"
                        "        gl_Position = vPosition = worldViewProjectionMatrix * vertexPosition;\n"
                        "        vTextureCoordinates = vertexTextureCoordinates;\n"
                        "}\n";

                static const char vertexShaderSkinShadingPass[] =
                        "invariant gl_Position;\n"
                        "attribute vec4 vertexPosition;\n"
                        "attribute vec4 vertexTextureCoordinates;\n"
                        "attribute vec4 vertexBoneWeights;\n"
                        "attribute vec4 vertexBoneIndices;\n"
                        "uniform mat4 worldViewProjectionMatrix;\n"
                        "uniform vec4 bonePositions[50];\n"
                        "uniform vec4 boneRotations[50];\n"
                        "varying vec4 vTextureCoordinates;\n"
                        "varying vec4 vPosition;\n"
                        "void main()\n"
                        "{\n"
                        "        vec3 position = vec3(0.0, 0.0, 0.0);\n"
                        "        ivec4 indices = ivec4(int(vertexBoneIndices.x), int(vertexBoneIndices.y), int(vertexBoneIndices.z), int(vertexBoneIndices.w));\n"
                        "        position += vertexBoneWeights.x * transformPosition(bonePositions[indices.x], boneRotations[indices.x], vertexPosition.xyz);\n"
                        "        position += vertexBoneWeights.y * transformPosition(bonePositions[indices.y], boneRotations[indices.y], vertexPosition.xyz);\n"
                        "        position += vertexBoneWeights.z * transformPosition(bonePositions[indices.z], boneRotations[indices.z], vertexPosition.xyz);\n"
                        "        position += vertexBoneWeights.w * transformPosition(bonePositions[indices.w], boneRotations[indices.w], vertexPosition.xyz);\n"
                        "        gl_Position = vPosition = worldViewProjectionMatrix * vec4(position, 1.0);\n"
                        "        vTextureCoordinates = vertexTextureCoordinates;\n"
                        "}\n";

                static const char fragmentShaderShadingPass[] =
                        "uniform sampler2D ambientMap;\n"
                        "uniform sampler2D diffuseMap;\n"
                        "uniform sampler2D specularMap;\n"
                        "uniform sampler2D lightBuffer;\n"
                        "uniform vec4 ambientColor;\n"
                        "uniform vec4 diffuseColor;\n"
                        "uniform vec4 specularColor;\n"
                        "uniform vec4 specularParameters;\n"
                        "uniform vec4 textureCoordinatesAdjustment;\n"
                        "varying vec4 vTextureCoordinates;\n"
                        "varying vec4 vPosition;\n"
                        "void main()\n"
                        "{\n"
                        "        vec2 textureCoordinatesForLightBuffer = vec2(0.5, 0.5) * (vPosition.xy / vPosition.w) + vec2(0.5, 0.5);\n"
                        "        textureCoordinatesForLightBuffer *= textureCoordinatesAdjustment.xy;\n"
                        "        vec4 lightBufferValue = texture2D(lightBuffer, textureCoordinatesForLightBuffer);\n"
                        "        vec4 ambient = ambientColor * texture2D(ambientMap, vTextureCoordinates.xy);\n"
                        "        vec4 diffuse = diffuseColor * texture2D(diffuseMap, vTextureCoordinates.xy);\n"
                        "        vec4 specular = specularColor * texture2D(specularMap, vTextureCoordinates.xy) * specularParameters.x * lightBufferValue.w;\n"
                        "        specular *= vec4(lightBufferValue.xyz, 1.0);\n"
                        "        gl_FragColor = ambient + diffuse * vec4(lightBufferValue.xyz, 1.0) + specular;\n"
                        "}\n";

                // shadow pass shaders
                static const char vertexShaderShadowsPass[] =
                        "invariant gl_Position;\n"
                        "attribute vec4 vertexPosition;\n"
                        "uniform mat4 worldViewProjectionMatrix;\n"
                        "void main()\n"
                        "{\n"
                        "        gl_Position = worldViewProjectionMatrix * vertexPosition;\n"
                        "}\n";

                static const char vertexShaderSkinShadowsPass[] =
                        "invariant gl_Position;\n"
                        "attribute vec4 vertexPosition;\n"
                        "attribute vec4 vertexBoneWeights;\n"
                        "attribute vec4 vertexBoneIndices;\n"
                        "uniform mat4 worldViewProjectionMatrix;\n"
                        "uniform vec4 bonePositions[50];\n"
                        "uniform vec4 boneRotations[50];\n"
                        "void main()\n"
                        "{\n"
                        "        vec3 position = vec3(0.0, 0.0, 0.0);\n"
                        "        ivec4 indices = ivec4(int(vertexBoneIndices.x), int(vertexBoneIndices.y), int(vertexBoneIndices.z), int(vertexBoneIndices.w));\n"
                        "        position += vertexBoneWeights.x * transformPosition(bonePositions[indices.x], boneRotations[indices.x], vertexPosition.xyz);\n"
                        "        position += vertexBoneWeights.y * transformPosition(bonePositions[indices.y], boneRotations[indices.y], vertexPosition.xyz);\n"
                        "        position += vertexBoneWeights.z * transformPosition(bonePositions[indices.z], boneRotations[indices.z], vertexPosition.xyz);\n"
                        "        position += vertexBoneWeights.w * transformPosition(bonePositions[indices.w], boneRotations[indices.w], vertexPosition.xyz);\n"
                        "        gl_Position = worldViewProjectionMatrix * vec4(position, 1.0);\n"
                        "}\n";

                static const char fragmentShaderShadowsPass[] =
                        "void main()\n"
                        "{\n"
                        "        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);\n"
                        "}\n";

                static const char* vertexShaderSources[NUM_OF_GLSL_PROGRAMS] =
                {
                        vertexShaderNormalsPass, vertexShaderSkinNormalsPass,
                        vertexShaderShadingPass, vertexShaderSkinShadingPass,
                        vertexShaderShadowsPass, vertexShaderSkinShadowsPass
                };

                static const char* fragmentShaderSources[NUM_OF_GLSL_PROGRAMS] =
                {
                        fragmentShaderNormalsPass, fragmentShaderNormalsPass,
                        fragmentShaderShadingPass, fragmentShaderShadingPass,
                        fragmentShaderShadowsPass, fragmentShaderShadowsPass
                };

                // load GLSL programs
                GlesGlslProgram::VertexAttribute vertexAttributes[] =
                {
                        GlesGlslProgram::VertexAttribute("vertexPosition", LOCATION_ATTRIBUTE_POSITION),
                        GlesGlslProgram::VertexAttribute("vertexNormal", LOCATION_ATTRIBUTE_NORMAL),
                        GlesGlslProgram::VertexAttribute("vertexTangent", LOCATION_ATTRIBUTE_TANGENT),
                        GlesGlslProgram::VertexAttribute("vertexTextureCoordinates", LOCATION_ATTRIBUTE_TEXTURE_COORDINATES),
                        GlesGlslProgram::VertexAttribute("vertexBoneIndices", LOCATION_ATTRIBUTE_BONE_INDICES),
                        GlesGlslProgram::VertexAttribute("vertexBoneWeights", LOCATION_ATTRIBUTE_BONE_WEIGHTS)
                };
                const uint8_t numVertexAttributes = sizeof(vertexAttributes) / sizeof(vertexAttributes[0]);

                for(uint8_t i = 0; i < NUM_OF_GLSL_PROGRAMS; ++i)
                {
                        if(!programs_[i].initialize(vertexShaderSources[i], fragmentShaderSources[i], vertexAttributes, numVertexAttributes, true, true))
                        {
                                destroy();
                                return false;
                        }
                }

                for(uint8_t i = 0; i < NUM_OF_GLSL_PROGRAMS; ++i)
                        variables_[i].obtainLocations(programs_[i]);

                return true;
        }

        //---------------------------------------------------------------------------------------------------------------------------------------------------------------
        void GlesActorsRenderer::destroy()
        {
                for(uint8_t i = 0; i < NUM_OF_GLSL_PROGRAMS; ++i)
                        programs_[i].destroy();

                renderTargetContainer_ = nullptr;
                frameParameters_ = nullptr;
                textureHandler_ = nullptr;
        }

        //---------------------------------------------------------------------------------------------------------------------------------------------------------------
        void GlesActorsRenderer::renderPositionsAndNormals(Renderer::Data::ActorNode& actorNode)
        {
                glEnable(GL_DEPTH_TEST);
                CHECK_GLES_ERROR("GlesActorsRenderer::renderPositionsAndNormals: glEnable");

                glCullFace(GL_FRONT);
                CHECK_GLES_ERROR("GlesActorsRenderer::renderPositionsAndNormals: glCullFace");

                glDepthMask(GL_TRUE);
                CHECK_GLES_ERROR("GlesActorsRenderer::renderPositionsAndNormals: glDepthMask");

                glDepthFunc(GL_LESS);
                CHECK_GLES_ERROR("GlesActorsRenderer::renderPositionsAndNormals: glDepthFunc");

                if(!renderTargetContainer_->setRenderTarget(RENDER_TARGET_NORMALS))
                {
                        LOGI("****************************** FAILED: GlesActorsRenderer::renderPositionsAndNormals: setRenderTarget(RENDER_TARGET_NORMALS)");
                        return;
                }

                glViewport(0, 0,
                           static_cast<GLsizei>(frameParameters_->screenSize.x),
                           static_cast<GLsizei>(frameParameters_->screenSize.y));

                glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                CHECK_GLES_ERROR("GlesActorsRenderer::renderPositionsAndNormals: glClear");

                renderActors(actorNode, RENDERING_PASS_NORMALS);
        }

        //---------------------------------------------------------------------------------------------------------------------------------------------------------------
        void GlesActorsRenderer::renderShadowMap(Renderer::Data::ActorNode& actorNode)
        {
                renderTargetContainer_->setShadowMap();

                glEnable(GL_DEPTH_TEST);
                CHECK_GLES_ERROR("GlesActorsRenderer::renderShadowMap: glEnable(GL_DEPTH_TEST)");

                glEnable(GL_CULL_FACE);
                CHECK_GLES_ERROR("GlesActorsRenderer::renderShadowMap: glEnable(GL_CULL_FACE)");

                glCullFace(GL_BACK);
                CHECK_GLES_ERROR("GlesActorsRenderer::renderShadowMap: glCullFace(GL_BACK)");

                glDepthMask(GL_TRUE);
                CHECK_GLES_ERROR("GlesActorsRenderer::renderShadowMap: glDepthMask(GL_TRUE)");

                glDepthFunc(GL_LESS);
                CHECK_GLES_ERROR("GlesActorsRenderer::renderShadowMap: glDepthFunc(GL_LESS)");

                glDisable(GL_BLEND);
                CHECK_GLES_ERROR("GlesActorsRenderer::renderShadowMap: glDisable(GL_BLEND)");

                glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
                CHECK_GLES_ERROR("GlesActorsRenderer::renderShadowMap: glColorMask");

                glViewport(0, 0,
                           static_cast<GLsizei>(frameParameters_->shadowMapSize.x),
                           static_cast<GLsizei>(frameParameters_->shadowMapSize.x));

                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                CHECK_GLES_ERROR("GlesActorsRenderer::renderShadowMap: glClear");

                renderActors(actorNode, RENDERING_PASS_SHADOWS);

                glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                CHECK_GLES_ERROR("GlesActorsRenderer::renderShadowMap: glColorMask");

                glCullFace(GL_FRONT);
                CHECK_GLES_ERROR("GlesActorsRenderer::renderShadowMap: glCullFace(GL_FRONT)");
        }

        //---------------------------------------------------------------------------------------------------------------------------------------------------------------
        void GlesActorsRenderer::renderShading(Renderer::Data::ActorNode& actorNode)
        {
                glEnable(GL_DEPTH_TEST);
                CHECK_GLES_ERROR("GlesActorsRenderer::renderShading: glEnable");

                glCullFace(GL_FRONT);
                CHECK_GLES_ERROR("GlesActorsRenderer::renderShading: glCullFace");

                glDepthMask(GL_TRUE);
                CHECK_GLES_ERROR("GlesActorsRenderer::renderShading: glDepthMask");

                glDepthFunc(GL_LESS);
                CHECK_GLES_ERROR("GlesActorsRenderer::renderShading: glDepthFunc");

                if(!renderTargetContainer_->setRenderTarget(RENDER_TARGET_RESULT))
                {
                        LOGI("****************************** FAILED: GlesActorsRenderer::renderShading: setRenderTarget(RENDER_TARGET_RESULT)");
                        return;
                }

                glViewport(0, 0,
                           static_cast<GLsizei>(frameParameters_->screenSize.x),
                           static_cast<GLsizei>(frameParameters_->screenSize.y));

                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                CHECK_GLES_ERROR("GlesActorsRenderer::renderShading: glClear");

                renderActors(actorNode, RENDERING_PASS_SHADING);
        }

        GlesActorsRenderer::Variables::Variables()
        {
                locationWorldViewProjectionMatrix = -1;
                locationNormalsMatrix = -1;
                locationBoneRotations = -1;
                locationBonePositions = -1;

                locationTextureCoordinatesAdjustment = -1;
                locationSpecularParameters = -1;

                locationAmbientMap = -1;
                locationDiffuseMap = -1;
                locationSpecularMap = -1;
                locationNormalMap = -1;

                locationLightBuffer = -1;

                locationAmbientColor = -1;
                locationDiffuseColor = -1;
                locationSpecularColor = -1;
        }
        GlesActorsRenderer::Variables::~Variables() {}

        //---------------------------------------------------------------------------------------------------------------------------------------------------------------
        void GlesActorsRenderer::Variables::obtainLocations(GlesGlslProgram& program)
        {
                locationWorldViewProjectionMatrix = program.getUniformLocation("worldViewProjectionMatrix");
                locationNormalsMatrix = program.getUniformLocation("normalsMatrix");
                locationBoneRotations = program.getUniformLocation("boneRotations");
                locationBonePositions = program.getUniformLocation("bonePositions");

                locationTextureCoordinatesAdjustment = program.getUniformLocation("textureCoordinatesAdjustment");
                locationSpecularParameters   = program.getUniformLocation("specularParameters");

                locationAmbientMap  = program.getUniformLocation("ambientMap");
                locationDiffuseMap  = program.getUniformLocation("diffuseMap");
                locationSpecularMap = program.getUniformLocation("specularMap");
                locationNormalMap   = program.getUniformLocation("normalMap");

                locationLightBuffer = program.getUniformLocation("lightBuffer");

                locationAmbientColor  = program.getUniformLocation("ambientColor");
                locationDiffuseColor  = program.getUniformLocation("diffuseColor");
                locationSpecularColor = program.getUniformLocation("specularColor");
        }

        //---------------------------------------------------------------------------------------------------------------------------------------------------------------
        void GlesActorsRenderer::setMaterial(const Material& material, uint8_t pass,
                                             const GlesActorsRenderer::Variables& variables)
        {
                switch(pass)
                {
                        case RENDERING_PASS_NORMALS:
                        {
                                Vector4d specularParameters(material.getGlossiness() * 0.005f);

                                glUniform1i(variables.locationNormalMap, 0);
                                CHECK_GLES_ERROR("GlesActorsRenderer::setMaterial: glUniform1i");
                                textureHandler_->setTexture(material.getTextureMap(TEXTURE_MAP_NORMAL), 0,
                                                            GlesTextureHandler::DUMMY_TEXTURE_NORMAL_MAP);

                                glUniform1i(variables.locationSpecularMap, 1);
                                CHECK_GLES_ERROR("GlesActorsRenderer::setMaterial: glUniform1i");
                                textureHandler_->setTexture(material.getTextureMap(TEXTURE_MAP_SPECULAR), 1,
                                                            GlesTextureHandler::DUMMY_TEXTURE_WHITE);

                                glUniform4fv(variables.locationSpecularParameters, 1, static_cast<const float*>(specularParameters));
                                CHECK_GLES_ERROR("GlesActorsRenderer::setMaterial: glUniform4fv");
                                break;
                        }

                        case RENDERING_PASS_SHADING:
                        {
                                Vector4d ambientColor( material.getColor(MATERIAL_COLOR_AMBIENT),  1.0);
                                Vector4d diffuseColor( material.getColor(MATERIAL_COLOR_DIFFUSE),  1.0);
                                Vector4d specularColor(material.getColor(MATERIAL_COLOR_SPECULAR), 1.0);
                                Vector4d specularParameters(material.getSpecularLevel(), material.getGlossiness());

                                glUniform1i(variables.locationAmbientMap, 0);
                                CHECK_GLES_ERROR("GlesActorsRenderer::setMaterial: glUniform1i");
                                textureHandler_->setTexture(material.getTextureMap(TEXTURE_MAP_AMBIENT), 0,
                                                            GlesTextureHandler::DUMMY_TEXTURE_WHITE);

                                glUniform1i(variables.locationDiffuseMap, 1);
                                CHECK_GLES_ERROR("GlesActorsRenderer::setMaterial: glUniform1i");
                                textureHandler_->setTexture(material.getTextureMap(TEXTURE_MAP_DIFFUSE), 1,
                                                            GlesTextureHandler::DUMMY_TEXTURE_WHITE);

                                glUniform1i(variables.locationSpecularMap, 2);
                                CHECK_GLES_ERROR("GlesActorsRenderer::setMaterial: glUniform1i");
                                textureHandler_->setTexture(material.getTextureMap(TEXTURE_MAP_SPECULAR), 2,
                                                            GlesTextureHandler::DUMMY_TEXTURE_WHITE);

                                glUniform4fv(variables.locationAmbientColor, 1, static_cast<const float*>(ambientColor));
                                glUniform4fv(variables.locationDiffuseColor, 1, static_cast<const float*>(diffuseColor));
                                glUniform4fv(variables.locationSpecularColor, 1, static_cast<const float*>(specularColor));
                                glUniform4fv(variables.locationSpecularParameters, 1, static_cast<const float*>(specularParameters));
                                CHECK_GLES_ERROR("GlesActorsRenderer::setMaterial: glUniform4fv");
                                break;
                        }
                }
        }

        //---------------------------------------------------------------------------------------------------------------------------------------------------------------
        void GlesActorsRenderer::setSkeletonPose(const Array<Skeleton::Transform, uint16_t>& boneTransforms,
                                                 const GlesActorsRenderer::Variables& variables)
        {
                if(boneTransforms.isEmpty())
                        return;

                static Quaternion rotations[MAX_NUM_OF_BONES_IN_MODEL];
                static Vector4d   positions[MAX_NUM_OF_BONES_IN_MODEL];

                uint16_t numBoneTransforms = (boneTransforms.getSize() > static_cast<uint16_t>(MAX_NUM_OF_BONES_IN_MODEL)) ?
                                             static_cast<uint16_t>(MAX_NUM_OF_BONES_IN_MODEL) : boneTransforms.getSize();

                for(uint16_t i = 0; i < numBoneTransforms; ++i)
                {
                        positions[i].define(boneTransforms[i].position, 0.0f);
                        rotations[i] = boneTransforms[i].rotation;
                }

                glUniform4fv(variables.locationBonePositions, numBoneTransforms, reinterpret_cast<const float*>(positions));
                glUniform4fv(variables.locationBoneRotations, numBoneTransforms, reinterpret_cast<const float*>(rotations));
                CHECK_GLES_ERROR("GlesActorsRenderer::setSkeletonPose: glUniform4fv");
        }

        //---------------------------------------------------------------------------------------------------------------------------------------------------------------
        void GlesActorsRenderer::renderActors(Renderer::Data::ActorNode& actorNode, uint8_t pass)
        {
                static const bool cullFaces[] = {true, false};
                GlesGlslProgram* programs = nullptr;

                static const uint8_t vertexBufferObjectIndices[NUM_OF_RENDERING_PASSES][MAX_NUM_OF_VERTEX_ATTRIBUTES_PER_PASS] =
                {
                        {
                                Mesh::VERTEX_STREAM_POSITIONS,
                                Mesh::VERTEX_STREAM_TBN_BASES,
                                Mesh::VERTEX_STREAM_TBN_BASES,
                                Mesh::VERTEX_STREAM_TEXTURE_COORDINATES
                        },
                        {
                                Mesh::VERTEX_STREAM_POSITIONS,
                                Mesh::VERTEX_STREAM_TEXTURE_COORDINATES, 0, 0
                        },
                        {
                                Mesh::VERTEX_STREAM_POSITIONS, 0, 0, 0
                        }
                };
                static const uint8_t vertexAttributeLocations[NUM_OF_RENDERING_PASSES][MAX_NUM_OF_VERTEX_ATTRIBUTES_PER_PASS] =
                {
                        {
                                LOCATION_ATTRIBUTE_POSITION,
                                LOCATION_ATTRIBUTE_NORMAL,
                                LOCATION_ATTRIBUTE_TANGENT,
                                LOCATION_ATTRIBUTE_TEXTURE_COORDINATES
                        },
                        {
                                LOCATION_ATTRIBUTE_POSITION,
                                LOCATION_ATTRIBUTE_TEXTURE_COORDINATES, 0, 0
                        },
                        {
                                LOCATION_ATTRIBUTE_POSITION, 0, 0, 0
                        }
                };
                static const uint8_t vertexAttributeSizes[NUM_OF_RENDERING_PASSES][MAX_NUM_OF_VERTEX_ATTRIBUTES_PER_PASS] =
                {
                        {3, 3, 4, 2},
                        {3, 2, 0, 0},
                        {3, 0, 0, 0}
                };
                static const uint8_t numVertexAttributes[NUM_OF_RENDERING_PASSES] = {4, 2, 1};

                switch(pass)
                {
                        case RENDERING_PASS_NORMALS:
                                programs = &programs_[GLSL_PROGRAM_NORMALS_PASS];
                                break;

                        case RENDERING_PASS_SHADING:
                                programs = &programs_[GLSL_PROGRAM_SHADING_PASS];
                                break;

                        case RENDERING_PASS_SHADOWS:
                                programs = &programs_[GLSL_PROGRAM_SHADOWS_PASS];
                                break;

                        default:
                                return;
                }

                // walk through all mesh units
                for(uint8_t meshUnit = 0; meshUnit < Renderer::Data::NUM_OF_MESH_UNITS; ++meshUnit)
                {
                        auto& materialNode = actorNode.getMaterialNode(meshUnit);
                        const auto& variables = variables_[Renderer::Data::NUM_OF_MESH_UNITS * pass + meshUnit];
                        programs[meshUnit].set();

                        if(pass == RENDERING_PASS_SHADING)
                        {
                                glUniform4fv(variables.locationTextureCoordinatesAdjustment, 1,
                                             static_cast<const float*>(frameParameters_->textureCoordinatesAdjustment));
                                CHECK_GLES_ERROR("GlesActorsRenderer::renderActors: glUniform4fv");

                                glUniform1i(variables.locationLightBuffer, 3);
                                CHECK_GLES_ERROR("GlesActorsRenderer::renderActors: glUniform1i");
                                textureHandler_->setTexture(renderTargetContainer_->getRenderTarget(RENDER_TARGET_LIGHT_BUFFER), 3);
                        }

                        // walk through all material units
                        for(uint8_t materialUnit = 0; materialUnit < Renderer::Data::NUM_OF_MATERIAL_UNITS; ++materialUnit)
                        {
                                if(cullFaces[materialUnit])
                                        glEnable(GL_CULL_FACE);
                                else
                                        glDisable(GL_CULL_FACE);

                                CHECK_GLES_ERROR("GlesActorsRenderer::renderActors: GL_CULL_FACE");

                                // walk through all materials
                                for(bool materialResult = materialNode.readFirstElement(materialUnit); materialResult;
                                         materialResult = materialNode.readNextElement())
                                {
                                        auto material = materialNode.getCurrentKey();
                                        if(material == nullptr)
                                                break;

                                        auto meshNode = materialNode.getCurrentData();
                                        if(meshNode == nullptr)
                                                break;

                                        setMaterial(*material, pass, variables);

                                        // walk through all meshes
                                        for(bool resultMesh = meshNode->readFirstElement(); resultMesh;
                                                 resultMesh = meshNode->readNextElement())
                                        {
                                                GlesMesh* glesMesh = static_cast<GlesMesh*>(meshNode->getCurrentKey());
                                                if(glesMesh == nullptr)
                                                        break;

                                                auto meshSubsetNode = meshNode->getCurrentData();
                                                if(meshSubsetNode == nullptr)
                                                        break;

                                                const auto& meshData = glesMesh->getData();

                                                for(uint8_t vertexAttributeNo = 0; vertexAttributeNo < numVertexAttributes[pass]; ++vertexAttributeNo)
                                                {
                                                        uint8_t vertexBufferObjectIndex = vertexBufferObjectIndices[pass][vertexAttributeNo];
                                                        uint8_t vertexAttributeLocation = vertexAttributeLocations[pass][vertexAttributeNo];

                                                        glBindBuffer(GL_ARRAY_BUFFER, glesMesh->vertexBuffers_[vertexBufferObjectIndex]);
                                                        CHECK_GLES_ERROR("GlesActorsRenderer::renderActors: glBindBuffer");

                                                        glEnableVertexAttribArray(vertexAttributeLocation);
                                                        CHECK_GLES_ERROR("GlesActorsRenderer::renderActors: glEnableVertexAttribArray");

                                                        glVertexAttribPointer(vertexAttributeLocation, vertexAttributeSizes[pass][vertexAttributeNo], GL_FLOAT, GL_FALSE,
                                                                              meshData.vertices[vertexBufferObjectIndex].getStride(),
                                                                              nullptr);
                                                        CHECK_GLES_ERROR("GlesActorsRenderer::renderActors: glVertexAttribPointer");
                                                }

                                                if(meshUnit == Renderer::Data::UNIT_MESH_SKIN)
                                                {
                                                        glBindBuffer(GL_ARRAY_BUFFER, glesMesh->vertexBuffers_[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS]);
                                                        CHECK_GLES_ERROR("GlesActorsRenderer::renderActors: glBindBuffer");

                                                        // set bone indices
                                                        glEnableVertexAttribArray(LOCATION_ATTRIBUTE_BONE_INDICES);
                                                        CHECK_GLES_ERROR("GlesActorsRenderer::renderActors: glEnableVertexAttribArray");

                                                        glVertexAttribPointer(LOCATION_ATTRIBUTE_BONE_INDICES, 4, GL_FLOAT, GL_FALSE,
                                                                              meshData.vertices[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS].getStride(),
                                                                              nullptr);
                                                        CHECK_GLES_ERROR("GlesActorsRenderer::renderActors: glVertexAttribPointer");

                                                        // set bone weights
                                                        glEnableVertexAttribArray(LOCATION_ATTRIBUTE_BONE_WEIGHTS);
                                                        CHECK_GLES_ERROR("GlesActorsRenderer::renderActors: glEnableVertexAttribArray");

                                                        glVertexAttribPointer(LOCATION_ATTRIBUTE_BONE_WEIGHTS, 4, GL_FLOAT, GL_FALSE,
                                                                              meshData.vertices[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS].getStride(),
                                                                              reinterpret_cast<uint8_t*>(sizeof(Vector4d)));
                                                        CHECK_GLES_ERROR("GlesActorsRenderer::renderActors: glVertexAttribPointer");
                                                }

                                                glBindBuffer(GL_ARRAY_BUFFER, 0);
                                                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glesMesh->indexBuffer_);
                                                CHECK_GLES_ERROR("GlesActorsRenderer::renderActors: glBindBuffer");

                                                // walk through all mesh subsets
                                                for(bool resultMeshSubset = meshSubsetNode->readFirstElement(); resultMeshSubset;
                                                         resultMeshSubset = meshSubsetNode->readNextElement())
                                                {
                                                        auto meshSubset = meshSubsetNode->getCurrentKey();
                                                        auto renderingList = meshSubsetNode->getCurrentData();

                                                        if(meshSubset == nullptr || renderingList == nullptr)
                                                                break;

                                                        renderMeshSubsetInstances(*renderingList, variables, *meshSubset, meshUnit, pass);
                                                }
                                        }
                                }
                        }

                        for(uint8_t vertexAttributeNo = 0; vertexAttributeNo < numVertexAttributes[pass]; ++vertexAttributeNo)
                                glDisableVertexAttribArray(vertexAttributeLocations[pass][vertexAttributeNo]);
                        glDisableVertexAttribArray(LOCATION_ATTRIBUTE_BONE_INDICES);
                        glDisableVertexAttribArray(LOCATION_ATTRIBUTE_BONE_WEIGHTS);
                        CHECK_GLES_ERROR("GlesActorsRenderer::renderActors: glDisableVertexAttribArray");
                }

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                CHECK_GLES_ERROR("GlesActorsRenderer::renderActors: glBindBuffer");
        }

        //---------------------------------------------------------------------------------------------------------------------------------------------------------------
        void GlesActorsRenderer::renderMeshSubsetInstances(const Renderer::Data::List<Actor::Instance>& renderingList,
                                                           const GlesActorsRenderer::Variables& variables,
                                                           const Mesh::Subset& meshSubset,
                                                           uint8_t meshRenderingUnit,
                                                           uint8_t pass)
        {
                const auto& instances = renderingList.getElements();
                for(auto it = instances.begin(); it != instances.end(); ++it)
                {
                        const auto& transform = (*it).getViewProjectionTransform();
                        glUniformMatrix4fv(variables.locationWorldViewProjectionMatrix, 1, GL_FALSE,
                                           static_cast<const float*>(transform.getWorldViewProjectionMatrix()));

                        if(pass == RENDERING_PASS_NORMALS)
                        {
                                glUniformMatrix4fv(variables.locationNormalsMatrix, 1, GL_FALSE,
                                                   static_cast<const float*>(transform.getNormalsMatrix()));
                                CHECK_GLES_ERROR("GlesActorsRenderer::renderMeshSubsetInstances: glUniformMatrix4fv");
                        }

                        if(meshRenderingUnit == Renderer::Data::UNIT_MESH_SKIN)
                                setSkeletonPose((*it).getSkeletonInstance()->getFinalBoneTransforms(), variables);

                        glDrawElements(GL_TRIANGLES, 3 * meshSubset.numFaces, GL_UNSIGNED_SHORT,
                                       reinterpret_cast<uint8_t*>(3 * meshSubset.faceIndex * sizeof(uint16_t)));
                        CHECK_GLES_ERROR("GlesActorsRenderer::renderMeshSubsetInstances: glDrawElements");
                }
        }

}
