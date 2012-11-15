// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GLES_TEXTURE_H
#define GLES_TEXTURE_H

#include "../../../../../Engine/Core/Resources/Texture/Texture.h"

namespace selene
{

        // Represents OpenGLES texture.
        class GlesTexture: public Texture
        {
        public:
                GlesTexture(const char* name = nullptr);
                ~GlesTexture();

                // Prepares OpenGLES texture
                bool prepare();

        private:
                friend class GlesRenderer;

        };

}

#endif
