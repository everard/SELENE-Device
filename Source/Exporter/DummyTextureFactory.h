// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef DUMMY_TEXTURE_FACTORY_H
#define DUMMY_TEXTURE_FACTORY_H

#include "../Engine/Framework.h"

namespace selene
{

        /**
         * Represents dummy texture factory.
         */
        class DummyTextureFactory: public ResourceFactory
        {
        public:
                /**
                 * \brief Constructs dummy texture factory with given file manager.
                 * \param[in] fileManager file manager
                 */
                DummyTextureFactory(FileManager* fileManager = nullptr);
                ~DummyTextureFactory();

                /**
                 * \brief Creates dummy texture with given name.
                 * \param[in] name name of the texture
                 * \return pointer to the created texture, or nullptr if texture
                 * could not be created
                 */
                Resource* createResource(const char* name);

        };

}

#endif
