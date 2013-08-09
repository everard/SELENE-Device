// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Cone.h"

namespace selene
{

        Cone::Cone(const Vector3d& apex,
                   const Vector3d& direction,
                   float radius):
                apex_(apex), direction_(direction), radius_(radius) {}
        Cone::~Cone() {}

        //-----------------------------------------------------------------
        void Cone::define(const Vector3d& apex,
                          const Vector3d& direction,
                          float radius)
        {
                apex_ = apex;
                direction_ = direction;
                radius_ = radius;
        }

        //-----------------------------------------------------------------
        void Cone::setApex(const Vector3d& apex)
        {
                apex_ = apex;
        }

        //-----------------------------------------------------------------
        const Vector3d& Cone::getApex() const
        {
                return apex_;
        }

        //-----------------------------------------------------------------
        void Cone::setDirection(const Vector3d& direction)
        {
                direction_ = direction;
        }

        //-----------------------------------------------------------------
        const Vector3d& Cone::getDirection() const
        {
                return direction_;
        }

        //-----------------------------------------------------------------
        void Cone::setRadius(float radius)
        {
                radius_ = radius;
        }

        //-----------------------------------------------------------------
        float Cone::getRadius() const
        {
                return radius_;
        }

        //-----------------------------------------------------------------
        RELATION Cone::determineRelation(const Volume& volume) const
        {
                const Plane* planes = volume.getPlanes();
                uint8_t numPlanes = volume.getNumPlanes();
                Vector3d point = apex_ + direction_;

                for(uint8_t i = 0; i < numPlanes; ++i)
                {
                        // check vertices
                        if(planes[i].distance(apex_) >= 0.0f) continue;
                        if(planes[i].distance(point) >= -radius_) continue;

                        return OUTSIDE;
                }

                return INSIDE;
        }

}
