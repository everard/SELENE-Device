// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLESMesh.h"
#include <new>

namespace selene
{

        GlesMesh::GlesMesh(const char* name): Mesh(name) {}
        GlesMesh::~GlesMesh()
        {
                destroy();
        }

        //-------------------------
        bool GlesMesh::retain()
        {
                // destroy OpenGL ES mesh data if any
                destroy();

                return true;
        }

        //-------------------------
        void GlesMesh::discard()
        {
                destroy();
        }

        //-------------------------
        void GlesMesh::destroy() {}

}
