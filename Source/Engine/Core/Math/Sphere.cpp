// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Sphere.h"

namespace selene
{

        Sphere::Sphere(const Vector3d& center, float radius)
        {
                define(center, radius);
        }
        Sphere::~Sphere() {}

        //-----------------------------------------------------------------
        void Sphere::define(const Vector3d& center, float radius)
        {
                center_ = center;
                radius_ = radius;
        }

        //-----------------------------------------------------------------
        void Sphere::setCenter(const Vector3d& center)
        {
                center_ = center;
        }

        //-----------------------------------------------------------------
        const Vector3d& Sphere::getCenter() const
        {
                return center_;
        }

        //-----------------------------------------------------------------
        void Sphere::setRadius(float radius)
        {
                radius_ = radius;
        }

        //-----------------------------------------------------------------
        float Sphere::getRadius() const
        {
                return radius_;
        }

        //-----------------------------------------------------------------
        RELATION Sphere::determineRelation(const Volume& volume) const
        {
                const Plane* planes = volume.getPlanes();
                uint8_t numPlanes = volume.getNumPlanes();
                RELATION result = INSIDE;

                // check all planes
                for(register uint8_t i = 0; i < numPlanes; ++i)
                {
                        // compute distance to the plane from sphere origin
                        float distance = planes[i].distance(center_);

                        if(distance <= -radius_)
                                return OUTSIDE;
                        else if(distance < radius_)
                                result = INTERSECTS;
                }

                return result;
        }

}
