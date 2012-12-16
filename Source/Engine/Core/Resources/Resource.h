// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef RESOURCE_H
#define RESOURCE_H

#include "../Entity/Entity.h"
#include <memory>

namespace selene
{

        /**
         * \addtogroup Resources
         * @{
         */

        /**
         * Represents resource. This is base class for all resources (textures, meshes, etc.).
         * Resource may be created and destroyed ONLY with ResourceManager. The only valid way to
         * obtain the resource is to request it from the ResourceManager, which possesses it. In
         * this case Resource::Instance object is returned. Note, that when resource is requested,
         * its type is automatically checked. This behavior improves program stability.
         *
         * To create resource, specific ResourceFactory must be used. For example, to create texture,
         * TextureFactory can be used.
         * \see ResourceManager for more info.
         * \see Resource::Instance
         */
        class Resource: public Entity
        {
        public:
                /**
                 * Represents resource instance. When resource is destroyed somewhere else, instance
                 * is automatically notified that resource no longer exists.
                 */
                template <class T> class Instance
                {
                public:
                        /**
                         * \brief Constructs resource instance with given pointer to the resource.
                         *
                         * Pointer to the base resource class is dynamically casted to the pointer
                         * to the resource of derived class (T).
                         * \param[in] sharedPointer std::shared_ptr to the resource
                         */
                        Instance(const std::shared_ptr<Resource>& sharedPointer)
                        {
                                resource_ = nullptr;

                                initialize(dynamic_cast<T*>(sharedPointer.get()));

                                if(resource_ != nullptr)
                                        weakPointer_ = sharedPointer;
                        }
                        Instance(const Instance<T>& instance)
                        {
                                *this = instance;
                        }
                        Instance()
                        {
                                resource_ = nullptr;
                        }
                        ~Instance()
                        {
                                if(weakPointer_.expired())
                                        return;

                                resource_->release();
                        }

                        /**
                         * \brief Assignes resource instance.
                         *
                         * Current resource is released and replaced by another resource, which is then requested.
                         * \param[in] instance another resource instance which will be assigned to current
                         * \return reference to the resource instance
                         */
                        Instance<T>& operator =(const Instance<T>& instance)
                        {
                                if(!weakPointer_.expired())
                                {
                                        weakPointer_.reset();
                                        resource_->release();
                                }

                                resource_ = nullptr;

                                if(!instance.weakPointer_.expired())
                                {
                                        initialize(instance.resource_);

                                        if(resource_ != nullptr)
                                                weakPointer_ = instance.weakPointer_;
                                }

                                return *this;
                        }

                        /**
                         * \brief Returns resource.
                         * \return pointer to the resource
                         */
                        T* operator *() const
                        {
                                if(!weakPointer_.expired())
                                        return resource_;

                                return nullptr;
                        }

                private:
                        std::weak_ptr<Resource> weakPointer_;
                        T* resource_;

                        /**
                         * \brief Initializes resource instance.
                         * \param[in] resource pointer to the resource
                         */
                        void initialize(T* resource)
                        {
                                if(resource == nullptr)
                                        return;

                                if(!resource->request())
                                        return;

                                resource_ = resource;
                        }

                };

                /**
                 * \brief Constructs resource with given name.
                 * \param[in] name name of the resource
                 */
                Resource(const char* name);
                virtual ~Resource();

                /**
                 * \brief Returns true if resource is used.
                 * \return true if resource is used
                 */
                bool isUsed() const;

                /**
                 * \brief Prepares resource.
                 * \return true if resource was successfully prepared for use in
                 * corresponding API
                 */
                virtual bool prepare() = 0;

        private:
                uint32_t numRequests_;

                /**
                 * \brief Requests resource.
                 * \return true if request was granted
                 */
                bool request();

                /**
                 * \brief Releases resource.
                 */
                void release();

        };

        /**
         * @}
         */

}

#endif
