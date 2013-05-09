// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "D3D9ActorsRenderer.h"

#include "../Resources/D3D9Mesh.h"
#include "../D3D9Renderer.h"

namespace selene
{

        D3d9ActorsRenderer::D3d9ActorsRenderer()
        {
                d3dMeshVertexDeclaration_ = nullptr;
                d3dDevice_ = nullptr;

                renderTargetContainer_ = nullptr;
                frameParameters_ = nullptr;
                textureHandler_ = nullptr;
                capabilities_ = nullptr;
        }
        D3d9ActorsRenderer::~D3d9ActorsRenderer()
        {
                destroy();
        }

        //-------------------------------------------------------------------------------------------------------------------------------------------------------
        bool D3d9ActorsRenderer::initialize(D3d9RenderTargetContainer& renderTargetContainer,
                                            D3d9FrameParameters& frameParameters,
                                            D3d9TextureHandler& textureHandler,
                                            D3d9Capabilities& capabilities)
        {
                destroy();

                renderTargetContainer_ = &renderTargetContainer;
                frameParameters_ = &frameParameters;
                textureHandler_ = &textureHandler;
                capabilities_ = &capabilities;

                d3dDevice_ = D3d9Renderer::getDevice();
                if(d3dDevice_ == nullptr)
                        return false;

                // load shaders
                D3d9Shader d3dVertexShaders[NUM_OF_VERTEX_SHADERS] =
                {
                        D3d9Shader("PositionPass.vsh",     "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, *capabilities_),
                        D3d9Shader("SkinPositionPass.vsh", "vs_2_0", 0, D3d9Shader::LIBRARY_VERTEX_SHADER, *capabilities_),
                        D3d9Shader("NormalsPass.vsh",      "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, *capabilities_),
                        D3d9Shader("SkinNormalsPass.vsh",  "vs_2_0", 0, D3d9Shader::LIBRARY_VERTEX_SHADER, *capabilities_),
                        D3d9Shader("ShadingPass.vsh",      "vs_1_1", 0, D3d9Shader::LIBRARY_EMPTY, *capabilities_),
                        D3d9Shader("SkinShadingPass.vsh",  "vs_2_0", 0, D3d9Shader::LIBRARY_VERTEX_SHADER, *capabilities_)
                };

                D3d9Shader d3dPixelShaders[NUM_OF_PIXEL_SHADERS] =
                {
                        D3d9Shader("PositionPass.psh",    "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_),
                        D3d9Shader("NormalsPass.psh",     "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_),
                        D3d9Shader("ShadingPass.psh",     "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_),
                        D3d9Shader("ShadingPassSSAO.psh", "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_)
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

                // load optional shaders
                if(capabilities_->isMultipleRenderTargetSupported())
                {
                        D3d9Shader d3dOptionalVertexShaders[NUM_OF_OPTIONAL_VERTEX_SHADERS] =
                        {
                                D3d9Shader("PositionNormalsPass.vsh",     "vs_1_1", 0, D3d9Shader::LIBRARY_VERTEX_SHADER, *capabilities_),
                                D3d9Shader("SkinPositionNormalsPass.vsh", "vs_2_0", 0, D3d9Shader::LIBRARY_VERTEX_SHADER, *capabilities_)
                        };

                        D3d9Shader d3dOptionalPixelShader("PositionNormalsPass.psh", "ps_2_0", 0, D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_);

                        for(uint32_t i = 0; i < NUM_OF_OPTIONAL_VERTEX_SHADERS; ++i)
                        {
                                if(!optionalVertexShaders_[i].create(d3dOptionalVertexShaders[i]))
                                {
                                        destroy();
                                        return false;
                                }
                        }

                        if(!optionalPixelShaders_[OPTIONAL_PIXEL_SHADER_POSITIONS_AND_NORMALS_PASS].create(d3dOptionalPixelShader))
                        {
                                destroy();
                                return false;
                        }
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
                        destroy();
                        return false;
                }

                return true;
        }

        //-------------------------------------------------------------------------------------------------------------------------------------------------------
        void D3d9ActorsRenderer::destroy()
        {
                for(uint8_t i = 0; i < NUM_OF_OPTIONAL_VERTEX_SHADERS; ++i)
                        optionalVertexShaders_[i].destroy();

                for(uint8_t i = 0; i < NUM_OF_OPTIONAL_PIXEL_SHADERS; ++i)
                        optionalPixelShaders_[i].destroy();

                for(uint8_t i = 0; i < NUM_OF_VERTEX_SHADERS; ++i)
                        vertexShaders_[i].destroy();

                for(uint8_t i = 0; i < NUM_OF_PIXEL_SHADERS; ++i)
                        pixelShaders_[i].destroy();

                SAFE_RELEASE(d3dMeshVertexDeclaration_);
                d3dDevice_ = nullptr;

                renderTargetContainer_ = nullptr;
                frameParameters_ = nullptr;
                textureHandler_ = nullptr;
                capabilities_ = nullptr;
        }

        //-------------------------------------------------------------------------------------------------------------------------------------------------------
        void D3d9ActorsRenderer::renderPositionsAndNormals(Renderer::Data::ActorNode& actorNode)
        {
                if(d3dDevice_ == nullptr)
                        return;

                d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                d3dDevice_->SetVertexDeclaration(d3dMeshVertexDeclaration_);
                d3dDevice_->SetStreamSource(1, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(2, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(3, nullptr, 0, 0);

                LPDIRECT3DSURFACE9 renderTargets[] =
                {
                        renderTargetContainer_->getRenderTarget(RENDER_TARGET_POSITIONS).getSurface(),
                        renderTargetContainer_->getRenderTarget(RENDER_TARGET_NORMALS).getSurface()
                };
                DWORD    d3dClearFlags[]  = {D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, D3DCLEAR_TARGET};
                D3DCOLOR d3dClearColors[] = {D3DCOLOR_XRGB(0, 0, 0), D3DCOLOR_XRGB(128, 128, 128)};
                const uint8_t numRenderTargets = 2;

                if(capabilities_->isMultipleRenderTargetSupported())
                {
                        d3dDevice_->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
                        d3dDevice_->SetRenderState(D3DRS_ZFUNC,        D3DCMP_LESSEQUAL);

                        for(uint8_t i = 0; i < numRenderTargets; ++i)
                        {
                                d3dDevice_->SetRenderTarget(0, renderTargets[i]);
                                d3dDevice_->Clear(0, nullptr, d3dClearFlags[i], d3dClearColors[i], 1.0f, 0);
                        }

                        for(uint8_t i = 0; i < numRenderTargets; ++i)
                                d3dDevice_->SetRenderTarget(i, renderTargets[i]);

                        textureHandler_->setStageState(0, D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_POINT);
                        textureHandler_->setStageState(1, D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_POINT);

                        optionalPixelShaders_[OPTIONAL_PIXEL_SHADER_POSITIONS_AND_NORMALS_PASS].set();
                        d3dDevice_->SetPixelShaderConstantF(LOCATION_PROJECTION_PARAMETERS,
                                                            static_cast<const float*>(frameParameters_->projectionParameters), 1);

                        renderActors(actorNode, RENDERING_PASS_POSITIONS_AND_NORMALS);
                        d3dDevice_->SetRenderTarget(1, nullptr);
                }
                else
                {
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

                                pixelShaders_[pixelShaderNo[pass]].set();
                                d3dDevice_->SetPixelShaderConstantF(LOCATION_PROJECTION_PARAMETERS,
                                                                    static_cast<const float*>(frameParameters_->projectionParameters), 1);

                                renderActors(actorNode, pass);
                        }
                }
        }

        //-------------------------------------------------------------------------------------------------------------------------------------------------------
        void D3d9ActorsRenderer::renderShadowMap(Renderer::Data::ActorNode& actorNode,
                                                 const Vector4d& projectionParameters)
        {
                if(d3dDevice_ == nullptr)
                        return;

                d3dDevice_->SetRenderTarget(0, renderTargetContainer_->getShadowMap().getSurface());
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

                renderActors(actorNode, RENDERING_PASS_POSITIONS);
        }

        //-------------------------------------------------------------------------------------------------------------------------------------------------------
        void D3d9ActorsRenderer::renderShading(Renderer::Data::ActorNode& actorNode, bool isSsaoEnabled)
        {
                if(d3dDevice_ == nullptr)
                        return;

                d3dDevice_->SetRenderTarget(0, renderTargetContainer_->getRenderTarget(RENDER_TARGET_RESULT).getSurface());
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

                renderActors(actorNode, RENDERING_PASS_SHADING);
        }

        //-------------------------------------------------------------------------------------------------------------------------------------------------------
        void D3d9ActorsRenderer::setMaterial(const Material& material, uint8_t pass)
        {
                switch(pass)
                {
                        case RENDERING_PASS_POSITIONS_AND_NORMALS:
                        case RENDERING_PASS_NORMALS:
                        {
                                Vector4d specularParameters(material.getGlossiness() * 0.005f);

                                textureHandler_->setTexture(material.getTextureMap(TEXTURE_MAP_NORMAL),
                                                            LOCATION_NORMAL_MAP_NORMALS_PASS,
                                                            D3d9TextureHandler::DUMMY_TEXTURE_NORMAL_MAP);
                                textureHandler_->setTexture(material.getTextureMap(TEXTURE_MAP_SPECULAR),
                                                            LOCATION_SPECULAR_MAP_NORMALS_PASS,
                                                            D3d9TextureHandler::DUMMY_TEXTURE_WHITE);

                                d3dDevice_->SetPixelShaderConstantF(LOCATION_SPECULAR_PARAMETERS_NORMALS_PASS,
                                                                    static_cast<const float*>(specularParameters), 1);
                                break;
                        }

                        case RENDERING_PASS_SHADING:
                        {
                                Vector4d ambientColor( material.getColor(MATERIAL_COLOR_AMBIENT),  1.0);
                                Vector4d diffuseColor( material.getColor(MATERIAL_COLOR_DIFFUSE),  1.0);
                                Vector4d specularColor(material.getColor(MATERIAL_COLOR_SPECULAR), 1.0);
                                Vector4d specularParameters(material.getSpecularLevel(), material.getGlossiness());

                                textureHandler_->setTexture(material.getTextureMap(TEXTURE_MAP_AMBIENT),
                                                            LOCATION_AMBIENT_MAP_SHADING_PASS,
                                                            D3d9TextureHandler::DUMMY_TEXTURE_WHITE);
                                textureHandler_->setTexture(material.getTextureMap(TEXTURE_MAP_DIFFUSE),
                                                            LOCATION_DIFFUSE_MAP_SHADING_PASS,
                                                            D3d9TextureHandler::DUMMY_TEXTURE_WHITE);
                                textureHandler_->setTexture(material.getTextureMap(TEXTURE_MAP_SPECULAR),
                                                            LOCATION_SPECULAR_MAP_SHADING_PASS,
                                                            D3d9TextureHandler::DUMMY_TEXTURE_WHITE);

                                d3dDevice_->SetPixelShaderConstantF(LOCATION_AMBIENT_COLOR_SHADING_PASS,
                                                                    static_cast<const float*>(ambientColor), 1);
                                d3dDevice_->SetPixelShaderConstantF(LOCATION_DIFFUSE_COLOR_SHADING_PASS,
                                                                    static_cast<const float*>(diffuseColor), 1);
                                d3dDevice_->SetPixelShaderConstantF(LOCATION_SPECULAR_COLOR_SHADING_PASS,
                                                                    static_cast<const float*>(specularColor), 1);
                                d3dDevice_->SetPixelShaderConstantF(LOCATION_SPECULAR_PARAMETERS_SHADING_PASS,
                                                                    static_cast<const float*>(specularParameters), 1);
                                break;
                        }
                }
        }

        //-------------------------------------------------------------------------------------------------------------------------------------------------------
        void D3d9ActorsRenderer::setSkeletonPose(const Array<Skeleton::Transform, uint16_t>& boneTransforms)
        {
                if(boneTransforms.isEmpty())
                        return;

                static Quaternion rotations[MAX_NUM_OF_BONES_IN_MODEL];
                static Vector4d   positions[MAX_NUM_OF_BONES_IN_MODEL];

                uint16_t numBoneTransforms = (boneTransforms.getSize() > static_cast<uint16_t>(MAX_NUM_OF_BONES_IN_MODEL)) ?
                                             static_cast<uint16_t>(MAX_NUM_OF_BONES_IN_MODEL) : boneTransforms.getSize();

                for(uint16_t i = 0; i < numBoneTransforms; ++i)
                {
                        rotations[i] = boneTransforms[i].rotation;
                        positions[i].define(boneTransforms[i].position, 0.0f);
                }

                d3dDevice_->SetVertexShaderConstantF(LOCATION_BONE_ROTATIONS, reinterpret_cast<const float*>(rotations), numBoneTransforms);
                d3dDevice_->SetVertexShaderConstantF(LOCATION_BONE_POSITIONS, reinterpret_cast<const float*>(positions), numBoneTransforms);
        }

        //-------------------------------------------------------------------------------------------------------------------------------------------------------
        void D3d9ActorsRenderer::renderActors(Renderer::Data::ActorNode& actorNode, uint8_t pass)
        {
                static const DWORD cullModes[] = {D3DCULL_CCW, D3DCULL_NONE};
                D3d9VertexShader* vertexShaders = nullptr;

                static const uint8_t vertexStreamIndices[NUM_OF_RENDERING_PASSES][MAX_VERTEX_STREAMS_PER_PASS] =
                {
                        {Mesh::VERTEX_STREAM_POSITIONS, 0, 0},
                        {
                                Mesh::VERTEX_STREAM_POSITIONS,
                                Mesh::VERTEX_STREAM_TBN_BASES,
                                Mesh::VERTEX_STREAM_TEXTURE_COORDINATES
                        },
                        {
                                Mesh::VERTEX_STREAM_POSITIONS,
                                Mesh::VERTEX_STREAM_TEXTURE_COORDINATES, 0
                        },
                        {
                                Mesh::VERTEX_STREAM_POSITIONS,
                                Mesh::VERTEX_STREAM_TBN_BASES,
                                Mesh::VERTEX_STREAM_TEXTURE_COORDINATES
                        }
                };
                static const uint8_t numVertexStreams[NUM_OF_RENDERING_PASSES] = {1, 3, 2, 3};

                switch(pass)
                {
                        case RENDERING_PASS_POSITIONS:
                                vertexShaders = &vertexShaders_[VERTEX_SHADER_POSITIONS_PASS];
                                break;

                        case RENDERING_PASS_NORMALS:
                                vertexShaders = &vertexShaders_[VERTEX_SHADER_NORMALS_PASS];
                                break;

                        case RENDERING_PASS_SHADING:
                                vertexShaders = &vertexShaders_[VERTEX_SHADER_SHADING_PASS];
                                break;

                        case RENDERING_PASS_POSITIONS_AND_NORMALS:
                                if(!capabilities_->isMultipleRenderTargetSupported())
                                        return;
                                vertexShaders = &optionalVertexShaders_[OPTIONAL_VERTEX_SHADER_POSITIONS_AND_NORMALS_PASS];
                                break;

                        default:
                                return;
                }

                // walk through all mesh units
                for(uint8_t meshUnit = 0; meshUnit < Renderer::Data::NUM_OF_MESH_UNITS; ++meshUnit)
                {
                        auto& materialNode = actorNode.getMaterialNode(meshUnit);
                        vertexShaders[meshUnit].set();

                        // walk through all material units
                        for(uint8_t materialUnit = 0; materialUnit < Renderer::Data::NUM_OF_MATERIAL_UNITS; ++materialUnit)
                        {
                                d3dDevice_->SetRenderState(D3DRS_CULLMODE, cullModes[materialUnit]);

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

                                        setMaterial(*material, pass);

                                        // walk through all meshes
                                        for(bool resultMesh = meshNode->readFirstElement(); resultMesh;
                                                 resultMesh = meshNode->readNextElement())
                                        {
                                                D3d9Mesh* d3dMesh = static_cast<D3d9Mesh*>(meshNode->getCurrentKey());
                                                if(d3dMesh == nullptr)
                                                        break;

                                                auto meshSubsetNode = meshNode->getCurrentData();
                                                if(meshSubsetNode == nullptr)
                                                        break;

                                                const auto& meshData = d3dMesh->getData();

                                                for(uint8_t vertexStream = 0; vertexStream < numVertexStreams[pass]; ++vertexStream)
                                                {
                                                        UINT streamNo = vertexStreamIndices[pass][vertexStream];
                                                        d3dDevice_->SetStreamSource(streamNo, d3dMesh->d3dVertexBuffers_[streamNo], 0,
                                                                                    meshData.vertices[streamNo].getStride());
                                                }

                                                if(meshUnit == Renderer::Data::UNIT_MESH_SKIN)
                                                        d3dDevice_->SetStreamSource(Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS,
                                                                                    d3dMesh->d3dVertexBuffers_[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS], 0,
                                                                                    meshData.vertices[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS].getStride());

                                                d3dDevice_->SetIndices(d3dMesh->d3dIndexBuffer_);

                                                // walk through all mesh subsets
                                                for(bool resultMeshSubset = meshSubsetNode->readFirstElement(); resultMeshSubset;
                                                         resultMeshSubset = meshSubsetNode->readNextElement())
                                                {
                                                        auto meshSubset = meshSubsetNode->getCurrentKey();
                                                        auto renderingList = meshSubsetNode->getCurrentData();

                                                        if(meshSubset == nullptr || renderingList == nullptr)
                                                                break;

                                                        renderMeshSubsetInstances(*renderingList, *meshSubset, meshUnit, pass);
                                                }
                                        }
                                }
                        }
                }
        }

        //-------------------------------------------------------------------------------------------------------------------------------------------------------
        void D3d9ActorsRenderer::renderMeshSubsetInstances(const Renderer::Data::List<Actor::Instance>& renderingList,
                                                           const Mesh::Subset& meshSubset,
                                                           uint8_t meshRenderingUnit,
                                                           uint8_t pass)
        {
                const auto& instances = renderingList.getElements();
                for(auto it = instances.begin(); it != instances.end(); ++it)
                {
                        const auto& transform = (*it).getViewProjectionTransform();
                        d3dDevice_->SetVertexShaderConstantF(LOCATION_WORLD_VIEW_PROJECTION_MATRIX,
                                                             static_cast<const float*>(transform.getWorldViewProjectionMatrix()),
                                                             4);

                        switch(pass)
                        {
                                case RENDERING_PASS_POSITIONS_AND_NORMALS:
                                        d3dDevice_->SetVertexShaderConstantF(LOCATION_WORLD_VIEW_MATRIX,
                                                                             static_cast<const float*>(transform.getWorldViewMatrix()),
                                                                             4);
                                        d3dDevice_->SetVertexShaderConstantF(LOCATION_NORMALS_MATRIX,
                                                                             static_cast<const float*>(transform.getNormalsMatrix()),
                                                                             4);
                                        break;

                                case RENDERING_PASS_POSITIONS:
                                        d3dDevice_->SetVertexShaderConstantF(LOCATION_WORLD_VIEW_MATRIX,
                                                                             static_cast<const float*>(transform.getWorldViewMatrix()),
                                                                             4);
                                        break;

                                case RENDERING_PASS_NORMALS:
                                        d3dDevice_->SetVertexShaderConstantF(LOCATION_NORMALS_MATRIX,
                                                                             static_cast<const float*>(transform.getNormalsMatrix()),
                                                                             4);
                                        break;
                        }

                        if(meshRenderingUnit == Renderer::Data::UNIT_MESH_SKIN)
                                setSkeletonPose((*it).getSkeletonInstance()->getFinalBoneTransforms());

                        d3dDevice_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, meshSubset.vertexIndex,
                                                         meshSubset.numVertices, 3 * meshSubset.faceIndex,
                                                         meshSubset.numFaces);
                }
        }

}
