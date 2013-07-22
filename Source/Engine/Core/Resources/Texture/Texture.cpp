// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Texture.h"

namespace selene
{

        Texture::Data::Data():
                pixels(), width(0), height(0),
                numMipMaps(0), format(0), bpp(0) {}
        Texture::Data::~Data() {}

        Texture::Texture(const char* name): Resource(name), data_() {}
        Texture::~Texture() {}

        //------------------------------------------------------------
        Texture::Data& Texture::getData()
        {
                return data_;
        }

}
