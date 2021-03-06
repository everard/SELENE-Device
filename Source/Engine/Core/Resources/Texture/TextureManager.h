// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "TextureFormat.h"

namespace selene
{

        /**
         * \addtogroup Resources
         * @{
         */

        /**
         * Represents texture manager. Reads DDS texture from std::istream.
         */
        class TextureManager
        {
        public:
                TextureManager();
                TextureManager(const TextureManager&) = default;
                ~TextureManager();
                TextureManager& operator =(const TextureManager&) = default;

                /**
                 * \brief Reads texture.
                 * \param[in] stream std::istream from which texture data is read
                 * \param[out] textureData texture data
                 * \return true if texture has been successfully read
                 */
                bool readTexture(std::istream& stream, Texture::Data& textureData);

        private:
                uint32_t totalSize_, compressionFactor_;
                DdsHeader ddsHeader_;
                bool isDxt_;

                /**
                 * \brief Reads header.
                 * \param[in] stream std::istream from which header is read
                 * \param[out] textureData texture data
                 * \return true if texture header has been successfully read
                 */
                bool readHeader(std::istream& stream, Texture::Data& textureData);

                /**
                 * \brief Gets texture properties.
                 *
                 * Properties are retrieved from header and checked.
                 * \param[out] textureData texture data
                 * \return true if texture properties have been successfully retrieved
                 */
                bool getProperties(Texture::Data& textureData);

                /**
                 * \brief Computes total size of texture.
                 *
                 * Also allocates memory for pixels.
                 * \param[out] textureData texture data
                 * \return true if total size has been computed and memory for pixels has been
                 * successfully allocated
                 */
                bool computeTotalSize(Texture::Data& textureData);

                /**
                 * \brief Reads pixels.
                 * \param[in] stream std::istream from which pixels are read
                 * \param[out] textureData texture data
                 * \return true if pixels have been successfully read
                 */
                bool readPixels(std::istream& stream, Texture::Data& textureData);

        };

        /**
         * @}
         */

}

#endif
