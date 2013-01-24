// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef D3D9_PARTICLES_RENDERER_H
#define D3D9_PARTICLES_RENDERER_H

#include "../../../../../Engine/Core/Macros/Macros.h"
#include "D3D9Shader.h"

namespace selene
{

        // Forward declaration of classes
        class ParticleSystem;

        // Represents D3D9 particles renderer.
        class D3d9ParticlesRenderer
        {
        public:
                D3d9ParticlesRenderer();
                ~D3d9ParticlesRenderer();

                // Initializes particles renderer
                bool initialize();

                // Destroys particles renderer
                void destroy();

                // Sets particle system
                void setParticleSystem(ParticleSystem* particleSystem);

                // Prepares rendering
                bool prepare();

                // Renders particles
                bool render();

        private:
                LPDIRECT3DVERTEXDECLARATION9 d3dVertexDeclaration_;
                LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffer_;
                LPDIRECT3DDEVICE9 d3dDevice_;
                ParticleSystem* particleSystem_;

                uint32_t numParticleFaces_;

                static const uint32_t numVerticesPerParticle_;
                static const uint32_t vertexStride_;
                static const uint32_t numVertices_;
                static const uint32_t batchSize_;
                static const uint32_t numFaces_;

        };

}

#endif
