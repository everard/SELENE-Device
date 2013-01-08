// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLESTexture.h"

namespace selene
{

        GlesTexture::GlesTexture(const char* name): Texture(name) {}
        GlesTexture::~GlesTexture() {}

        //----------------------------
        bool GlesTexture::retain()
        {
                return true;
        }

        //----------------------------
        void GlesTexture::discard() {}

}
