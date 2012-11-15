// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GLES_MESH_H
#define GLES_MESH_H

#include "../../../../../Engine/Core/Resources/Mesh/Mesh.h"

namespace selene
{

        // Represents OpenGLES mesh.
        class GlesMesh: public Mesh
        {
        public:
                GlesMesh(const char* name = nullptr);
                ~GlesMesh();

                // Prepares OpenGLES mesh
                bool prepare();

        private:
                friend class GlesRenderer;

                // Destroys OpenGLES mesh data
                void destroy();

        };

}

#endif
