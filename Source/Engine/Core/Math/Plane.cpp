// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Plane.h"

namespace selene
{

        Plane::Plane(float a, float b, float c, float d)
        {
                define(a, b, c, d);
        }
        Plane::Plane(const Vector3d& normal, const Vector3d& point)
        {
                define(normal, point);
        }
        Plane::Plane(const Vector3d& point0,
                     const Vector3d& point1,
                     const Vector3d& point2)
        {
                define(point0, point1, point2);
        }
        Plane::~Plane() {}

        //---------------------------------------------------------------
        void Plane::define(float a, float b, float c, float d)
        {
                normal_.define(a, b, c);
                d_ = d;
        }

        //---------------------------------------------------------------
        void Plane::define(const Vector3d& normal, const Vector3d& point)
        {
                normal_ = normal;
                d_ = -normal_.dot(point);
        }

        //---------------------------------------------------------------
        void Plane::define(const Vector3d& point0,
                           const Vector3d& point1,
                           const Vector3d& point2)
        {
                normal_ = -((point1 - point0).cross(point2 - point0));
                normal_.normalize();

                d_ = -normal_.dot(point0);
        }

        //---------------------------------------------------------------
        float Plane::distance(const Vector3d& point) const
        {
                return (normal_.dot(point) + d_);
        }

        //---------------------------------------------------------------
        void Plane::normalize()
        {
                float l = 1.0f / normal_.length();
                normal_ *= l;
                d_ *= l;
        }

}
