// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Box.h"

namespace selene
{

        Box::Box(const Vector3d& center,
                 float width,
                 float height,
                 float depth)
        {
                define(center, width, height, depth);
        }
        Box::Box(const Vector3d* vertices)
        {
                define(vertices);
        }
        Box::~Box() {}

        //---------------------------------------------------------
        void Box::define(const Vector3d& center,
                         float width,
                         float height,
                         float depth)
        {
                // compute half of width, height and depth
                width  *= 0.5f;
                height *= 0.5f;
                depth  *= 0.5f;

                // compute vertices
                vertices_[0].define(center.x - width, center.y - height, center.z - depth);
                vertices_[1].define(center.x + width, center.y - height, center.z - depth);
                vertices_[2].define(center.x - width, center.y + height, center.z - depth);
                vertices_[3].define(center.x + width, center.y + height, center.z - depth);
                vertices_[4].define(center.x - width, center.y - height, center.z + depth);
                vertices_[5].define(center.x + width, center.y - height, center.z + depth);
                vertices_[6].define(center.x - width, center.y + height, center.z + depth);
                vertices_[7].define(center.x + width, center.y + height, center.z + depth);
        }

        //---------------------------------------------------------
        void Box::define(const Vector3d* vertices)
        {
                for(register uint8_t i = 0; i < 8; ++i)
                        vertices_[i] = vertices[i];
        }

        //---------------------------------------------------------
        const Vector3d* Box::getVertices() const
        {
                return vertices_;
        }

        //---------------------------------------------------------
        RELATION Box::determineRelation(const Volume& volume) const
        {
                const Plane* planes = volume.getPlanes();
                uint8_t numPlanes = volume.getNumPlanes();

                // check all planes
                for(register uint8_t i = 0; i < numPlanes; ++i)
                {
                        // check vertices
                        if(planes[i].distance(vertices_[0]) >= 0.0f) continue;
                        if(planes[i].distance(vertices_[1]) >= 0.0f) continue;
                        if(planes[i].distance(vertices_[2]) >= 0.0f) continue;
                        if(planes[i].distance(vertices_[3]) >= 0.0f) continue;
                        if(planes[i].distance(vertices_[4]) >= 0.0f) continue;
                        if(planes[i].distance(vertices_[5]) >= 0.0f) continue;
                        if(planes[i].distance(vertices_[6]) >= 0.0f) continue;
                        if(planes[i].distance(vertices_[7]) >= 0.0f) continue;

                        return OUTSIDE;
                }

                return INSIDE;
        }

        //---------------------------------------------------------
        void Box::transform(const Matrix& matrix)
        {
                vertices_[0].transform(matrix);
                vertices_[1].transform(matrix);
                vertices_[2].transform(matrix);
                vertices_[3].transform(matrix);
                vertices_[4].transform(matrix);
                vertices_[5].transform(matrix);
                vertices_[6].transform(matrix);
                vertices_[7].transform(matrix);
        }

}
