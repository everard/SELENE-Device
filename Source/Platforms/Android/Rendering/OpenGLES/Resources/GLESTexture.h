// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GLES_TEXTURE_H
#define GLES_TEXTURE_H

#include "../../../../../Engine/Core/Resources/Texture/Texture.h"

namespace selene
{

        // Represents OpenGL ES texture.
        class GlesTexture: public Texture
        {
        public:
                GlesTexture(const char* name = nullptr);
                ~GlesTexture();

                // Retains OpenGL ES texture
                bool retain();

                // Discards OpenGL ES texture
                void discard();

        private:
                friend class GlesRenderer;

        };

}

#endif
