// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Circle.h"
#include "Sphere.h"
#include "Line.h"
#include "Ray.h"

#include <limits>

namespace selene
{

        Ray2d::Ray2d(const Vector2d& origin, const Vector2d& direction):
                origin_(origin), direction_(direction) {}
        Ray2d::~Ray2d() {}

        //--------------------------------------------------------------------------------------------------
        void Ray2d::define(const Vector2d& origin, const Vector2d& direction)
        {
                origin_ = origin;
                direction_ = direction;
        }

        //--------------------------------------------------------------------------------------------------
        void Ray2d::setOrigin(const Vector2d& origin)
        {
                origin_ = origin;
        }

        //--------------------------------------------------------------------------------------------------
        const Vector2d& Ray2d::getOrigin() const
        {
                return origin_;
        }

        //--------------------------------------------------------------------------------------------------
        void Ray2d::setDirection(const Vector2d& direction)
        {
                direction_ = direction;
        }

        //--------------------------------------------------------------------------------------------------
        const Vector2d& Ray2d::getDirection() const
        {
                return direction_;
        }

        //--------------------------------------------------------------------------------------------------
        bool Ray2d::intersects(const Line2d& line, float& distanceToIntersection) const
        {
                const float epsilon = std::numeric_limits<float>::epsilon() * 10.0f;

                float denom = line.getNormal().dot(direction_);
                if(std::fabs(denom) < std::fabs(denom) * epsilon)
                        return false;

                float numer = line.distance(origin_);

                distanceToIntersection = -numer / denom;
                return true;
        }

        //--------------------------------------------------------------------------------------------------
        bool Ray2d::intersects(const LineSegment2d& lineSegment, float& distanceToIntersection) const
        {
                if(!intersects(lineSegment.getLine(), distanceToIntersection))
                        return false;

                Vector2d intersectionPoint = origin_ + direction_ * distanceToIntersection;
                if(lineSegment.determineRelation(intersectionPoint) != INSIDE)
                        return false;

                return true;
        }

        //--------------------------------------------------------------------------------------------------
        bool Ray2d::intersects(const Circle& circle, float& distanceToIntersection) const
        {
                Vector2d newRayOrigin = origin_ - circle.getCenter();
                float aInv = 1.0f / direction_.dot(direction_);
                float b = aInv * 2.0f * direction_.dot(newRayOrigin);
                float c = aInv * (newRayOrigin.dot(newRayOrigin) - circle.getRadius() * circle.getRadius());
                float d = b * b - 4.0f * c;
                if(d < 0.0f)
                        return false;

                d = std::sqrt(d);
                distanceToIntersection = (-b - d) * 0.5f;
                if(distanceToIntersection > 0.0f)
                        return true;

                distanceToIntersection = (-b + d) * 0.5f;
                return true;
        }

        Ray3d::Ray3d(const Vector3d& origin, const Vector3d& direction):
                origin_(origin), direction_(direction) {}
        Ray3d::~Ray3d() {}

        //--------------------------------------------------------------------------------------------------
        void Ray3d::define(const Vector3d& origin, const Vector3d& direction)
        {
                origin_ = origin;
                direction_ = direction;
        }

        //--------------------------------------------------------------------------------------------------
        void Ray3d::setOrigin(const Vector3d& origin)
        {
                origin_ = origin;
        }

        //--------------------------------------------------------------------------------------------------
        const Vector3d& Ray3d::getOrigin() const
        {
                return origin_;
        }

        //--------------------------------------------------------------------------------------------------
        void Ray3d::setDirection(const Vector3d& direction)
        {
                direction_ = direction;
        }

        //--------------------------------------------------------------------------------------------------
        const Vector3d& Ray3d::getDirection() const
        {
                return direction_;
        }

        //--------------------------------------------------------------------------------------------------
        bool Ray3d::intersects(const Sphere& sphere, float& distanceToIntersection) const
        {
                Vector3d newRayOrigin = origin_ - sphere.getCenter();
                float aInv = 1.0f / direction_.dot(direction_);
                float b = aInv * 2.0f * direction_.dot(newRayOrigin);
                float c = aInv * (newRayOrigin.dot(newRayOrigin) - sphere.getRadius() * sphere.getRadius());
                float d = b * b - 4.0f * c;
                if(d < 0.0f)
                        return false;

                d = std::sqrt(d);
                distanceToIntersection = (-b - d) * 0.5f;
                if(distanceToIntersection > 0.0f)
                        return true;

                distanceToIntersection = (-b + d) * 0.5f;
                return true;
        }

}
