// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef RENDERING_MEMORY_ALLOCATOR_H
#define RENDERING_MEMORY_ALLOCATOR_H

#include "RenderingMemoryBuffer.h"
#include <limits>

namespace selene
{

        /**
         * \addtogroup Rendering
         * @{
         */

        /**
         * Represents rendering memory allocator. Allocates memory from specified rendering
         * memory buffer. Should be used only for simple object types (which do not allocate
         * memory in the heap), because destructor for constructed objects is never called.
         */
        template <class T> class RenderingMemoryAllocator
        {
        public:
                // Type definitions according to C++11 Standard
                typedef T value_type;
                typedef T* pointer;
                typedef T& reference;
                typedef const T* const_pointer;
                typedef const T& const_reference;
                typedef std::size_t    size_type;
                typedef std::ptrdiff_t difference_type;
                template <class U> class rebind
                {
                public:
                        typedef RenderingMemoryAllocator<U> other;

                };

                /**
                 * \brief Constructs allocator with given memory buffer.
                 * \param[in] memoryBuffer rendering memory buffer
                 */
                RenderingMemoryAllocator(RenderingMemoryBuffer& memoryBuffer): memoryBuffer_(&memoryBuffer) {}
                RenderingMemoryAllocator(const RenderingMemoryAllocator&) = default;
                template <class U> RenderingMemoryAllocator(const RenderingMemoryAllocator<U>& other):
                        memoryBuffer_(other.memoryBuffer_) {}
                ~RenderingMemoryAllocator() {}
                RenderingMemoryAllocator& operator =(const RenderingMemoryAllocator&) = default;

                /**
                 * \brief Allocates memory for given number of objects.
                 *
                 * This function ignores the second parameter, because there is no
                 * need to search for free memory in rendering memory buffer.
                 * \param[in] n number of objects
                 * \return pointer to the allocated memory chunk
                 */
                pointer allocate(size_type n, const void*)
                {
                        return allocate(n);
                }

                /**
                 * \brief Allocates memory for given number of objects.
                 * \param[in] n number of objects
                 * \return pointer to the allocated memory chunk
                 */
                pointer allocate(size_type n)
                {
                        return memoryBuffer_->allocateMemory<value_type>(n);
                }

                /**
                 * \brief Does nothing.
                 *
                 * Rendering memory is deallocated with call to RenderingMemoryBuffer::clear() function.
                 */
                void deallocate(pointer, size_type) {}

                /**
                 * \brief Returns the largest value that can meaningfully be
                 * passed to allocate() function.
                 * \return std::numeric_limits<size_type>::max()
                 */
                size_type max_size() const
                {
                        return std::numeric_limits<size_type>::max();
                }

                /**
                 * \brief Constructs object of type T at p.
                 * \param[in] p location at which object should be constructed
                 * \param[in] val const reference to the object, which is used for
                 * copy construction
                 */
                void construct(pointer p, const_reference val)
                {
                        new(static_cast<void*>(p)) T(val);
                }

                /**
                 * \brief Does nothing.
                 *
                 * Destructors of objects are never called.
                 */
                template <class U> void destroy(U*) {}

        private:
                template <class U> friend class RenderingMemoryAllocator;

                RenderingMemoryBuffer* memoryBuffer_;

        };

        /**
         * @}
         */

}

#endif
