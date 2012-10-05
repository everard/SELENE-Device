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

        Mesh::VertexElement::VertexElement(uint8_t type_,
                                           uint8_t size_,
                                           uint8_t offset_,
                                           uint8_t index_)
        {
                offset = offset_;
                index = index_;
                type = type_;
                size = size_;
        }
        Mesh::VertexElement::~VertexElement() {}

        Mesh::Mesh(const char* name): Resource(name) {}
        Mesh::~Mesh() {}

        //----------------------------
        bool Mesh::prepare()
        {
                return true;
        }

        //----------------------------
        Mesh::Data& Mesh::getData()
        {
                return data_;
        }

        //----------------------------
        bool Mesh::hasSkeleton() const
        {
                return (data_.skeleton.getBones().getSize() > 0);
        }

}
