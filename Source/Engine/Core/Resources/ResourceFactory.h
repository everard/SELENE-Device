// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef RESOURCE_FACTORY_H
#define RESOURCE_FACTORY_H

#include "../FileManager/FileManager.h"
#include <memory>

namespace selene
{

        /**
         * \addtogroup Resources
         * @{
         */

        // Forward declaration of classes
        class ResourceManager;
        class Resource;

        /**
         * Represents resource factory. This is base class for all resource factories.
         * The actual factory must implement ResourceFactory::createResource method.
         * This method must return valid pointer to the actual resource (be it texture,
         * mesh or whatever) if it has been successfully created, or nullptr otherwise.
         *
         * \see ResourceManager for usage example (of TextureFactory in that particular case).
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
                FileManager* fileManager_;
                ResourceManager* resourceManager_;
                ResourceFactory* resourceFactory_;

        };

        /**
         * @}
         */

}

#endif
