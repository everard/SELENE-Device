// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "D3D9ParticlesRenderer.h"

#include "../../../../../Engine/Scene/Nodes/ParticleSystem.h"
#include "../../../../../Engine/Core/Helpers/Bag.h"
#include "../D3D9Renderer.h"

namespace selene
{

        static const uint32_t particlesGeometryNumVerticesPerParticle = 6;
        static const uint32_t particlesGeometryNumVectorsPerVertex = 3;
        static const uint32_t particlesGeometryBatchSize = 1024;

        static const uint32_t particlesGeometryVertexStride = particlesGeometryNumVectorsPerVertex * sizeof(Vector4d);
        static const uint32_t particlesGeometryNumVertices = particlesGeometryBatchSize *
                                                             particlesGeometryNumVerticesPerParticle;

        static const uint32_t particlesGeometryBufferSize = particlesGeometryNumVertices *
                                                            particlesGeometryVertexStride;
        static Vector4d particlesGeometryBuffer[particlesGeometryNumVertices * particlesGeometryNumVectorsPerVertex];

        D3d9ParticlesRenderer::D3d9ParticlesRenderer():
                d3dVertexDeclaration_(nullptr), d3dVertexBuffer_(nullptr), d3dDevice_(nullptr),
                renderTargetContainer_(nullptr), frameParameters_(nullptr),
                textureHandler_(nullptr), capabilities_(nullptr)
        {
                Vector4d vertices[] =
                {
                        Vector4d(), Vector4d(), Vector4d(0.0f, 0.0f, -1.0f, -1.0f),
                        Vector4d(), Vector4d(), Vector4d(1.0f, 0.0f,  1.0f, -1.0f),

                        Vector4d(), Vector4d(), Vector4d(0.0f, 1.0f, -1.0f,  1.0f),
                        Vector4d(), Vector4d(), Vector4d(0.0f, 1.0f, -1.0f,  1.0f),

                        Vector4d(), Vector4d(), Vector4d(1.0f, 0.0f,  1.0f, -1.0f),
                        Vector4d(), Vector4d(), Vector4d(1.0f, 1.0f,  1.0f,  1.0f)
                };

                Vector4d* particlesGeometry = particlesGeometryBuffer;
                for(uint32_t i = 0; i < particlesGeometryBatchSize; ++i)
                {
                        memcpy(particlesGeometry, vertices, sizeof(vertices));
                        particlesGeometry += particlesGeometryNumVerticesPerParticle *
                                             particlesGeometryNumVectorsPerVertex;
                }
        }
        D3d9ParticlesRenderer::~D3d9ParticlesRenderer()
        {
                destroy();
        }

