// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GLES_TEXTURE_H
#define GLES_TEXTURE_H

#include "../../../../../Engine/Core/Resources/Texture/Texture.h"
#include <GLES2/gl2.h>

namespace selene
{

        /**
         * \addtogroup Android
         * @{
         */

        /**
         * Represents OpenGL ES texture.
         */
        class GlesTexture: public Texture
        {
        public:
                /**
                 * \brief Constructs OpenGL ES texture with given name.
                 * \param[in] name name of the texture
                 */
                GlesTexture(const char* name = nullptr);
                GlesTexture(const GlesTexture&) = delete;
                ~GlesTexture();
                GlesTexture& operator =(const GlesTexture&) = delete;

                // Texture interface implementation
                bool retain();
                void discard();

        private:
                friend class GlesTextureHandler;

                GLuint texture_;

                /**
                 * \brief Destroys OpenGL ES texture data.
                 */
                void destroy();

        };

        /**
         * @}
         */

}

#endif
