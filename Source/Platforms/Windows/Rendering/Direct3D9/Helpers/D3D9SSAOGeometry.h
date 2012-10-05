// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef D3D9_SSAO_GEOMETRY_H
#define D3D9_SSAO_GEOMETRY_H

#include "../../../../../Engine/Core/Macros/Macros.h"
#include "../D3D9Device.h"

namespace selene
{

        // Represents D3D9 SSAO geometry.
        class D3d9SsaoGeometry
        {
        public:
                D3d9SsaoGeometry();
                ~D3d9SsaoGeometry();

                // Initializes SSAO geometry
                bool initialize();

                // Destroys SSAO geometry
                void destroy();

                // Renders SSAO geometry
                bool render();

        private:
                LPDIRECT3DVERTEXDECLARATION9 d3dVertexDeclaration_;
                LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffer_;
                LPDIRECT3DDEVICE9 d3dDevice_;

                static float vertices_[];
                static const uint32_t vertexStride_;
                static const uint32_t numVertices_;
                static const uint32_t numFaces_;

        };

}

#endif
