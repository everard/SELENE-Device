// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "D3D9ParticlesRenderer.h"

#include "../../../../../Engine/Scene/Nodes/ParticleSystem.h"
#include "../../../../../Engine/Core/Helpers/Bag.h"
#include "../D3D9Renderer.h"

namespace selene
{

        const uint32_t D3d9ParticlesRenderer::numVerticesPerParticle_ = 6;

        const uint32_t D3d9ParticlesRenderer::vertexStride_ = 12 * sizeof(float);

        const uint32_t D3d9ParticlesRenderer::numVertices_ = 24576;

        const uint32_t D3d9ParticlesRenderer::batchSize_ = D3d9ParticlesRenderer::numVertices_ /
                                                           D3d9ParticlesRenderer::numVerticesPerParticle_;

        const uint32_t D3d9ParticlesRenderer::numFaces_ = 8192;

        D3d9ParticlesRenderer::D3d9ParticlesRenderer()
        {
                d3dVertexDeclaration_ = nullptr;
                d3dVertexBuffer_ = nullptr;
                d3dDevice_ = nullptr;
                particleSystem_ = nullptr;

                numParticleFaces_ = 0;
        }
        D3d9ParticlesRenderer::~D3d9ParticlesRenderer()
        {
                destroy();
        }

        //---------------------------------------------------------------------------
        bool D3d9ParticlesRenderer::initialize()
        {
                d3dDevice_ = D3d9Renderer::getDevice();
                if(d3dDevice_ == nullptr)
                        return false;

                destroy();

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

                if(FAILED(d3dDevice_->CreateVertexBuffer(numVertices_ * vertexStride_, D3DUSAGE_WRITEONLY,
                                                         0, D3DPOOL_DEFAULT, &d3dVertexBuffer_, nullptr)))
                {
                        destroy();
                        return false;
                }

                uint8_t* destinationBuffer = nullptr;
                if(FAILED(d3dVertexBuffer_->Lock(0, numVertices_ * vertexStride_,
                                                 (void**)&destinationBuffer, 0)))
                {
                        destroy();
                        return false;
                }

                Vector4d vertices[] =
                {
                        Vector4d(), Vector4d(), Vector4d(0.0f, 0.0f, -1.0f, -1.0f),
                        Vector4d(), Vector4d(), Vector4d(1.0f, 0.0f,  1.0f, -1.0f),

                        Vector4d(), Vector4d(), Vector4d(0.0f, 1.0f, -1.0f,  1.0f),
                        Vector4d(), Vector4d(), Vector4d(0.0f, 1.0f, -1.0f,  1.0f),

                        Vector4d(), Vector4d(), Vector4d(1.0f, 0.0f,  1.0f, -1.0f),
                        Vector4d(), Vector4d(), Vector4d(1.0f, 1.0f,  1.0f,  1.0f)
                };

                for(uint32_t i = 0; i < batchSize_; ++i)
                {
                        memcpy(destinationBuffer, vertices, sizeof(vertices));
                        destinationBuffer += numVerticesPerParticle_ * vertexStride_;
                }

                d3dVertexBuffer_->Unlock();
                return true;
        }

        //---------------------------------------------------------------------------
        void D3d9ParticlesRenderer::destroy()
        {
                SAFE_RELEASE(d3dVertexDeclaration_);
                SAFE_RELEASE(d3dVertexBuffer_);

                numParticleFaces_ = 0;
        }

        //---------------------------------------------------------------------------
        void D3d9ParticlesRenderer::setParticleSystem(ParticleSystem* particleSystem)
        {
                particleSystem_ = particleSystem;
        }

        //---------------------------------------------------------------------------
        bool D3d9ParticlesRenderer::prepare()
        {
                numParticleFaces_ = 0;

                if(d3dVertexDeclaration_ == nullptr || d3dVertexBuffer_ == nullptr ||
                   d3dDevice_ == nullptr || particleSystem_ == nullptr)
                        return false;

                const Bag<ParticleSystem::Particle>& particles = particleSystem_->getParticles();

                if(particles.getSize() == 0)
                        return false;

                uint8_t* destinationBuffer = nullptr;
                if(FAILED(d3dVertexBuffer_->Lock(0, numVertices_ * vertexStride_,
                                                 (void**)&destinationBuffer, 0)))
                {
                        destroy();
                        return false;
                }

                Vector4d vertex[2];

                for(uint32_t i = 0; i < particles.getSize(); ++i)
                {
                        if(numParticleFaces_ >= numFaces_)
                                break;

                        const ParticleSystem::Particle& particle = particles[i];
                        vertex[0].define(particle.getPosition(), particle.getSize());
                        vertex[1] = particle.getColor();

                        for(uint8_t i = 0; i < numVerticesPerParticle_; ++i)
                        {
                                memcpy(destinationBuffer, vertex, sizeof(vertex));
                                destinationBuffer += vertexStride_;
                        }

                        numParticleFaces_ += 2;
                }

                d3dVertexBuffer_->Unlock();
                d3dDevice_->SetVertexDeclaration(d3dVertexDeclaration_);
                d3dDevice_->SetStreamSource(0, d3dVertexBuffer_, 0, vertexStride_);
                return true;
        }

        //---------------------------------------------------------------------------
        bool D3d9ParticlesRenderer::render()
        {
                if(numParticleFaces_ == 0)
                        return false;

                d3dDevice_->DrawPrimitive(D3DPT_TRIANGLELIST, 0, numParticleFaces_);

                return true;
        }

}
