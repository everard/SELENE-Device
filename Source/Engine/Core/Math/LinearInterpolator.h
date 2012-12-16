// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef LINEAR_INTERPOLATOR_H
#define LINEAR_INTERPOLATOR_H

#include "../Macros/Macros.h"

namespace selene
{

        /**
         * \addtogroup Math
         * @{
         */

        /**
         * Represents linear interpolator.
         */
        template <class T> class LinearInterpolator
        {
        public:
                LinearInterpolator() {}
                ~LinearInterpolator() {}

                /**
                 * \brief Sets value.
                 * \param[in] value value of interpolation point
                 * \param[in] index index of interpolation point
                 */
                void setValue(const T& value, uint32_t index)
                {
                        if(index >= NUM_OF_VALUES)
                                return;

                        values_[index] = value;
                }

                /**
                 * \brief Returns value.
                 * \param[in] index index of interpolation point
                 * \return value of interpolation point
                 */
                const T& getValue(uint32_t index) const
                {
                        if(index >= NUM_OF_VALUES)
                                return values_[0];

                        return values_[index];
                }

                /**
                 * \brief Returns interpolated value.
                 * \param[in] scalar interpolation amount (float in [0; 1] range)
                 * \return result of interpolation
                 */
                T getInterpolatedValue(float scalar) const
                {
                        if(scalar <= 0.0f)
                                return values_[0];

                        if(scalar >= 1.0f)
                                return values_[1];

                        return (values_[0] - scalar * (values_[0] - values_[1]));
                }

        private:
                enum
                {
                        NUM_OF_VALUES = 2
                };

                T values_[NUM_OF_VALUES];

        };

        /**
         * @}
         */

}

#endif
