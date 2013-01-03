// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef DUMMY_TEXTURE_FACTORY_H
#define DUMMY_TEXTURE_FACTORY_H

#include "../Engine/Framework.h"

namespace selene
{

        /**
         * \addtogroup Exporter
         * \brief Application, which exports geometry to the SLE model format.
         *
         * Its input is RawMeshData, which is loaded from file. RawMeshData is then processed in Exporter, which
         * saves the result as SLE model.
         * @{
         */

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

        /**
         * @}
         */

}

#endif
