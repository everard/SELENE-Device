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

        /// Texture formats
        enum
        {
                // Texture formats
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
                 * Represents texture data container.
                 */
                class Data
                {
                public:
                        // Pixels
                        Array<uint8_t, uint32_t> pixels;

                        // Width and height
                        uint32_t width, height;

                        // Number of mip maps
                        uint32_t numMipMaps;

                        // Texture format and bytes per pixel
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
                 * \brief Prepares texture.
                 * \return true if texture was successfully prepared for rendering
                 */
                bool prepare();

                /**
                 * \brief Returns texture data.
                 * \return reference to the texture data
                 */
                Data& getData();

        protected:
                // Data
                Data data_;

        };

}

#endif
