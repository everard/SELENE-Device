// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Mesh.h"

namespace selene
{

        Mesh::Subset::Subset():
                vertexIndex(0), numVertices(0), faceIndex(0),
                numFaces(0), material() {}
        Mesh::Subset::~Subset() {}

        Mesh::Data::Data(): faces(), subsets(), boundingBox(), skeleton() {}
        Mesh::Data::~Data() {}

        Mesh::Mesh(const char* name): Resource(name), data_() {}
        Mesh::~Mesh() {}

        //------------------------------------------------------------------
        Mesh::Data& Mesh::getData()
        {
                return data_;
        }

        //------------------------------------------------------------------
        bool Mesh::hasSkeleton() const
        {
                return static_cast<bool>(data_.skeleton);
        }

}
