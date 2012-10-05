// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "DummyTextureFactory.h"

namespace selene
{

        DummyTextureFactory::DummyTextureFactory(FileManager* fileManager): ResourceFactory(fileManager) {}
        DummyTextureFactory::~DummyTextureFactory() {}

        //-------------------------------------------------------------
        Resource* DummyTextureFactory::createResource(const char* name)
        {
                if(name == nullptr)
                        return nullptr;

                return new(std::nothrow) Texture(name);
        }

}
