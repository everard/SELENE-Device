// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Circle.h"

namespace selene
{

        Circle::Circle(const Vector2d& center, float radius): center_(center), radius_(radius) {}
        Circle::~Circle() {}

        //------------------------------------------------------------------------------------------------------
        void Circle::define(const Vector2d& center, float radius)
        {
                center_ = center;
                radius_ = radius;
        }

        //------------------------------------------------------------------------------------------------------
        void Circle::setCenter(const Vector2d& center)
        {
                center_ = center;
        }

        //------------------------------------------------------------------------------------------------------
        const Vector2d& Circle::getCenter() const
        {
                return center_;
        }

        //------------------------------------------------------------------------------------------------------
        void Circle::setRadius(float radius)
        {
                radius_ = radius;
        }

        //------------------------------------------------------------------------------------------------------
        float Circle::getRadius() const
        {
                return radius_;
        }

        SweptCircle::SweptCircle(const Vector2d& center, float radius, const Vector2d& direction): SweptCircle()
        {
                define(center, radius, direction);
        }
        SweptCircle::SweptCircle(): center_(), direction_(), radius_(1.0f), sweepDistance_(0.0f) {}
        SweptCircle::~SweptCircle() {}

        //------------------------------------------------------------------------------------------------------
        void SweptCircle::define(const Vector2d& center, float radius, const Vector2d& direction)
        {
                center_ = center;
                radius_ = radius;
                setDirection(direction);
        }

        //------------------------------------------------------------------------------------------------------
        void SweptCircle::setCenter(const Vector2d& center)
        {
                center_ = center;
        }

        //------------------------------------------------------------------------------------------------------
        const Vector2d& SweptCircle::getCenter() const
        {
                return center_;
        }

        //------------------------------------------------------------------------------------------------------
        void SweptCircle::setDirection(const Vector2d& direction)
        {
                direction_ = direction;
                sweepDistance_ = direction_.length();
                if(sweepDistance_ > SELENE_EPSILON)
                        direction_.normalize();
        }

        //------------------------------------------------------------------------------------------------------
        const Vector2d& SweptCircle::getDirection() const
        {
                return direction_;
        }

        //------------------------------------------------------------------------------------------------------
        void SweptCircle::setRadius(float radius)
        {
                radius_ = radius;
        }

        //------------------------------------------------------------------------------------------------------
        float SweptCircle::getRadius() const
        {
                return radius_;
        }

        //------------------------------------------------------------------------------------------------------
        float SweptCircle::getSweepDistance() const
        {
                return sweepDistance_;
        }

}
