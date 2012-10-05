// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef CONE_H
#define CONE_H

#include "Sphere.h"

namespace selene
{

        /**
         * Represents cone in 3D space.
         */
        class Cone
        {
        public:
                /**
                 * \brief Constructs cone with given apex, direction and radius.
                 * \param[in] apex apex of the cone
                 * \param[in] direction direction from the apex to the base
                 * \param[in] radius radius of the base
                 */
                Cone(const Vector3d& apex = Vector3d(),
                     const Vector3d& direction = Vector3d(1.0f),
                     float radius = 1.0f);
                ~Cone();

                /**
                 * \brief Defines cone with given apex, direction and radius.
                 * \param[in] apex apex of the cone
                 * \param[in] direction direction from the apex to the base
                 * \param[in] radius radius of the base
                 */
                void define(const Vector3d& apex,
                            const Vector3d& direction,
                            float radius);

                /**
                 * \brief Sets apex.
                 * \param[in] apex apex of the cone
                 */
                void setApex(const Vector3d& apex);

                /**
                 * \brief Returns apex.
                 * \return apex of the cone
                 */
                const Vector3d& getApex() const;

                /**
                 * \brief Sets direction.
                 * \param[in] direction direction from the apex to the base
                 */
                void setDirection(const Vector3d& direction);

                /**
                 * \brief Returns direction.
                 * \return direction from the apex to the base
                 */
                const Vector3d& getDirection() const;

                /**
                 * \brief Sets radius.
                 * \param[in] radius radius of the base
                 */
                void setRadius(float radius);

                /**
                 * \brief Returns radius.
                 * \return radius of the base
                 */
                float getRadius() const;

                /**
                 * \brief Determines relation between cone and volume.
                 * \param[in] volume volume
                 * \return OUTSIDE if cone is ouside the volume, INTERSECTS if
                 * cone intersects the volume and INSIDE if cone is inside the volume
                 */
                RELATION determineRelation(const Volume& volume) const;

        private:
                // Cone definition
                Vector3d apex_, direction_;
                float radius_;

        };

}

#endif
