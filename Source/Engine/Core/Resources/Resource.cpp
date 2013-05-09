// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Resource.h"
#include <limits>

namespace selene
{

        Resource::Resource(const char* name): Entity(name)
        {
                numRequests_ = 0;
        }
        Resource::~Resource() {}

        //--------------------------------------------------------------
        bool Resource::isUsed() const
        {
                return (numRequests_ > 0);
        }

        //--------------------------------------------------------------
        bool Resource::request()
        {
                if(numRequests_ == std::numeric_limits<uint32_t>::max())
                        return false;

                ++numRequests_;
                return true;
        }

        //--------------------------------------------------------------
        void Resource::release()
        {
                if(numRequests_ > 0)
                        --numRequests_;
        }

}
