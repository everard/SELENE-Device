// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "ResourceFactory.h"
#include "ResourceManager.h"

namespace selene
{

        ResourceFactory::ResourceFactory(FileManager* fileManager)
        {
                setFileManager(fileManager);
                resourceManager_ = nullptr;
                resourceFactory_ = nullptr;
        }
        ResourceFactory::~ResourceFactory() {}

        //------------------------------------------------------------------------
        void ResourceFactory::setFileManager(FileManager* fileManager)
        {
                fileManager_ = fileManager;
        }

        //------------------------------------------------------------------------
        void ResourceFactory::setResourceManager(ResourceManager* resourceManager)
        {
                resourceManager_ = resourceManager;
        }

        //------------------------------------------------------------------------
        void ResourceFactory::setResourceFactory(ResourceFactory* resourceFactory)
        {
                resourceFactory_ = resourceFactory;
        }

}
