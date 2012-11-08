// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLTexture.h"

namespace selene
{

        GlTexture::GlTexture(const char* name): Texture(name) {}
        GlTexture::~GlTexture() {}

        //-----------------------
        bool GlTexture::prepare()
        {
                return true;
        }

}
