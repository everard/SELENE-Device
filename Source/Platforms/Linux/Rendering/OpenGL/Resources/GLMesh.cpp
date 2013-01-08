// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLMesh.h"
#include <new>

namespace selene
{

        GlMesh::GlMesh(const char* name): Mesh(name) {}
        GlMesh::~GlMesh()
        {
                destroy();
        }

        //-----------------------
        bool GlMesh::retain()
        {
                // destroy OpenGL mesh data if any
                destroy();

                return true;
        }

        //-----------------------
        void GlMesh::discard()
        {
                destroy();
        }

        //-----------------------
        void GlMesh::destroy() {}

}
