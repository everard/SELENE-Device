// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef LINE_H
#define LINE_H

#include "Sphere.h"

namespace selene
{

        /**
         * \addtogroup Math
         * @{
         */

        /**
         * Represents line in 2D space.
         */
        class Line2d
        {
        public:
                /**
                 * \brief Constructs line from given normal and point.
                 * \param[in] normal normal of the line
                 * \param[in] point point on the line
                 */
                Line2d(const Vector2d& normal, const Vector2d& point);
                /**
                 * \brief Constructs line from given vertices.
                 * \param[in] vertices two vertices which lie on the line
                 */
                Line2d(const Vector2d* vertices);
                Line2d(const Line2d&) = default;
                Line2d();
                ~Line2d();
                Line2d& operator =(const Line2d&) = default;

                /**
                 * \brief Defines line with given normal and point.
                 * \param[in] normal normal of the line
                 * \param[in] point point on the line
                 */
                void define(const Vector2d& normal, const Vector2d& point);

                /**
                 * \brief Defines line with given vertices.
                 * \param[in] vertices two vertices which lie on the line
                 */
                void define(const Vector2d* vertices);

                /**
                 * \brief Returns normal.
                 * \return normal of the line
                 */
                const Vector2d& getNormal() const;

                /**
                 * \brief Computes distance from a point to the line.
                 * \param[in] point point
                 * \return signed distance from a point to the line
                 */
                float distance(const Vector2d& point) const;

        private:
                Vector2d normal_;
                float c_;

        };

        /**
         * Represents line segment in 2D space.
         */
        class LineSegment2d
        {
        public:
                /// Helper constants
                enum
                {
                        NUM_OF_VERTICES = 2
                };

                /**
                 * \brief Constructs line segment from given vertices.
                 * \param[in] vertices vertices of the line segment
                 */
                LineSegment2d(const Vector2d* vertices);
                LineSegment2d(const LineSegment2d&) = default;
                LineSegment2d();
                ~LineSegment2d();
                LineSegment2d& operator =(const LineSegment2d&) = default;

                /**
                 * \brief Defines line segment with given vertices.
                 * \param[in] vertices vertices of the line segment
                 */
                void define(const Vector2d* vertices);

                /**
                 * \brief Returns vertices.
                 * \return vertices of the line segment
                 */
                const Vector2d* getVertices() const;

                /**
                 * \brief Returns line.
                 * \return line containing current segment
                 */
                const Line2d& getLine() const;

                /**
                 * \brief Determines relation between the line segment and a point on the line
                 * containing current segment.
                 * \param[in] point point on the line containing current segment
                 * \return INSIDE if given point is inside the line segment, OUTSIDE otherwise
                 * (INTERSECTS is never returned)
                 */
                RELATION determineRelation(const Vector2d& point) const;

                /**
                 * \brief Returns closest to the given point vertex of the line segment.
                 * \param[in] point point
                 * \return closest to the given point vertex of the line segment
                 */
                const Vector2d& getClosestVertex(const Vector2d& point) const;

        private:
                Vector2d vertices_[NUM_OF_VERTICES];
                Line2d line_;

        };

        /**
         * @}
         */

}

#endif
