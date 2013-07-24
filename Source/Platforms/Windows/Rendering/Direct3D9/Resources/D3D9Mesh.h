// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef D3D9_MESH_H
#define D3D9_MESH_H

#include "../../../../../Engine/Core/Resources/Mesh/Mesh.h"
#include <d3d9.h>

namespace selene
{

        /**
         * \addtogroup Windows
         * @{
         */

        /**
         * Represents D3D9 mesh.
         */
        class D3d9Mesh: public Mesh
        {
        public:
                /**
                 * \brief Constructs D3D9 mesh with given name.
                 * \param[in] name name of the D3D9 mesh
                 */
                D3d9Mesh(const char* name = nullptr);
                D3d9Mesh(const D3d9Mesh&) = delete;
                ~D3d9Mesh();
                D3d9Mesh& operator =(const D3d9Mesh&) = delete;

                // Mesh interface implementation
                bool retain();
                void discard();

        private:
                friend class D3d9ActorsRenderer;

                LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffers_[NUM_OF_VERTEX_STREAMS];
                LPDIRECT3DINDEXBUFFER9 d3dIndexBuffer_;

                /**
                 * \brief Destroys D3D9 mesh data.
                 */
                void destroy();

        };

        /**
         * @}
         */

}

#endif
