// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef TEXTURE_FACTORY_H
#define TEXTURE_FACTORY_H

#include "../ResourceFactory.h"
#include "TextureManager.h"

namespace selene
{

        /**
         * Represents texture factory.
         */
        template <class T> class TextureFactory: public ResourceFactory
        {
        public:
                /**
                 * \brief Constructs texture factory with given file manager.
                 * \param[in] fileManager file manager
                 */
                TextureFactory(FileManager* fileManager = nullptr): ResourceFactory(fileManager) {}
                ~TextureFactory() {}

                /**
                 * \brief Creates texture.
                 *
                 * Texture data is loaded from file.
                 * \param[in] name name of the texture (and name of the file from which
                 * texture is loaded)
                 * \return pointer to the created texture, or nullptr if texture
                 * could not be created
                 */
                Resource* createResource(const char* name)
                {
                        // validate
                        if(name == nullptr || fileManager_ == nullptr)
                                return nullptr;

                        std::ifstream stream;

                        // open file
                        if(!fileManager_->open(name, stream))
                                return nullptr;

                        // create texture
                        std::unique_ptr<T> resource(new(std::nothrow) T(name));

                        // validate
                        if(resource.get() == nullptr)
                                return nullptr;

                        // read texture
                        TextureManager textureManager;
                        if(textureManager.readTexture(stream, resource->getData()))
                                return resource.release();

                        return nullptr;
                }

        };

}

#endif
