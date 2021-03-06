// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Utility.h"

namespace selene
{

        //-----------------------------------------------------------------------------------------
        bool Utility::StringComparator::operator ()(const char* string0, const char* string1) const
        {
                return (std::strcmp(string0, string1) < 0);
        }

        Utility::Utility() {}
        Utility::~Utility() {}

        //-----------------------------------------------------------------------------------------
        bool Utility::readString(std::istream& stream, char* string)
        {
                if(!stream.good())
                        return false;

                uint16_t length = 0;

                // read length
                stream.read(reinterpret_cast<char*>(&length), sizeof(uint16_t));

                // read string
                if(length != 0 && length < MAX_STRING_LENGTH)
                {
                        stream.read(string, length);
                        string[length] = 0;
                        return true;
                }

                stream.seekg(length, std::ios_base::cur);
                string[0] = 0;
                return false;
        }

        //-----------------------------------------------------------------------------------------
        bool Utility::writeString(std::ostream& stream, const char* string)
        {
                if(!stream.good())
                        return false;

                uint16_t length = 0;

                if(string == nullptr)
                {
                        stream.write(reinterpret_cast<char*>(&length), sizeof(uint16_t));
                        return false;
                }

                // get string length
                size_t stringLength = std::strlen(string);
                length = (stringLength >= MAX_STRING_LENGTH) ? (MAX_STRING_LENGTH - 1) :
                         (static_cast<uint16_t>(stringLength));

                // write string length
                stream.write(reinterpret_cast<char*>(&length), sizeof(uint16_t));
                if(length == 0)
                        return false;

                // write string
                stream.write(string, length);

                return true;
        }

        //-----------------------------------------------------------------------------------------
        uint32_t Utility::getNearestPowerOfTwo(uint32_t x)
        {
                --x;
                x |= x >> 1;
                x |= x >> 2;
                x |= x >> 4;
                x |= x >> 8;
                x |= x >> 16;
                return ++x;
        }

}
