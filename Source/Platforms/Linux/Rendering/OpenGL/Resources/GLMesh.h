// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GL_MESH_H
#define GL_MESH_H

#include "../../../../../Engine/Core/Resources/Mesh/Mesh.h"

namespace selene
{

        // Represents OpenGL mesh.
        class GlMesh: public Mesh
        {
        public:
                GlMesh(const char* name = nullptr);
                ~GlMesh();

                // Retains OpenGL mesh
                bool retain();

                // Discards OpenGL mesh
                void discard();

        private:
                friend class GlRenderer;

                // Destroys OpenGL mesh data
                void destroy();

        };

}

#endif
