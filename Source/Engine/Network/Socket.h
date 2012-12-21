// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef SOCKET_H
#define SOCKET_H

#include "../Core/Macros/Macros.h"

namespace selene
{

        /**
         * \addtogroup Network
         * @{
         */

        /**
         * Represents socket. This is base class for all sockets.
         */
        class Socket
        {
        public:
                /**
                 * \brief Receives data.
                 * \param[in] buffer buffer which will hold data
                 * \param[in] size size of the buffer (number of bytes to receive)
                 * \return number of bytes received
                 */
                virtual uint32_t receive(uint8_t* buffer, uint32_t size) = 0;

                /**
                 * \brief Sends data.
                 * \param[in] buffer buffer which holds data
                 * \param[in] size size of the buffer
                 * \return true if data was successfully sent
                 */
                virtual bool send(const uint8_t* buffer, uint32_t size) const = 0;

        };

        /**
         * @}
         */

}

#endif
