// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef SPHERE_H
#define SPHERE_H

#include "Vector.h"
#include "Volume.h"

namespace selene
{

        /**
         * \addtogroup Math
         * @{
         */

        /// Relation between geometry objects
        enum RELATION
        {
                OUTSIDE = 0,
                INSIDE,
                INTERSECTS
        };

        /**
         * Represents sphere in 3D space.
         */
        class Sphere
        {
        public:
                /**
                 * \brief Constructs sphere with given center and radius.
                 * \param[in] center center of the sphere
                 * \param[in] radius radius of the sphere
                 */
                Sphere(const Vector3d& center = Vector3d(), float radius = 1.0f);
                Sphere(const Sphere&) = default;
                ~Sphere();
                Sphere& operator =(const Sphere&) = default;

                /**
                 * \brief Defines sphere with given center and radius.
                 * \param[in] center center of the sphere
                 * \param[in] radius radius of the sphere
                 */
                void define(const Vector3d& center, float radius);

                /**
                 * \brief Sets center.
                 * \param[in] center center of the sphere
                 */
                void setCenter(const Vector3d& center);

                /**
                 * \brief Returns center.
                 * \return center of the sphere
                 */
                const Vector3d& getCenter() const;

                /**
                 * \brief Sets radius.
                 * \param[in] radius radius of the sphere
                 */
                void setRadius(float radius);

                /**
                 * \brief Returns radius.
                 * \return radius of the sphere
                 */
                float getRadius() const;

                /**
                 * \brief Determines relation between sphere and volume.
                 * \param[in] volume volume
                 * \return OUTSIDE if sphere is ouside the volume, INTERSECTS if
                 * sphere intersects the volume and INSIDE if sphere is inside the volume
                 */
                RELATION determineRelation(const Volume& volume) const;

        private:
                Vector3d center_;
                float radius_;

        };

        /**
         * @}
         */

}

#endif
