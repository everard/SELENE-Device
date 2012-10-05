// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Line.h"

namespace selene
{

        Line2d::Line2d(const Vector2d& normal, const Vector2d& point)
        {
                define(normal, point);
        }
        Line2d::Line2d(const Vector2d* vertices)
        {
                define(vertices);
        }
        Line2d::Line2d()
        {
                c_ = 0.0f;
        }
        Line2d::~Line2d() {}

        //--------------------------------------------------------------------------
        void Line2d::define(const Vector2d& normal, const Vector2d& point)
        {
                normal_ = normal;
                c_ = -normal_.dot(point);
        }

        //--------------------------------------------------------------------------
        void Line2d::define(const Vector2d* vertices)
        {
                normal_ = vertices[1] - vertices[0];
                normal_.define(-normal_.y, normal_.x);

                if(normal_.length() > SELENE_EPSILON)
                        normal_.normalize();

                c_ = -normal_.dot(vertices[0]);
        }

        //--------------------------------------------------------------------------
        const Vector2d& Line2d::getNormal() const
        {
                return normal_;
        }

        //--------------------------------------------------------------------------
        float Line2d::distance(const Vector2d& point) const
        {
                return (normal_.dot(point) + c_);
        }

        LineSegment2d::LineSegment2d(const Vector2d* vertices): line_(vertices)
        {
                vertices_[0] = vertices[0];
                vertices_[1] = vertices[1];
        }
        LineSegment2d::LineSegment2d() {}
        LineSegment2d::~LineSegment2d() {}

        //--------------------------------------------------------------------------
        void LineSegment2d::define(const Vector2d* vertices)
        {
                vertices_[0] = vertices[0];
                vertices_[1] = vertices[1];
                line_.define(vertices_);
        }

        //--------------------------------------------------------------------------
        const Vector2d* LineSegment2d::getVertices() const
        {
                return vertices_;
        }

        //--------------------------------------------------------------------------
        const Line2d& LineSegment2d::getLine() const
        {
                return line_;
        }

        //--------------------------------------------------------------------------
        RELATION LineSegment2d::determineRelation(const Vector2d& point) const
        {
                Vector2d v0 = point - vertices_[0];
                Vector2d v1 = point - vertices_[1];
                float d = v0.dot(v1);
                if(d > SELENE_EPSILON)
                        return OUTSIDE;

                return INSIDE;
        }

        //--------------------------------------------------------------------------
        const Vector2d& LineSegment2d::getClosestVertex(const Vector2d& point) const
        {
                Vector2d v0 = point - vertices_[0];
                Vector2d v1 = point - vertices_[1];

                if(v0.dot(v0) < v1.dot(v1))
                        return vertices_[0];

                return vertices_[1];
        }

}
