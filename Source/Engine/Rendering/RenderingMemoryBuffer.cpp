// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "RenderingMemoryBuffer.h"
#include <new>

namespace selene
{

        RenderingMemoryBuffer::RenderingMemoryBuffer()
        {
                allocatedMemory_ = nullptr;
                location_ = size_ = 0;
        }
        RenderingMemoryBuffer::~RenderingMemoryBuffer()
        {
                destroy();
        }

        //----------------------------------------------------------
        bool RenderingMemoryBuffer::initialize(std::size_t size)
        {
                destroy();

                size_ = size;

                if(size_ == 0)
                        return false;

                allocatedMemory_ = new(std::nothrow) uint8_t[size_];
                if(allocatedMemory_ == nullptr)
                        return false;

                location_ = 0;
                return true;
        }

        //----------------------------------------------------------
        void RenderingMemoryBuffer::destroy()
        {
                SAFE_DELETE_ARRAY(allocatedMemory_);
                location_ = size_ = 0;
        }

        //----------------------------------------------------------
        void RenderingMemoryBuffer::clear()
        {
                location_ = 0;
        }

}
