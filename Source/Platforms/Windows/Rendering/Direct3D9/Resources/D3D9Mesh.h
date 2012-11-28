// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef D3D9_MESH_H
#define D3D9_MESH_H

#include "../../../../../Engine/Core/Resources/Mesh/Mesh.h"
#include "../D3D9Device.h"

namespace selene
{

        // Represents D3D9 mesh.
        class D3d9Mesh: public Mesh
        {
        public:
                D3d9Mesh(const char* name = nullptr);
                ~D3d9Mesh();

                // Prepares D3D9 mesh
                bool prepare();

        private:
                friend class D3d9Renderer;

                LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffers_[NUM_OF_VERTEX_STREAMS];
                LPDIRECT3DINDEXBUFFER9 d3dIndexBuffer_;

                // Destroys D3D9 mesh data
                void destroy();

        };

}

#endif
