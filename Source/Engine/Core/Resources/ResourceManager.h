// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "ResourceFactory.h"
#include "Resource.h"

#include <unordered_map>
#include <set>

namespace selene
{

        /**
         * \addtogroup Resources
         * @{
         */

        /// Result of the ResourceManager's operation
        enum RESULT
        {
                FAIL = 0,
                SUCCESS,
                RESOURCE_IS_USED,
                RESOURCE_ALREADY_EXISTS,
                RESOURCE_COULD_NOT_BE_RETAINED
        };

        /**
         * Represents resource manager. Handles resource creation and destruction.
         * Creates resource instances when requested.
         * The following code shows basic resource life cycle in the application (in this example texture
         * is used):
         * \code
         * // create texture
         * selene::TextureFactory<selene::Texture> textureFactory(&fileManager);
         * selene::ResourceManager resourceManager;
         *
         * if(resourceManager.createResource("brick.dds", textureFactory) != SUCCESS)
         * {
         *         handleError();
         * }
         *
         * ...
         *
         * // get resource instance
         * auto textureInstance = resourceManager.requestResource<selene::Texture>("brick.dds");
         * // now to access actual texture (*textureInstance) can be used
         *
         * // note, that if requested resource can not be found or has different type (not Texture),
         * // then (*textureInstance) will be equal to nullptr
         *
         * ...
         *
         * // destroy resource
         * resourceManager.destroyResource("brick.dds");
         * // if you want to destroy resource, even if it is used somewhere, force its destruction:
         * resourceManager.destroyResource("brick.dds", true);
         * \endcode
         */
        class ResourceManager
        {
        public:
                ResourceManager();
                ResourceManager(const ResourceManager&) = delete;
                ~ResourceManager();
                ResourceManager& operator =(const ResourceManager&) = delete;

                /**
                 * \brief Returns number of resources.
                 * \return number of resources
                 */
                size_t getNumResources();

                /**
                 * \brief Creates resource.
                 * \param[in] name name of the resource
                 * \param[in] resourceFactory resource factory
                 * \return SUCCESS if resource successfully created, FAIL if resource could not be
                 * created, RESOURCE_ALREADY_EXISTS if resource with given name already exists,
                 * RESOURCE_COULD_NOT_BE_RETAINED if resource could not be retained for use in
                 * corresponding subsystem.
                 */
                RESULT createResource(const char* name, ResourceFactory& resourceFactory);

                /**
                 * \brief Destroys resource.
                 * \param[in] name name of the resource
                 * \param[in] forced flag, which forces destruction of resource
                 * \return SUCCESS if resource successfully destroyed, FAIL if resource with given
                 * name does not exist, RESOURCE_IS_USED if resource with given name is used and
                 * destruction is not forced
                 */
                RESULT destroyResource(const char* name, bool forced = false);

                /**
                 * \brief Destroys resources.
                 * \param[in] forced flag, which forces destruction of used resources
                 */
                void destroyResources(bool forced = false);

                /**
                 * \brief Returns resource instance.
                 * \param[in] name name of the resource
                 * \return resource instance
                 */
                template <class T> Resource::Instance<T> requestResource(const char* name)
                {
                        return Resource::Instance<T>(getResource(name));
                }

                /**
                 * \brief Retains all resources in application.
                 * \see Resource::retain
                 * \return true if all resources have been successfully retained
                 */
                static bool retainResources();

                /**
                 * \brief Discards all resources in application.
                 * \see Resource::discard
                 */
                static void discardResources();

        private:
                std::unordered_map<std::string, std::shared_ptr<Resource>> resources_;
                std::shared_ptr<Resource> nullSharedPointer_;
                bool isInitialized_;

                static std::set<ResourceManager*> resourceManagers_;

                /**
                 * \brief Returns resource.
                 * \param[in] name name of the resource
                 * \return reference to the std::shared_ptr to the resource
                 */
                const std::shared_ptr<Resource>& getResource(const char* name);

        };

        /**
         * @}
         */

}

#endif
