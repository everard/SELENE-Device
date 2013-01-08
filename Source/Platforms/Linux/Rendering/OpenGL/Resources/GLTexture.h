// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GL_TEXTURE_H
#define GL_TEXTURE_H

#include "../../../../../Engine/Core/Resources/Texture/Texture.h"

namespace selene
{

        // Represents OpenGL texture.
        class GlTexture: public Texture
        {
        public:
                GlTexture(const char* name = nullptr);
                ~GlTexture();

                // Retains OpenGL texture
                bool retain();

                // Discards OpenGL texture
                void discard();

        private:
                friend class GlRenderer;

        };

}

#endif
