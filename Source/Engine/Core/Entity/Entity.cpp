// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Entity.h"
#include <new>

namespace selene
{

        Entity::Entity(const char* name)
        {
                name_ = nullptr;

                if(name != nullptr)
                {
                        auto nameLength = std::strlen(name) + 1;
                        name_ = new(std::nothrow) char[nameLength];

                        if(name_ == nullptr)
                                return;

                        std::memcpy(name_, name, nameLength);
                }
        }
        Entity::~Entity()
        {
                SAFE_DELETE_ARRAY(name_);
        }

        //---------------------------------------------------------
        const char* Entity::getName() const
        {
                return name_;
        }

}
