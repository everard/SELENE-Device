// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef RENDERING_MEMORY_BUFFER_H
#define RENDERING_MEMORY_BUFFER_H

#include "../Core/Macros/Macros.h"
#include <exception>
#include <memory>
#include <deque>

namespace selene
{

        /**
         * \addtogroup Rendering
         * @{
         */

        /**
         * Represents rendering memory buffer.
         */
        class RenderingMemoryBuffer
        {
        public:
                RenderingMemoryBuffer();
                ~RenderingMemoryBuffer();

                /**
                 * \brief Initializes rendering memory buffer.
                 *
                 * Allocates given number of bytes from system memory for the buffer.
                 * After this operation allocation with allocateMemory() becomes possible.
                 * \param[in] size size of the memory buffer in bytes
                 * \return true if rendering memory buffer has been successfully initialized
                 */
                bool initialize(std::size_t size);

                /**
                 * \brief Destroys memory buffer.
                 *
                 * Frees system memory. Allocation with allocateMemory() is no longer possible.
                 */
                void destroy();

                /**
                 * \brief Allocates memory for given number of objects of type T.
                 * \param[in] numObjects number of objects of type T
                 * \return pointer to the memory chunk (if buffer is out of memory, then this
                 * function throws std::bad_alloc)
                 */
                template <class T> T* allocateMemory(std::size_t numObjects = 1)
                {
                        if(size_ == 0 || allocatedMemory_ == nullptr || numObjects == 0)
                                throw std::bad_alloc();

                        std::size_t size = numObjects * sizeof(T);
                        if((location_ + size) >= size_)
                                throw std::bad_alloc();

                        T* result = reinterpret_cast<T*>(&allocatedMemory_[location_]);
                        location_ += size;
                        return result;
                }

                /**
                 * \brief Clears memory buffer.
                 *
                 * After this operation memory buffer becomes empty, but system memory is not freed.
                 */
                void clear();

        private:
                uint8_t* allocatedMemory_;
                std::size_t location_, size_;

        };

        /**
         * @}
         */

}

#endif
