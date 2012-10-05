// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Entity.h"

namespace selene
{

        Entity::Entity(const char* name)
        {
                if(name != nullptr)
                        name_ = name;
        }
        Entity::~Entity() {}

        //---------------------------------
        const char* Entity::getName() const
        {
                return name_.c_str();
        }

}
