// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Message.h"
#include <cstring>

namespace selene
{

        Message::Message()
        {
                clear();
                numBytesReceived_ = 0;
        }
        Message::~Message() {}

        //------------------------------------------------------
        uint8_t* Message::getData() const
        {
                return const_cast<uint8_t*>(data_ + MESSAGE_HEADER_SIZE);
        }

        //------------------------------------------------------
        uint16_t Message::getType() const
        {
                if(position_ > 0)
                        return BAD_MESSAGE_TYPE;

                return type_;
        }

        //------------------------------------------------------
        uint16_t Message::getSize() const
        {
                if(position_ > 0)
                        return 0;

                return size_;
        }

        //------------------------------------------------------
        uint16_t Message::getNumBytesReceived() const
        {
                return numBytesReceived_;
        }

        //------------------------------------------------------
        bool Message::receive(uint8_t* buffer, uint16_t size)
        {
                numBytesReceived_ = 0;

                // validate
                if(size == 0 || buffer == nullptr)
                        return false;

                uint16_t numBytesLeftToReceive = 0;

                // still need to read header
                if(position_ < MESSAGE_HEADER_SIZE)
                {
                        numBytesLeftToReceive = MESSAGE_HEADER_SIZE - position_;

                        if(size < numBytesLeftToReceive)
                        {
                                // read header part
                                memcpy(data_ + position_, buffer, size);
                                position_ += size;
                                numBytesReceived_ = size;
                                return false;
                        }

                        // read header
                        memcpy(data_ + position_, buffer, numBytesLeftToReceive);

                        position_ += numBytesLeftToReceive;
                        buffer += numBytesLeftToReceive;
                        size   -= numBytesLeftToReceive;
                        numBytesReceived_ += numBytesLeftToReceive;

                        memcpy(&type_, data_,  sizeof(type_));
                        memcpy(&size_, data_ + sizeof(type_), sizeof(size_));

                        if(size_ > MAX_MESSAGE_SIZE)
                        {
                                clear();
                                return true;
                        }
                        else if(size_ == 0)
                        {
                                position_ = 0;
                                return true;
                        }
                }

                numBytesLeftToReceive = size_ + MESSAGE_HEADER_SIZE - position_;

                if(size < numBytesLeftToReceive)
                {
                        memcpy(data_ + position_, buffer, size);
                        position_ += size;
                        numBytesReceived_ += size;
                        return false;
                }

                memcpy(data_ + position_, buffer, numBytesLeftToReceive);
                numBytesReceived_ += numBytesLeftToReceive;
                position_ = 0;

                return true;
        }

        //------------------------------------------------------
        bool Message::send(const Socket& socket, uint16_t type,
                           const uint8_t* buffer, uint16_t size)
        {
                if(size > MAX_MESSAGE_SIZE || (size > 0 && buffer == nullptr))
                        return false;

                uint8_t data[MESSAGE_HEADER_SIZE + MAX_MESSAGE_SIZE];
                memcpy(data, &type, sizeof(type));
                memcpy(data + sizeof(type), &size, sizeof(size));

                if(size > 0)
                        memcpy(data + MESSAGE_HEADER_SIZE, buffer, size);

                return socket.send(data, size + MESSAGE_HEADER_SIZE);
        }

        //------------------------------------------------------
        void Message::clear()
        {
                type_     = BAD_MESSAGE_TYPE;
                size_     = 0;
                position_ = 0;
        }

}
