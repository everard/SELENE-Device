// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "ResourceManager.h"

namespace selene
{

        std::set<ResourceManager*> ResourceManager::resourceManagers_;

        ResourceManager::ResourceManager()
        {
                resourceManagers_.insert(this);
        }
        ResourceManager::~ResourceManager()
        {
                resourceManagers_.erase(this);
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

                // retain resource
                if(!resource->retain())
                        return RESOURCE_COULD_NOT_BE_RETAINED;

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
        bool ResourceManager::retainResources()
        {
                bool result = true;
                for(auto m = resourceManagers_.begin(); m != resourceManagers_.end(); ++m)
                {
                        auto& resources = (*m)->resources_;
                        for(auto it = resources.begin(); it != resources.end(); ++it)
                        {
                                if(!it->second->retain())
                                        result = false;
                        }
                }

                return result;
        }

        //----------------------------------------------------------------------------------------
        void ResourceManager::discardResources()
        {
                for(auto m = resourceManagers_.begin(); m != resourceManagers_.end(); ++m)
                {
                        auto& resources = (*m)->resources_;
                        for(auto it = resources.begin(); it != resources.end(); ++it)
                                it->second->discard();
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
