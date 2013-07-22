// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef RAY_H
#define RAY_H

#include "Vector.h"

namespace selene
{

        /**
         * \addtogroup Math
         * @{
         */

        // Forward declaration of classes
        class LineSegment2d;
        class Line2d;
        class Circle;
        class Sphere;

        /**
         * Represents ray in 2D space.
         */
        class Ray2d
        {
        public:
                /**
                 * \brief Constructs ray with given origin and direction.
                 * \param[in] origin origin of the ray
                 * \param[in] direction direction of the ray
                 */
                Ray2d(const Vector2d& origin, const Vector2d& direction);
                Ray2d(const Ray2d&) = default;
                ~Ray2d();
                Ray2d& operator =(const Ray2d&) = default;

                /**
                 * \brief Defines ray with given origin and direction (direction must be already normalized).
                 * \param[in] origin origin of the ray
                 * \param[in] direction direction of the ray
                 */
                void define(const Vector2d& origin, const Vector2d& direction);

                /**
                 * \brief Sets origin.
                 * \param[in] origin origin of the ray
                 */
                void setOrigin(const Vector2d& origin);

                /**
                 * \brief Returns origin.
                 * \return origin of the ray
                 */
                const Vector2d& getOrigin() const;

                /**
                 * \brief Sets direction.
                 * \param[in] direction direction of the ray
                 */
                void setDirection(const Vector2d& direction);

                /**
                 * \brief Returns direction.
                 * \return direction of the ray
                 */
                const Vector2d& getDirection() const;

                /**
                 * \brief Determines intersection with line.
                 * \param[in] line line
                 * \param[out] distanceToIntersection distance to intersection point
                 * \return true if ray intersects the line
                 */
                bool intersects(const Line2d& line, float& distanceToIntersection) const;

                /**
                 * \brief Determines intersection with line segment.
                 * \param[in] lineSegment line segment
                 * \param[out] distanceToIntersection distance to intersection point
                 * \return true if ray intersects the line segment
                 */
                bool intersects(const LineSegment2d& lineSegment, float& distanceToIntersection) const;

                /**
                 * \brief Determines intersection with circle.
                 * \param[in] circle circle
                 * \param[out] distanceToIntersection distance to intersection point (closest one)
                 * \return true if ray intersects the circle
                 */
                bool intersects(const Circle& circle, float& distanceToIntersection) const;

        private:
                Vector2d origin_, direction_;

        };

        /**
         * Represents ray in 3D space.
         */
        class Ray3d
        {
        public:
                /**
                 * \brief Constructs ray with given origin and direction.
                 * \param[in] origin origin of the ray
                 * \param[in] direction direction of the ray
                 */
                Ray3d(const Vector3d& origin, const Vector3d& direction);
                Ray3d(const Ray3d&) = default;
                ~Ray3d();
                Ray3d& operator =(const Ray3d&) = default;

                /**
                 * \brief Defines ray with given origin and direction (direction must be already normalized).
                 * \param[in] origin origin of the ray
                 * \param[in] direction direction of the ray
                 */
                void define(const Vector3d& origin, const Vector3d& direction);

                /**
                 * \brief Sets origin.
                 * \param[in] origin origin of the ray
                 */
                void setOrigin(const Vector3d& origin);

                /**
                 * \brief Returns origin.
                 * \return origin of the ray
                 */
                const Vector3d& getOrigin() const;

                /**
                 * \brief Sets direction.
                 * \param[in] direction direction of the ray
                 */
                void setDirection(const Vector3d& direction);

                /**
                 * \brief Returns direction.
                 * \return direction of the ray
                 */
                const Vector3d& getDirection() const;

                /**
                 * \brief Determines intersection with sphere.
                 * \param[in] sphere sphere
                 * \param[out] distanceToIntersection distance to intersection point (closest one)
                 * \return true if ray intersects the sphere
                 */
                bool intersects(const Sphere& sphere, float& distanceToIntersection) const;

        private:
                Vector3d origin_, direction_;

        };

        /**
         * @}
         */

}

#endif
