// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef MESSAGE_H
#define MESSAGE_H

#include "Socket.h"

namespace selene
{

        /**
         * \addtogroup Network
         * \brief Sockets and messages.
         * @{
         */

        /**
         * Represents message. This message is used for client-server communications via TCP.
         * Message consists of type (2 bytes), size (2 bytes), and data. Checksum is not used,
         * because TCP already guarantees package delivery. Type has application-defined meaning.
         *
         * Message can be received with multiple pieces. In this case return value of Message::receive
         * method should be checked. If it returns true, then message is completely received.
         *
         * Simple example:
         * \code
         * // some declarations:
         * // we have socket, which receives raw data via TCP, and message
         * // of course, actual socket implementation must be used instead of
         * // the pure virtual class (because the latter can not define objects)
         * selene::Socket socket;
         * selene::Message message;
         *
         * // we also have buffer, which holds raw data, and size of the received data:
         * uint8_t buffer[BUFFER_SIZE];
         * uint16_t numBytesReceived = 0;
         *
         * ...
         * // somewhere in program we receive data through socket
         * numBytesReceived = socket.receive(buffer, BUFFER_SIZE);
         * // and from raw data we get our message
         * if(message.receive(buffer, numBytesReceived))
         * {
         *         // message received complitely, now we can use it somehow
         *         doSomething(message);
         * }
         * \endcode
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
                uint8_t data_[MESSAGE_HEADER_SIZE + MAX_MESSAGE_SIZE];
                uint16_t type_;
                uint16_t size_;
                uint16_t position_;
                uint16_t numBytesReceived_;

                /**
                 * \brief Clears message.
                 */
                void clear();

        };

        /**
         * @}
         */

}

#endif
