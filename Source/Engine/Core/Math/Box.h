// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef BOX_H
#define BOX_H

#include "Vector.h"
#include "Volume.h"
#include "Sphere.h"

namespace selene
{

        /**
         * \addtogroup Math
         * \brief Linear algebra and vector geometry.
         * @{
         */

        /**
         * Represents box in 3D space.
         */
        class Box
        {
        public:
                /// Helper constants
                enum
                {
                        NUM_OF_VERTICES = 8
                };

                /**
                 * \brief Constructs box with given center, width, height and depth.
                 * \param[in] center center of the box
                 * \param[in] width width of the box
                 * \param[in] height height of the box
                 * \param[in] depth depth of the box
                 */
                Box(const Vector3d& center = Vector3d(), float width = 1.0f,
                    float height = 1.0f, float depth = 1.0f);
                /**
                 * \brief Constructs box with given eight vertices.
                 * \param[in] vertices array of the vertices of the box
                 */
                Box(const Vector3d* vertices);
                Box(const Box&) = default;
                ~Box();
                Box& operator =(const Box&) = default;

                /**
                 * \brief Defines box with given center, width, height and depth.
                 * \param[in] center center of the box
                 * \param[in] width width of the box
                 * \param[in] height height of the box
                 * \param[in] depth depth of the box
                 */
                void define(const Vector3d& center, float width, float height, float depth);

                /**
                 * \brief Defines box with given eight vertices.
                 * \param[in] vertices array of the vertices of the box
                 */
                void define(const Vector3d* vertices);

                /**
                 * \brief Returns vertices (box has eight vertices).
                 * \return array of the vertices of the box
                 */
                const Vector3d* getVertices() const;

                /**
                 * \brief Determines relation between box and volume.
                 * \param[in] volume volume
                 * \return OUTSIDE if box is outside the volume, INTERSECTS if
                 * box intersects the volume and INSIDE if box is inside the volume
                 */
                RELATION determineRelation(const Volume& volume) const;

                /**
                 * \brief Transforms box.
                 * \param[in] matrix transformation matrix
                 */
                void transform(const Matrix& matrix);

        private:
                Vector3d vertices_[NUM_OF_VERTICES];

        };

        /**
         * @}
         */

}

#endif
