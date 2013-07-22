// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef ARRAY_H
#define ARRAY_H

#include "../Macros/Macros.h"
#include <new>

namespace selene
{

        /**
         * \addtogroup Core
         * @{
         */

        /**
         * Represents array.
         * In such array data stride may be specified (this might be helpful for holding mesh vertices).
         * \see Array::create for use cases.
         */
        template <class D, class S> class Array
        {
        public:
                Array():
                        data_(nullptr), size_(0), sizeModifier_(0),
                        realSize_(0), stride_(0) {}
                Array(const Array<D, S>& other): Array()
                {
                        *this = other;
                }
                ~Array()
                {
                        destroy();
                }

                /**
                 * \brief Checks array's emptiness.
                 * \return true if array is empty
                 */
                bool isEmpty() const
                {
                        if(data_ == nullptr || size_ == 0 || stride_ == 0)
                                return true;

                        return false;
                }

                /**
                 * \brief Creates array.
                 *
                 * Array's real size is determined through following formula: real size = size * stride * sizeModifier.
                 * Such array modification is helpful for holding the mesh vertices and faces data.
                 * For example, mesh vertices are often represented as array of bytes. And it is critical to have
                 * stride information. This also applies for mesh faces data. But in case of faces there is also need
                 * for array size modification, because face consists of multiple indices (three for triangles, four
                 * for quads).
                 * \param[in] size size of array
                 * \param[in] stride data stride
                 * \param[in] sizeModifier array size modifier
                 * \return true if array successfully created
                 */
                bool create(S size, uint8_t stride = 1, S sizeModifier = 1)
                {
                        // destroy array
                        destroy();

                        // validate
                        if(size <= 0 || stride < 1 || sizeModifier < 1)
                                return false;

                        size_ = size;
                        sizeModifier_ = sizeModifier;
                        stride_ = stride;

                        realSize_ = size_ * sizeModifier_ * stride_;

                        // allocate memory
                        data_ = new(std::nothrow) D[realSize_];

                        // validate
                        if(data_ == nullptr)
                        {
                                size_ = sizeModifier_ = 0;
                                realSize_ = 0;
                                stride_ = 0;
                                return false;
                        }

                        return true;
                }

                /**
                 * \brief Destroys array.
                 */
                void destroy()
                {
                        delete[] data_;
                        data_ = nullptr;

                        size_ = sizeModifier_ = 0;
                        realSize_ = 0;
                        stride_ = 0;
                }

                /**
                 * \brief Returns size.
                 * \return array size without modification and stride
                 */
                S getSize() const
                {
                        return size_;
                }

                /**
                 * \brief Returns data stride.
                 * \return data stride
                 */
                uint8_t getStride() const
                {
                        return stride_;
                }

                /**
                 * \brief Returns array element.
                 * \param[in] index index of array element
                 * \return reference to the array element
                 */
                D& operator [](size_t index)
                {
                        return data_[index];
                }

                /**
                 * \brief Returns array element.
                 * \param[in] index index of array element
                 * \return const reference to the array element
                 */
                const D& operator [](size_t index) const
                {
                        return data_[index];
                }

                /**
                 * \brief Assignes array.
                 * \param[in] other array, which will be assigned to current
                 * \return reference to the array
                 */
                Array<D, S>& operator =(const Array<D, S>& other)
                {
                        if(sizeModifier_ == other.sizeModifier_ &&
                           stride_ == other.stride_ &&
                           size_   == other.size_)
                        {
                                for(uint32_t i = 0; i < realSize_; ++i)
                                        data_[i] = other.data_[i];

                                return *this;
                        }

                        destroy();

                        if(create(other.size_, other.stride_, other.sizeModifier_))
                        {
                                for(uint32_t i = 0; i < realSize_; ++i)
                                        data_[i] = other.data_[i];
                        }

                        return *this;
                }

        private:
                D* data_;
                S size_, sizeModifier_;
                uint32_t realSize_;
                uint8_t stride_;

        };

        /**
         * @}
         */

}

#endif
