// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef BAG_H
#define BAG_H

#include "../Macros/Macros.h"
#include <new>

namespace selene
{

        /**
         * \addtogroup Core
         * @{
         */

        /**
         * Represents bag. This is NOT regular bag data structure (i.e. unspecified order of elements, etc.).
         * This data structure is intended for fast insertion and deletion of the elements without slow memory
         * allocation and deallocation. In order to do so, placement new operator is used. Such bag has limited
         * capacity, which is specified during its creation.
         */
        template <class T> class Bag
        {
        public:
                /**
                 * \brief Constructs bag with given capacity.
                 * \param[in] capacity capacity of the bag
                 */
                Bag(uint32_t capacity)
                {
                        capacity_ = capacity;
                        size_ = 0;

                        if(capacity_ == 0)
                        {
                                data_ = nullptr;
                                elements_ = nullptr;
                        }
                        else
                        {
                                data_ = new(std::nothrow) uint8_t[capacity_ * sizeof(T)];
                                elements_ = new(std::nothrow) T*[capacity_];
                        }

                        if(data_ == nullptr || elements_ == nullptr)
                        {
                                SAFE_DELETE_ARRAY(data_);
                                SAFE_DELETE_ARRAY(elements_);
                                capacity_ = 0;
                                return;
                        }

                        for(uint32_t i = 0; i < capacity_; ++i)
                                elements_[i] = reinterpret_cast<T*>(&data_[i * sizeof(T)]);
                }
                ~Bag()
                {
                        clear();

                        SAFE_DELETE_ARRAY(data_);
                        SAFE_DELETE_ARRAY(elements_);
                }

                /**
                 * \brief Clears bag.
                 *
                 * For each element in the bag its destructor is called.
                 */
                void clear()
                {
                        // explicitly call destructors
                        for(uint32_t i = 0; i < size_; ++i)
                                elements_[i]->~T();

                        size_ = 0;
                }

                /**
                 * \brief Adds element to the bag.
                 * \param[in] element added element
                 * \return true if element successfully added
                 */
                bool add(const T& element)
                {
                        if(size_ >= getCapacity())
                                return false;

                        elements_[size_] = new(reinterpret_cast<void*>(elements_[size_])) T(element);
                        ++size_;

                        return true;
                }

                /**
                 * \brief Removes element from the bag.
                 *
                 * The last element takes place of the deleted one.
                 * \param[in] index index of the deleted element
                 * \return true if element successfully deleted
                 */
                bool remove(uint32_t index)
                {
                        if(index >= size_)
                                return false;

                        elements_[index]->~T();
                        --size_;

                        if(size_ == 0)
                                return true;

                        T* deletedElement = elements_[index];
                        elements_[index] = elements_[size_];
                        elements_[size_] = deletedElement;

                        return true;
                }

                /**
                 * \brief Returns size.
                 * \return current size of the bag
                 */
                uint32_t getSize() const
                {
                        return size_;
                }

                /**
                 * \brief Returns capacity.
                 * \return capacity of the bag
                 */
                uint32_t getCapacity() const
                {
                        return capacity_;
                }

                /**
                 * \brief Returns element.
                 * \param[in] index index of the element
                 * \return reference to the element
                 */
                T& operator [](size_t index)
                {
                        return *elements_[index];
                }

                /**
                 * \brief Returns element.
                 * \param[in] index index of the element
                 * \return const reference to the element
                 */
                const T& operator [](size_t index) const
                {
                        return *elements_[index];
                }

        private:
                uint8_t* data_;
                T** elements_;
                volatile uint32_t size_;
                uint32_t capacity_;

        };

        /**
         * @}
         */

}

#endif
