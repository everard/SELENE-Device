// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Texture.h"

namespace selene
{

        Texture::Data::Data()
        {
                width = height = 0;
                format = bpp = 0;
                numMipMaps = 0;
        }
        Texture::Data::~Data() {}

        Texture::Texture(const char* name): Resource(name) {}
        Texture::~Texture() {}

        //-------------------------------
        bool Texture::prepare()
        {
                return true;
        }

        //-------------------------------
        Texture::Data& Texture::getData()
        {
                return data_;
        }

}
