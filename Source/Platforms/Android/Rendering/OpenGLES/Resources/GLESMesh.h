// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GLES_MESH_H
#define GLES_MESH_H

#include "../../../../../Engine/Core/Resources/Mesh/Mesh.h"
#include <GLES2/gl2.h>

namespace selene
{

        // Represents OpenGL ES mesh.
        class GlesMesh: public Mesh
        {
        public:
                GlesMesh(const char* name = nullptr);
                ~GlesMesh();

                // Retains OpenGL ES mesh
                bool retain();

                // Discards OpenGL ES mesh
                void discard();

        private:
                friend class GlesRenderer;

                GLuint vertexBuffers_[NUM_OF_VERTEX_STREAMS];
                GLuint indexBuffer_;

                // Destroys OpenGL ES mesh data
                void destroy();

        };

}

#endif
