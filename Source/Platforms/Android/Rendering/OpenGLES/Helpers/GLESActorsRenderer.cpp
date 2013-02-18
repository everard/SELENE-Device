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
                capabilities_ = nullptr;
        }
        GlesActorsRenderer::~GlesActorsRenderer()
        {
                destroy();
        }

        //------------------------------------------------------------------------------------------------------------
        bool GlesActorsRenderer::initialize(GlesRenderTargetContainer& renderTargetContainer,
                                            GlesFrameParameters& frameParameters,
                                            GlesTextureHandler& textureHandler,
                                            GlesCapabilities& capabilities)
        {
                destroy();

                renderTargetContainer_ = &renderTargetContainer;
                frameParameters_ = &frameParameters;
                textureHandler_ = &textureHandler;
                capabilities_ = &capabilities;

                static const char vertexShader[] =
                        "attribute vec4 vertexPosition;"
                        "attribute vec4 vertexTextureCoordinates;"
                        "uniform mat4 worldViewProjectionMatrix;"
                        "varying vec4 vTextureCoordinates;"
                        "void main()"
                        "{"
                        "        gl_Position = worldViewProjectionMatrix * vertexPosition;"
                        "        vTextureCoordinates  = vertexTextureCoordinates;"
                        "}\n";

                static const char fragmentShader[] =
                        "precision mediump float;"
                        "varying vec4 vTextureCoordinates;"
                        "uniform vec4 ambientColor;"
                        "uniform sampler2D ambientMap;"
                        "void main()"
                        "{"
                        "        gl_FragColor = ambientColor * texture2D(ambientMap, vTextureCoordinates.xy);"
                        "}\n";

                static const char skinVertexShader[] =
                        "attribute vec4 vertexPosition;\n"
                        "attribute vec4 vertexTextureCoordinates;\n"
                        "attribute vec4 vertexBoneWeights;\n"
                        "attribute vec4 vertexBoneIndices;\n"
                        "uniform mat4 worldViewProjectionMatrix;\n"
                        "uniform vec4 bonePositions[50];\n"
                        "uniform vec4 boneRotations[50];\n"
                        "varying vec4 vTextureCoordinates;\n"
                        "void main()\n"
                        "{\n"
                        "        vec3 P = vec3(0.0, 0.0, 0.0);\n"
                        "        P += vertexBoneWeights.x * Transform(bonePositions[int(vertexBoneIndices.x)], boneRotations[int(vertexBoneIndices.x)], vertexPosition.xyz);\n"
                        "        P += vertexBoneWeights.y * Transform(bonePositions[int(vertexBoneIndices.y)], boneRotations[int(vertexBoneIndices.y)], vertexPosition.xyz);\n"
                        "        P += vertexBoneWeights.z * Transform(bonePositions[int(vertexBoneIndices.z)], boneRotations[int(vertexBoneIndices.z)], vertexPosition.xyz);\n"
                        "        P += vertexBoneWeights.w * Transform(bonePositions[int(vertexBoneIndices.w)], boneRotations[int(vertexBoneIndices.w)], vertexPosition.xyz);\n"
                        "        gl_Position = worldViewProjectionMatrix * vec4(P, 1.0);\n"
                        "        vTextureCoordinates  = vertexTextureCoordinates;\n"
                        "}\n";

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

                for(uint8_t i = 0; i < NUM_OF_RENDERING_PASSES; ++i)
                {
                        if(!programs_[2 * i].initialize(vertexShader, fragmentShader, vertexAttributes, numVertexAttributes))
                        {
                                destroy();
                                return false;
                        }

                        if(!programs_[2 * i + 1].initialize(skinVertexShader, fragmentShader, vertexAttributes, numVertexAttributes, true))
                        {
                                destroy();
                                return false;
                        }
                }

                for(uint8_t i = 0; i < NUM_OF_GLSL_PROGRAMS; ++i)
                        variables_[i].obtainLocations(programs_[i]);

                return true;
        }

        //------------------------------------------------------------------------------------------------------------
        void GlesActorsRenderer::destroy()
        {
                for(uint8_t i = 0; i < NUM_OF_GLSL_PROGRAMS; ++i)
                        programs_[i].destroy();

                renderTargetContainer_ = nullptr;
                frameParameters_ = nullptr;
                textureHandler_ = nullptr;
                capabilities_ = nullptr;
        }

        //------------------------------------------------------------------------------------------------------------
        void GlesActorsRenderer::renderPositionsAndNormals(Renderer::Data::ActorNode& actorNode)
        {
                /*d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

                LPDIRECT3DSURFACE9 renderTargets[] =
                {
                        renderTargetContainer_->getRenderTarget(RENDER_TARGET_POSITIONS).getSurface(),
                        renderTargetContainer_->getRenderTarget(RENDER_TARGET_NORMALS).getSurface()
                };
                DWORD    d3dClearFlags[]  = {D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCLEAR_TARGET};
                D3DCOLOR d3dClearColors[] = {D3DCOLOR_XRGB(0, 0, 0), D3DCOLOR_XRGB(128, 128, 128)};
                const uint8_t numRenderTargets = 2;

                DWORD d3dDepthWriteFlags[] = {TRUE, FALSE};
                DWORD d3dDepthFunctions[]  = {D3DCMP_LESSEQUAL, D3DCMP_EQUAL};
                uint8_t pixelShaderNo[]    = {PIXEL_SHADER_POSITIONS_PASS, PIXEL_SHADER_NORMALS_PASS};

                for(uint8_t pass = RENDERING_PASS_POSITIONS; pass <= RENDERING_PASS_NORMALS; ++pass)
                {
                        d3dDevice_->SetRenderState(D3DRS_ZWRITEENABLE, d3dDepthWriteFlags[pass]);
                        d3dDevice_->SetRenderState(D3DRS_ZFUNC,        d3dDepthFunctions[pass]);

                        d3dDevice_->SetRenderTarget(0, renderTargets[pass]);
                        d3dDevice_->Clear(0, nullptr, d3dClearFlags[pass], d3dClearColors[pass], 1.0f, 0);

                        textureHandler_->setStageState(0, D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_POINT);
                        textureHandler_->setStageState(1, D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_POINT);

                        d3dDevice_->SetPixelShaderConstantF(LOCATION_PROJECTION_PARAMETERS,
                                                            static_cast<const float*>(frameParameters_->projectionParameters), 1);

                        renderActors(actorNode, pass);
                }*/

                glCullFace(GL_FRONT);
                CHECK_GLES_ERROR("GlesActorsRenderer::renderPositionsAndNormals: glCullFace");

                renderTargetContainer_->getRenderTarget(RENDER_TARGET_POSITIONS).set();

                glViewport(0, 0,
                           static_cast<GLsizei>(frameParameters_->screenSize.x),
                           static_cast<GLsizei>(frameParameters_->screenSize.y));
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                CHECK_GLES_ERROR("GlesActorsRenderer::renderPositionsAndNormals: glClear");

                renderActors(actorNode, RENDERING_PASS_SHADING);
        }

        //------------------------------------------------------------------------------------------------------------
        void GlesActorsRenderer::renderShadowMap(Renderer::Data::ActorNode& actorNode,
                                                 const Vector4d& projectionParameters)
        {
                /*d3dDevice_->SetRenderTarget(0, renderTargetContainer_->getShadowMap().getSurface());
                d3dDevice_->SetDepthStencilSurface(renderTargetContainer_->getShadowMap().getDepthStencilSurface());

                d3dDevice_->Clear(0, nullptr, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET,
                                  D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);

                d3dDevice_->SetVertexDeclaration(d3dMeshVertexDeclaration_);
                d3dDevice_->SetStreamSource(1, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(2, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(3, nullptr, 0, 0);

                textureHandler_->setStageState(0, D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE);

                d3dDevice_->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
                d3dDevice_->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
                d3dDevice_->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

                pixelShaders_[PIXEL_SHADER_POSITIONS_PASS].set();
                d3dDevice_->SetPixelShaderConstantF(LOCATION_PROJECTION_PARAMETERS,
                                                    static_cast<const float*>(projectionParameters), 1);

                renderActors(actorNode, RENDERING_PASS_POSITIONS);*/
        }

        //------------------------------------------------------------------------------------------------------------
        void GlesActorsRenderer::renderShading(Renderer::Data::ActorNode& actorNode, bool isSsaoEnabled)
        {
                /*d3dDevice_->SetRenderTarget(0, renderTargetContainer_->getRenderTarget(RENDER_TARGET_RESULT).getSurface());
                d3dDevice_->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

                d3dDevice_->SetVertexDeclaration(d3dMeshVertexDeclaration_);
                d3dDevice_->SetStreamSource(1, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(2, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(3, nullptr, 0, 0);

                d3dDevice_->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
                d3dDevice_->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);

                d3dDevice_->SetSamplerState(LOCATION_AMBIENT_MAP_SHADING_PASS,
                                            D3DSAMP_MAXANISOTROPY, capabilities_->getMaxTextureAnisotropy());
                textureHandler_->setStageState(LOCATION_AMBIENT_MAP_SHADING_PASS,
                                               D3DTEXF_ANISOTROPIC, D3DTEXF_ANISOTROPIC, D3DTEXF_ANISOTROPIC);

                d3dDevice_->SetSamplerState(LOCATION_DIFFUSE_MAP_SHADING_PASS,
                                            D3DSAMP_MAXANISOTROPY, capabilities_->getMaxTextureAnisotropy());
                textureHandler_->setStageState(LOCATION_DIFFUSE_MAP_SHADING_PASS,
                                               D3DTEXF_ANISOTROPIC, D3DTEXF_ANISOTROPIC, D3DTEXF_ANISOTROPIC);

                d3dDevice_->SetSamplerState(LOCATION_SPECULAR_MAP_SHADING_PASS,
                                            D3DSAMP_MAXANISOTROPY, capabilities_->getMaxTextureAnisotropy());
                textureHandler_->setStageState(LOCATION_SPECULAR_MAP_SHADING_PASS,
                                               D3DTEXF_ANISOTROPIC, D3DTEXF_ANISOTROPIC, D3DTEXF_ANISOTROPIC);

                textureHandler_->setStageState(LOCATION_LIGHT_BUFFER_SHADING_PASS,
                                               D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_POINT,
                                               D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(LOCATION_LIGHT_BUFFER_SHADING_PASS,
                                       renderTargetContainer_->getRenderTarget(RENDER_TARGET_LIGHT_BUFFER).getTexture());

                if(isSsaoEnabled)
                {
                        textureHandler_->setStageState(LOCATION_SSAO_BUFFER_SHADING_PASS,
                                                       D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_POINT,
                                                       D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
                        d3dDevice_->SetTexture(LOCATION_SSAO_BUFFER_SHADING_PASS,
                                               renderTargetContainer_->getRenderTarget(RENDER_TARGET_HELPER_0).getTexture());

                        pixelShaders_[PIXEL_SHADER_SHADING_PASS_WITH_SSAO].set();
                }
                else
                        pixelShaders_[PIXEL_SHADER_SHADING_PASS].set();

                d3dDevice_->SetPixelShaderConstantF(LOCATION_TEXTURE_COORDINATES_ADJUSTMENT,
                                                    static_cast<const float*>(frameParameters_->textureCoordinatesAdjustment), 1);

                renderActors(actorNode, RENDERING_PASS_SHADING);*/
        }

        GlesActorsRenderer::Variables::Variables()
        {
                locationWorldViewProjectionMatrix = -1;
                locationWorldViewMatrix = -1;
                locationNormalsMatrix = -1;
                locationBoneRotations = -1;
                locationBonePositions = -1;

                locationProjectionParameters = -1;
                locationSpecularParameters = -1;

                locationAmbientMap = -1;
                locationDiffuseMap = -1;
                locationSpecularMap = -1;
                locationNormalMap = -1;

                locationLightBuffer = -1;
                locationSsaoBuffer = -1;

                locationAmbientColor = -1;
                locationDiffuseColor = -1;
                locationSpecularColor = -1;
        }
        GlesActorsRenderer::Variables::~Variables() {}

        //------------------------------------------------------------------------------------------------------------
        void GlesActorsRenderer::Variables::obtainLocations(GlesGlslProgram& program)
        {
                locationWorldViewProjectionMatrix = program.getUniformLocation("worldViewProjectionMatrix");
                locationWorldViewMatrix = program.getUniformLocation("worldViewMatrix");
                locationNormalsMatrix = program.getUniformLocation("normalsMatrix");
                locationBoneRotations = program.getUniformLocation("boneRotations");
                locationBonePositions = program.getUniformLocation("bonePositions");

                locationProjectionParameters = program.getUniformLocation("projectionParameters");
                locationSpecularParameters   = program.getUniformLocation("specularParameters");

                locationAmbientMap  = program.getUniformLocation("ambientMap");
                locationDiffuseMap  = program.getUniformLocation("diffuseMap");
                locationSpecularMap = program.getUniformLocation("specularMap");
                locationNormalMap   = program.getUniformLocation("normalMap");

                locationLightBuffer = program.getUniformLocation("lightBuffer");
                locationSsaoBuffer  = program.getUniformLocation("ssaoBuffer");

                locationAmbientColor  = program.getUniformLocation("ambientColor");
                locationDiffuseColor  = program.getUniformLocation("diffuseColor");
                locationSpecularColor = program.getUniformLocation("specularColor");
        }

        //------------------------------------------------------------------------------------------------------------
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
                                /*Vector4d ambientColor( material.getColor(MATERIAL_COLOR_AMBIENT),  1.0);
                                Vector4d diffuseColor( material.getColor(MATERIAL_COLOR_DIFFUSE),  1.0);
                                Vector4d specularColor(material.getColor(MATERIAL_COLOR_SPECULAR), 1.0);
                                Vector4d specularParameters(material.getSpecularLevel(), material.getGlossiness());

                                glUniform1i(variables.locationAmbientMap, 0);
                                textureHandler_->setTexture(material.getTextureMap(TEXTURE_MAP_AMBIENT), GL_TEXTURE0,
                                                            GlesTextureHandler::DUMMY_TEXTURE_WHITE);

                                glUniform1i(variables.locationDiffuseMap, 1);
                                textureHandler_->setTexture(material.getTextureMap(TEXTURE_MAP_DIFFUSE), GL_TEXTURE1,
                                                            GlesTextureHandler::DUMMY_TEXTURE_WHITE);

                                glUniform1i(variables.locationSpecularMap, 2);
                                textureHandler_->setTexture(material.getTextureMap(TEXTURE_MAP_SPECULAR), GL_TEXTURE2,
                                                            GlesTextureHandler::DUMMY_TEXTURE_WHITE);

                                glUniform4fv(variables.locationAmbientColor, 1, static_cast<const float*>(ambientColor));
                                glUniform4fv(variables.locationDiffuseColor, 1, static_cast<const float*>(diffuseColor));
                                glUniform4fv(variables.locationSpecularColor, 1, static_cast<const float*>(specularColor));
                                glUniform4fv(variables.locationSpecularParameters, 1, static_cast<const float*>(specularParameters));
                                CHECK_GLES_ERROR("GlesActorsRenderer::setMaterial: glUniform4fv");*/

                                Vector4d ambientColor( material.getColor(MATERIAL_COLOR_AMBIENT), 1.0);
                                glUniform4fv(variables.locationAmbientColor, 1, static_cast<const float*>(ambientColor));

                                glUniform1i(variables.locationAmbientMap, 0);
                                CHECK_GLES_ERROR("GlesActorsRenderer::setMaterial: glUniform1i");

                                textureHandler_->setTexture(material.getTextureMap(TEXTURE_MAP_AMBIENT), 0,
                                                            GlesTextureHandler::DUMMY_TEXTURE_WHITE);
                                break;
                        }
                }
        }

        //------------------------------------------------------------------------------------------------------------
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

        //------------------------------------------------------------------------------------------------------------
        void GlesActorsRenderer::renderActors(Renderer::Data::ActorNode& actorNode, uint8_t pass)
        {
                static const bool cullFaces[] = {true, false};
                GlesGlslProgram* programs = nullptr;

                static const uint8_t vertexBufferObjectIndices[NUM_OF_RENDERING_PASSES][MAX_NUM_OF_VERTEX_ATTRIBUTES_PER_PASS] =
                {
                        {Mesh::VERTEX_STREAM_POSITIONS, 0, 0, 0},
                        {
                                Mesh::VERTEX_STREAM_POSITIONS,
                                Mesh::VERTEX_STREAM_TBN_BASES,
                                Mesh::VERTEX_STREAM_TBN_BASES,
                                Mesh::VERTEX_STREAM_TEXTURE_COORDINATES
                        },
                        {
                                Mesh::VERTEX_STREAM_POSITIONS,
                                Mesh::VERTEX_STREAM_TEXTURE_COORDINATES, 0, 0
                        }
                };
                static const uint8_t vertexAttributeLocations[NUM_OF_RENDERING_PASSES][MAX_NUM_OF_VERTEX_ATTRIBUTES_PER_PASS] =
                {
                        {LOCATION_ATTRIBUTE_POSITION, 0, 0, 0},
                        {
                                LOCATION_ATTRIBUTE_POSITION,
                                LOCATION_ATTRIBUTE_NORMAL,
                                LOCATION_ATTRIBUTE_TANGENT,
                                LOCATION_ATTRIBUTE_TEXTURE_COORDINATES
                        },
                        {
                                LOCATION_ATTRIBUTE_POSITION,
                                LOCATION_ATTRIBUTE_TEXTURE_COORDINATES, 0, 0
                        }
                };
                static const uint8_t vertexAttributeSizes[NUM_OF_RENDERING_PASSES][MAX_NUM_OF_VERTEX_ATTRIBUTES_PER_PASS] =
                {
                        {3, 0, 0, 0},
                        {3, 3, 4, 2},
                        {3, 2, 0, 0}
                };
                static const uint8_t numVertexAttributes[NUM_OF_RENDERING_PASSES] = {1, 4, 2};

                switch(pass)
                {
                        case RENDERING_PASS_POSITIONS:
                                programs = &programs_[GLSL_PROGRAM_POSITIONS_PASS];
                                break;

                        case RENDERING_PASS_NORMALS:
                                programs = &programs_[GLSL_PROGRAM_NORMALS_PASS];
                                break;

                        case RENDERING_PASS_SHADING:
                                programs = &programs_[GLSL_PROGRAM_SHADING_PASS];
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

        //------------------------------------------------------------------------------------------------------------
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

                        switch(pass)
                        {
                                case RENDERING_PASS_POSITIONS:
                                        glUniformMatrix4fv(variables.locationWorldViewMatrix, 1, GL_FALSE,
                                                           static_cast<const float*>(transform.getWorldViewMatrix()));
                                        break;

                                case RENDERING_PASS_NORMALS:
                                        glUniformMatrix4fv(variables.locationNormalsMatrix, 1, GL_FALSE,
                                                           static_cast<const float*>(transform.getNormalsMatrix()));
                                        break;
                        }
                        CHECK_GLES_ERROR("GlesActorsRenderer::renderMeshSubsetInstances: glUniformMatrix4fv");

                        if(meshRenderingUnit == Renderer::Data::UNIT_MESH_SKIN)
                                setSkeletonPose((*it).getSkeletonInstance()->getFinalBoneTransforms(), variables);

                        glDrawElements(GL_TRIANGLES, 3 * meshSubset.numFaces, GL_UNSIGNED_SHORT, reinterpret_cast<uint8_t*>(3 * meshSubset.faceIndex * sizeof(uint16_t)));
                        CHECK_GLES_ERROR("GlesActorsRenderer::renderMeshSubsetInstances: glDrawElements");
                }
        }

}
