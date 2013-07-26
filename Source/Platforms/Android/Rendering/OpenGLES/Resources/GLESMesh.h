// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GLES_MESH_H
#define GLES_MESH_H

#include "../../../../../Engine/Core/Resources/Mesh/Mesh.h"
#include <GLES2/gl2.h>

namespace selene
{

        /**
         * \addtogroup Android
         * @{
         */

        /**
         * Represents OpenGL ES mesh.
         */
        class GlesMesh: public Mesh
        {
        public:
                /**
                 * \brief Constructs OpenGL ES mesh with given name.
                 * \param[in] name name of the mesh
                 */
                GlesMesh(const char* name = nullptr);
                GlesMesh(const GlesMesh&) = delete;
                ~GlesMesh();
                GlesMesh& operator =(const GlesMesh&) = delete;

                // Mesh interface implementation
                bool retain();
                void discard();

        private:
                friend class GlesActorsRenderer;

                GLuint vertexBuffers_[NUM_OF_VERTEX_STREAMS];
                GLuint indexBuffer_;

                /**
                 * \brief Destroys OpenGL ES mesh data.
                 */
                void destroy();

        };

        /**
         * @}
         */

}

#endif
