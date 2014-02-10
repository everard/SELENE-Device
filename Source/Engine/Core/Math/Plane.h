// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef PLANE_H
#define PLANE_H

#include "Vector.h"

namespace selene
{

        /**
         * \addtogroup Math
         * @{
         */

        /**
         * Represents plane in 3D space.
         */
        class Plane
        {
        public:
                /**
                 * \brief Constructs plane with given coefficients.
                 * \param[in] a x coordinate of normal
                 * \param[in] b y coordinate of normal
                 * \param[in] c z coordinate of normal
                 * \param[in] d constant in plane equation
                 */
                Plane(float a = 0.0f, float b = 0.0f, float c = 0.0f, float d = 0.0f);
                /**
                 * \brief Constructs plane with given normal and point.
                 * \param[in] normal normal of the plane
                 * \param[in] point point on the plane
                 */
                Plane(const Vector3d& normal, const Vector3d& point);
                /**
                 * \brief Constructs plane from three points.
                 * \param[in] point0 the first point
                 * \param[in] point1 the second point
                 * \param[in] point2 the third point
                 */
                Plane(const Vector3d& point0, const Vector3d& point1, const Vector3d& point2);
                Plane(const Plane&) = default;
                ~Plane();
                Plane& operator =(const Plane&) = default;

                /**
                 * \brief Defines plane with given coefficients.
                 * \param[in] a x coordinate of normal
                 * \param[in] b y coordinate of normal
                 * \param[in] c z coordinate of normal
                 * \param[in] d constant in plane equation
                 */
                void define(float a, float b, float c, float d);

                /**
                 * \brief Defines plane with given normal and point.
                 * \param[in] normal normal of the plane
                 * \param[in] point point on the plane
                 */
                void define(const Vector3d& normal, const Vector3d& point);

                /**
                 * \brief Defines plane with three points.
                 * \param[in] point0 the first point
                 * \param[in] point1 the second point
                 * \param[in] point2 the third point
                 */
                void define(const Vector3d& point0, const Vector3d& point1, const Vector3d& point2);

                /**
                 * \brief Computes distance to the given point.
                 * \param[in] point point
                 * \return distance to the point
                 */
                float distance(const Vector3d& point) const;

                /**
                 * \brief Normalizes plane.
                 */
                void normalize();

        private:
                Vector3d normal_;
                float d_;

        };

        /**
         * @}
         */

}

#endif
