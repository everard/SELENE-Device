// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef RESOURCE_FACTORY_H
#define RESOURCE_FACTORY_H

#include "../FileManager/FileManager.h"
#include <memory>

namespace selene
{

        // Forward declaration of classes
        class ResourceManager;
        class Resource;

        /**
         * Represents resource factory. This is base class for all resource factories.
         */
        class ResourceFactory
        {
        public:
                /**
                 * \brief Constructs resource factory with given file manager.
                 * \param[in] fileManager file manager
                 */
                ResourceFactory(FileManager* fileManager = nullptr);
                virtual ~ResourceFactory();

                /**
                 * \brief Sets file manager.
                 * \param[in] fileManager file manager
                 */
                void setFileManager(FileManager* fileManager);

                /**
                 * \brief Sets resource manager.
                 *
                 * Resource manager might be needed if current factory creates
                 * more than one resource.
                 * \param[in] resourceManager resource manager
                 */
                void setResourceManager(ResourceManager* resourceManager);

                /**
                 * \brief Sets resource factory.
                 *
                 * Another resource factory might be needed if current factory
                 * creates resources which use another type of resources.
                 * \param[in] resourceFactory resource factory
                 */
                void setResourceFactory(ResourceFactory* resourceFactory);

                /**
                 * \brief Creates resource.
                 * \param[in] name name of the resource
                 * \return pointer to the created resource, or nullptr if resource
                 * could not be created
                 */
                virtual Resource* createResource(const char* name) = 0;

        protected:
                // File manager
                FileManager* fileManager_;

                // Resource manager
                ResourceManager* resourceManager_;

                // Resource factory
                ResourceFactory* resourceFactory_;

        };

}

#endif
