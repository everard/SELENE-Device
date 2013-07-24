// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef UTILITY_H
#define UTILITY_H

#include "../Macros/Macros.h"
#include <fstream>

namespace selene
{

        /**
         * \addtogroup Core
         * @{
         */

        /**
         * Represents utility.
         */
        class Utility
        {
        public:
                /// Helper constants
                enum
                {
                        MAX_STRING_LENGTH = 1024
                };

                Utility();
                Utility(const Utility&) = default;
                ~Utility();
                Utility& operator =(const Utility&) = default;

                /**
                 * \brief Reads string.
                 * \param[in] stream std::istream, from which string is read
                 * \param[out] string c-string
                 * \return true if string has been successfully read
                 */
                static bool readString(std::istream& stream, char* string);

                /**
                 * \brief Writes string.
                 * \param[in] stream std::ostream, to which string shall be written
                 * \param[in] string c-string
                 * \return true if string has been successfully written
                 */
                static bool writeString(std::ostream& stream, const char* string);

                /**
                 * \brief Returns nearest to the given x power-of-two.
                 * \param[in] x value
                 * \return nearest to the given x power-of-two
                 */
                static uint32_t getNearestPowerOfTwo(uint32_t x);

        };

        /**
         * @}
         */

}

#endif
