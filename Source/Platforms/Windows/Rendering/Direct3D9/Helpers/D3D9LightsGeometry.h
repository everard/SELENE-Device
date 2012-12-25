// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef D3D9_LIGHTS_GEOMETRY_H
#define D3D9_LIGHTS_GEOMETRY_H

#include "../../../../../Engine/Rendering/Renderer.h"
#include "../../../../../Engine/Scene/Nodes/Light.h"
#include "../D3D9Device.h"

namespace selene
{

        // Represents D3D9 lights geometry.
        class D3d9LightsGeometry
        {
        public:
                enum
                {
                        LIGHT_DIRECTIONAL = 0,
                        LIGHT_POINT,
                        LIGHT_SPOT,
                        NUM_OF_LIGHT_TYPES
                };

                D3d9LightsGeometry();
                ~D3d9LightsGeometry();

                // Initializes lights geometry
                bool initialize();

                // Destroys lights geometry
                void destroy();

                // Returns batch size
                uint32_t getBatchSize() const;

                // Begins rendering
                bool beginRendering(uint8_t lightType);

                // Ends rendering
                void endRendering();

                // Renders lights geometry
                bool render(uint32_t numLights);

        private:
                LPDIRECT3DVERTEXDECLARATION9 d3dVertexDeclarations_[NUM_OF_LIGHT_TYPES];
                LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffers_[NUM_OF_LIGHT_TYPES];
                LPDIRECT3DDEVICE9 d3dDevice_;

                int16_t currentLightType_;

                static float vertices_[];
                static const uint32_t batchSize_;
                static const uint32_t offsets_[NUM_OF_LIGHT_TYPES];
                static const uint32_t numVertices_[NUM_OF_LIGHT_TYPES];
                static const uint32_t vertexStrides_[NUM_OF_LIGHT_TYPES];
                static const uint32_t vertexBufferSizes_[NUM_OF_LIGHT_TYPES];

        };

}

#endif
