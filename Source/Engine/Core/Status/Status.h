// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef STATUS_H
#define STATUS_H

#include "../Macros/Macros.h"

namespace selene
{

        /**
         * Represents status.
         */
        class Status
        {
        public:
                /**
                 * \brief Constructs status object with given flags.
                 * \param[in] flags status flags
                 */
                Status(uint8_t flags = 0);
                virtual ~Status();

                /**
                 * \brief Sets flags.
                 *
                 * Performs bitwise OR operation on current status flags and executes onChange callback.
                 * \param[in] flags flags which will be set
                 */
                void setFlags(uint8_t flags);

                /**
                 * \brief Clears flags.
                 *
                 * Performs bitwise NOT operation on input parameter, then applies bitwise AND operation
                 * on current status flags and executes onChange callback.
                 * \param[in] flags flags which will be cleared
                 */
                void clearFlags(uint8_t flags);

                /**
                 * \brief Returns flags.
                 * \return current status flags
                 */
                uint8_t getFlags() const;

                /**
                 * \brief Returns true if specified flags are set.
                 * \param[in] flags flags which are checked
                 * \return true if all specified flags are currently set
                 */
                bool is(uint8_t flags) const;

        protected:
                /**
                 * \brief Status change event callback.
                 */
                virtual void onChange();

                /**
                 * \brief Sets flags.
                 *
                 * Performs bitwise OR operation on current status flags (onChange callback is not executed).
                 * \param[in] flags flags which will be set
                 */
                void setFlags(uint8_t flags) const;

                /**
                 * \brief Clears flags.
                 *
                 * Performs bitwise NOT operation on input parameter, then applies bitwise AND operation on
                 * current status flags (onChange callback is not executed).
                 * \param[in] flags flags which will be cleared
                 */
                void clearFlags(uint8_t flags) const;

        private:
                // Flags
                mutable uint8_t flags_;

        };

}

#endif
