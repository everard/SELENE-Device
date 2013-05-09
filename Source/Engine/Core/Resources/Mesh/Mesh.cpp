// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Mesh.h"

namespace selene
{

        Mesh::Subset::Subset()
        {
                vertexIndex = numVertices = faceIndex = numFaces = 0;
        }
        Mesh::Subset::~Subset() {}

        Mesh::Mesh(const char* name): Resource(name) {}
        Mesh::~Mesh() {}

        //-----------------------------------------------------------
        Mesh::Data& Mesh::getData()
        {
                return data_;
        }

        //-----------------------------------------------------------
        bool Mesh::hasSkeleton() const
        {
                return static_cast<bool>(data_.skeleton);
        }

}
