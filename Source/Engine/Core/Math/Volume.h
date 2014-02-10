// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef VOLUME_H
#define VOLUME_H

#include "Plane.h"

namespace selene
{

        /**
         * \addtogroup Math
         * @{
         */

        // Forward declaration of classes
        class Matrix;

        /**
         * Represents volume in 3D space.
         */
        class Volume
        {
        public:
                /**
                 * \brief Constructs volume from given bounding planes.
                 * \param[in] planes bounding planes
                 * \param[in] numPlanes number of bounding planes
                 */
                Volume(const Plane* planes = nullptr, uint8_t numPlanes = 0);
                Volume(const Volume&) = default;
                ~Volume();
                Volume& operator =(const Volume&) = default;

                /**
                 * \brief Defines volume with given planes.
                 *
                 * If number of planes exceeds maximum, this number is set to zero and
                 * volume becomes infinite (has no bounds).
                 * \param[in] planes bounding planes
                 * \param[in] numPlanes number of bounding planes
                 */
                void define(const Plane* planes, uint8_t numPlanes);

                /**
                 * \brief Defines volume with given view-projection matrix.
                 *
                 * Builds bounding planes from view-projection frustum.
                 * \param[in] viewProjectionMatrix view-projection matrix
                 */
                void define(const Matrix& viewProjectionMatrix);

                /**
                 * \brief Returns number of planes.
                 * \return number of planes
                 */
                uint8_t getNumPlanes() const;

                /**
                 * \brief Returns array of planes.
                 * \return pointer to the first element in the array of bounding planes
                 */
                const Plane* getPlanes() const;

        private:
                /// Helper constants
                enum
                {
                        MAX_NUM_OF_PLANES = 16
                };

                Plane planes_[MAX_NUM_OF_PLANES];
                uint8_t numPlanes_;

        };

        /**
         * @}
         */

}

#endif
