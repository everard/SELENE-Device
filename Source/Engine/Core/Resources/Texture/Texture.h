// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef TEXTURE_H
#define TEXTURE_H

#include "../../FileManager/FileManager.h"
#include "../../Helpers/Array.h"
#include "../ResourceManager.h"
#include "../Resource.h"

namespace selene
{

        /**
         * \addtogroup Resources
         * @{
         */

        /// Texture format
        enum TEXTURE_FORMAT
        {
                TEXTURE_FORMAT_DXT1 = 0,
                TEXTURE_FORMAT_DXT3,
                TEXTURE_FORMAT_DXT5,
                TEXTURE_FORMAT_NOT_COMPRESSED
        };

        /**
         * Represents texture.
         */
        class Texture: public Resource
        {
        public:
                /**
                 * Represents texture data container. Includes:
                 * - array of bytes, which contains pixels,
                 * - width and height of the texture,
                 * - number of mip maps,
                 * - format of texture (one of the selene::TEXTURE_FORMAT),
                 * - bytes per pixel (bpp) for not compressed texture.
                 */
                class Data
                {
                public:
                        Array<uint8_t, uint32_t> pixels;
                        uint32_t width, height;
                        uint32_t numMipMaps;
                        uint8_t format, bpp;

                        Data();
                        ~Data();

                };

                /**
                 * \brief Constructs texture with given name.
                 * \param[in] name name of the texture
                 */
                Texture(const char* name);
                ~Texture();

                /**
                 * \brief Returns texture data.
                 * \return reference to the texture data
                 */
                Data& getData();

        protected:
                Data data_;

        };

        /**
         * @}
         */

}

#endif
