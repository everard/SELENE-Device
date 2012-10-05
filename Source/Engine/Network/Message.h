// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef MESSAGE_H
#define MESSAGE_H

#include "Socket.h"

namespace selene
{

        /**
         * Represents message. This message is used for client-server communications.
         * Message consists of type (2 bytes), size (2 bytes), and data.
         */
        class Message
        {
        public:
                /// Message constants
                enum
                {
                        MESSAGE_HEADER_SIZE = 4,
                        MAX_MESSAGE_SIZE = 2044,
                        BAD_MESSAGE_TYPE = 0xFFFF
                };

                Message();
                ~Message();

                /**
                 * \brief Returns data.
                 * \return pointer to the message data
                 */
                uint8_t* getData() const;

                /**
                 * \brief Returns type.
                 * \return message type
                 */
                uint16_t getType() const;

                /**
                 * \brief Returns size.
                 * \return size of the message in bytes
                 */
                uint16_t getSize() const;

                /**
                 * \brief Returns number of bytes received during last attempt.
                 * \see receive
                 * \return number of bytes received during last attempt
                 */
                uint16_t getNumBytesReceived() const;

                /**
                 * \brief Receives message.
                 *
                 * It is possible to receive message with multiple fragments, i.e. through
                 * multiple calls to this function. This function will return true when message
                 * is completely received.
                 * \param[in] buffer buffer from which message is received
                 * \param[in] size size of the buffer
                 * \return true if message was completely received
                 */
                bool receive(uint8_t* buffer, uint16_t size);

                /**
                 * \brief Sends message via socket.
                 * \param[in] socket socket
                 * \param[in] type type of the message
                 * \param[in] buffer buffer which contains message data
                 * \param[in] size size of the message
                 * \return true if message was successfully sent
                 */
                static bool send(const Socket& socket, uint16_t type,
                                 const uint8_t* buffer = nullptr, uint16_t size = 0);

        private:
                // Data
                uint8_t data_[MESSAGE_HEADER_SIZE + MAX_MESSAGE_SIZE];

                // Type
                uint16_t type_;

                // Size
                uint16_t size_;

                // Position
                uint16_t position_;

                // Number of bytes received during last attempt
                uint16_t numBytesReceived_;

                /**
                 * \brief Clears message.
                 */
                void clear();

        };

}

#endif