        //------------------------------------------------------------------------------------------------------------
        bool D3d9ParticlesRenderer::initialize(D3d9RenderTargetContainer& renderTargetContainer,
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

                static const char* vertexShaderParticlesPass =
                        "float4x4 projectionMatrix: register(c0);"
                        "float4x4 viewMatrix:       register(c4);"
                        ""
                        "struct Input"
                        "{"
                        "        float4 position:      POSITION0;"
                        "        float4 particleColor: TEXCOORD0;"
                        "        float4 offsets:       TEXCOORD1;"
                        "};"
                        ""
                        "struct Output"
                        "{"
                        "        float4 position:           POSITION;"
                        "        float4 textureCoordinates: TEXCOORD0;"
                        "        float4 particleParameters: TEXCOORD1;"
                        "        float4 particleColor:      TEXCOORD2;"
                        "};"
                        ""
                        "Output main(Input input)"
                        "{"
                        "        Output output;"
                        "        float4 viewSpacePosition = mul(viewMatrix, float4(input.position.xyz, 1.0));"
                        "        viewSpacePosition.xyz /= viewSpacePosition.w;"
                        "        viewSpacePosition.xy += input.offsets.zw * input.position.w;"
                        ""
                        "        output.position = mul(projectionMatrix, float4(viewSpacePosition.xyz, 1.0));"
                        "        output.textureCoordinates = input.offsets;"
                        ""
                        "        output.particleParameters = float4(output.position.xy,"
                        "                                           viewSpacePosition.z,"
                        "                                           output.position.w);"
                        "        output.particleColor = input.particleColor;"
                        ""
                        "        return output;"
                        "}";

                static const char* pixelShaderParticlesPass =
                        "float4 textureCoordinatesAdjustment: register(c0);"
                        ""
                        "float4 unprojectionVector:   register(c1);"
                        "float4 projectionParameters: register(c2);"
                        ""
                        "sampler2D positionsBuffer: register(s0);"
                        "sampler2D particleTexture: register(s1);"
                        ""
                        "struct Input"
                        "{"
                        "        float4 textureCoordinates: TEXCOORD0;"
                        "        float4 particleParameters: TEXCOORD1;"
                        "        float4 particleColor:      TEXCOORD2;"
                        "};"
                        ""
                        "float4 main(Input input): COLOR0"
                        "{"
                        "        input.particleParameters.xy /= input.particleParameters.w;"
                        "        input.particleParameters.xy += textureCoordinatesAdjustment.xy;"
                        ""
                        "        float2 textureCoordinates = 0.5f * (float2( input.particleParameters.x,"
                        "                                                   -input.particleParameters.y) + 1.0);"
                        ""
                        "        float eyeZ = decodeEyeZ(tex2D(positionsBuffer, textureCoordinates),"
                        "                                projectionParameters);"
                        ""
                        "        float distance = abs(input.particleParameters.z - eyeZ);"
                        "        float opacity = clamp(-1.0 / (distance + 0.5) + 1.0, 0.0, 1.0);"
                        ""
                        "        return float4(1.0.xxx, opacity) * input.particleColor *"
                        "               tex2D(particleTexture, input.textureCoordinates.xy);"
                        "}";

                // load shaders
                D3d9Shader d3dVertexShaders[NUM_OF_VERTEX_SHADERS] =
                {
                        D3d9Shader(vertexShaderParticlesPass, "vs_1_1", 0,
                                   D3d9Shader::LIBRARY_EMPTY, *capabilities_)
                };

                D3d9Shader d3dPixelShaders[NUM_OF_PIXEL_SHADERS] =
                {
                        D3d9Shader(pixelShaderParticlesPass, "ps_2_0", 0,
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

                // create vertex declaration and vertex buffer
                static D3DVERTEXELEMENT9 d3dVertexElements[] =
                {
                        {0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
                        {0, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
                        {0, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
                        D3DDECL_END()
                };

                if(FAILED(d3dDevice_->CreateVertexDeclaration(d3dVertexElements, &d3dVertexDeclaration_)))
                {
                        destroy();
                        return false;
                }

                if(FAILED(d3dDevice_->CreateVertexBuffer(particlesGeometryBufferSize,
                                                         D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
                                                         0, D3DPOOL_DEFAULT, &d3dVertexBuffer_, nullptr)))
                {
                        destroy();
                        return false;
                }

                return true;
        }

        //------------------------------------------------------------------------------------------------------------
        void D3d9ParticlesRenderer::destroy()
        {
                for(uint32_t i = 0; i < NUM_OF_VERTEX_SHADERS; ++i)
                        vertexShaders_[i].destroy();

                for(uint32_t i = 0; i < NUM_OF_PIXEL_SHADERS; ++i)
                        pixelShaders_[i].destroy();

                SAFE_RELEASE(d3dVertexDeclaration_);
                SAFE_RELEASE(d3dVertexBuffer_);

                d3dDevice_ = nullptr;

                renderTargetContainer_ = nullptr;
                frameParameters_ = nullptr;
                textureHandler_ = nullptr;
                capabilities_ = nullptr;
        }

        //------------------------------------------------------------------------------------------------------------
        void D3d9ParticlesRenderer::renderParticleSystems(Renderer::Data::ParticleSystemNode& particleSystemNode)
        {
                if(d3dDevice_ == nullptr)
                        return;

                d3dDevice_->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
                d3dDevice_->SetRenderState(D3DRS_ZFUNC,        D3DCMP_LESSEQUAL);
                d3dDevice_->SetRenderState(D3DRS_CULLMODE,     D3DCULL_CW);

                d3dDevice_->SetVertexDeclaration(d3dVertexDeclaration_);
                d3dDevice_->SetStreamSource(1, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(2, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(3, nullptr, 0, 0);

                vertexShaders_[VERTEX_SHADER_PARTICLES_PASS].set();
                pixelShaders_[PIXEL_SHADER_PARTICLES_PASS].set();

                textureHandler_->setStageState(LOCATION_POSITIONS_MAP,
                                               D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE,
                                               D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(LOCATION_POSITIONS_MAP,
                                       renderTargetContainer_->getRenderTarget(RENDER_TARGET_POSITIONS).getTexture());

                textureHandler_->setStageState(LOCATION_PARTICLE_TEXTURE,
                                               D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_POINT);

                d3dDevice_->SetVertexShaderConstantF(LOCATION_PROJECTION_MATRIX,
                                                     frameParameters_->projectionMatrix,
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

                d3dDevice_->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
                d3dDevice_->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
                d3dDevice_->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

                for(bool result = particleSystemNode.readFirstElement(Renderer::Data::UNIT_PARTICLE_SYSTEM); result;
                         result = particleSystemNode.readNextElement())
                {
                        auto texture = particleSystemNode.getCurrentKey();
                        auto renderingList = particleSystemNode.getCurrentData();

                        if(texture == nullptr || renderingList == nullptr)
                                continue;

                        textureHandler_->setTexture(texture, LOCATION_PARTICLE_TEXTURE,
                                                    D3d9TextureHandler::DUMMY_TEXTURE_WHITE);

                        const auto& particleSystems = renderingList->getElements();
                        for(auto it = particleSystems.begin(); it != particleSystems.end(); ++it)
                                renderParticleSystem(*it);
                }

                d3dDevice_->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        }

        //------------------------------------------------------------------------------------------------------------
        void D3d9ParticlesRenderer::renderParticleSystem(ParticleSystem* particleSystem)
        {
                if(particleSystem == nullptr)
                        return;

                const auto& particles = particleSystem->getParticles();

                if(particles.getSize() == 0)
                        return;

                Vector4d vertices[2];
                uint32_t numParticles = 0;
                Vector4d* particlesGeometry = particlesGeometryBuffer;

                for(uint32_t i = 0; i < particles.getSize(); ++i)
                {
                        const ParticleSystem::Particle& particle = particles[i];

                        vertices[0].define(particle.getPosition(), particle.getSize());
                        vertices[1] = particle.getColor();

                        for(uint8_t i = 0; i < particlesGeometryNumVerticesPerParticle; ++i)
                        {
                                memcpy(particlesGeometry, vertices, sizeof(vertices));
                                particlesGeometry += particlesGeometryNumVectorsPerVertex;
                        }

                        ++numParticles;

                        if(numParticles == particlesGeometryBatchSize)
                        {
                                renderParticles(numParticles);

                                numParticles = 0;
                                particlesGeometry = particlesGeometryBuffer;
                        }
                }

                if(numParticles > 0)
                        renderParticles(numParticles);
        }

        //------------------------------------------------------------------------------------------------------------
        void D3d9ParticlesRenderer::renderParticles(uint32_t numParticles)
        {
                uint8_t* destinationBuffer = nullptr;
                uint32_t lockedSize = numParticles * particlesGeometryVertexStride *
                                      particlesGeometryNumVerticesPerParticle;
                if(FAILED(d3dVertexBuffer_->Lock(0, lockedSize, reinterpret_cast<void**>(&destinationBuffer), 0)))
                        return;

                memcpy(destinationBuffer, particlesGeometryBuffer, lockedSize);
                d3dVertexBuffer_->Unlock();

                d3dDevice_->SetStreamSource(0, d3dVertexBuffer_, 0, particlesGeometryVertexStride);
                d3dDevice_->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2 * numParticles);
        }

}
