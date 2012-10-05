// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "ResourceManager.h"

namespace selene
{

        ResourceManager::ResourceManager() {}
        ResourceManager::~ResourceManager()
        {
                destroyResources(true);
        }

        //----------------------------------------------------------------------------------------
        size_t ResourceManager::getNumResources()
        {
                return resources_.size();
        }

        //----------------------------------------------------------------------------------------
        RESULT ResourceManager::createResource(const char* name, ResourceFactory& resourceFactory)
        {
                // validate
                if(name == nullptr)
                        return FAIL;

                if(getResource(name))
                        return RESOURCE_ALREADY_EXISTS;

                // create resource
                std::unique_ptr<Resource> resource(resourceFactory.createResource(name));
                if(resource.get() == nullptr)
                        return FAIL;

                // prepare resource
                if(!resource->prepare())
                        return PREPARATION_FAILED;

                // store resource
                std::string key = resource->getName();
                auto result = resources_.insert(std::make_pair(key,
                                                std::shared_ptr<Resource>(resource.release())));

                if(!result.second)
                        return FAIL;

                return SUCCESS;
        }

        //----------------------------------------------------------------------------------------
        RESULT ResourceManager::destroyResource(const char* name, bool forced)
        {
                // validate
                if(name == nullptr)
                        return FAIL;

                // find
                auto it = resources_.find(std::string(name));

                if(it == resources_.end())
                        return FAIL;

                // destroy
                if(!it->second->isUsed() || forced)
                        resources_.erase(it);
                else
                        return RESOURCE_IS_USED;

                return SUCCESS;
        }

        //----------------------------------------------------------------------------------------
        void ResourceManager::destroyResources(bool forced)
        {
                if(forced)
                        resources_.clear();
                else
                {
                        auto it = resources_.begin();
                        while(it != resources_.end())
                        {
                                if(!it->second->isUsed())
                                        it = resources_.erase(it);
                                else
                                        ++it;
                        }
                }
        }

        //----------------------------------------------------------------------------------------
        const std::shared_ptr<Resource>& ResourceManager::getResource(const char* name)
        {
                // validate
                if(name == nullptr)
                        return nullSharedPointer_;

                // find
                auto it = resources_.find(std::string(name));

                if(it == resources_.end())
                        return nullSharedPointer_;

                // return resource
                return it->second;
        }

}
