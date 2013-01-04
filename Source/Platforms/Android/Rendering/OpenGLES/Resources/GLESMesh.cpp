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
        bool GlesMesh::prepare()
        {
                // destroy OpenGL ES mesh data if any
                destroy();

                return true;
        }

        //-------------------------
        void GlesMesh::destroy() {}

}
