// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef RENDERING_NODE_H
#define RENDERING_NODE_H

#include "RenderingMemoryAllocator.h"
#include <unordered_map>
#include <iterator>
#include <memory>
#include <deque>

namespace selene
{

        /**
         * \addtogroup Rendering
         * @{
         */

        /**
         * Represents rendering node. Each node contains elements, which are
         * sorted by given key K. During data preparation elements are added
         * to one or more rendering units (number of units is N). Each element
         * contains rendering data D, which can be anything. All memory, which
         * is used by rendering node, is allocated from RenderingMemoryBuffer.
         */
        template <class K, class D, uint8_t N = 1> class RenderingNode
        {
        public:
                /**
                 * \brief Constructs rendering node with given memory buffer.
                 * \param[in] memoryBuffer rendering memory buffer
                 */
                RenderingNode(RenderingMemoryBuffer& memoryBuffer)
                {
                        memoryBuffer_ = &memoryBuffer;
                        clear();
                }
                ~RenderingNode() {}

                /**
                 * \brief Clears node.
                 */
                void clear()
                {
                        elementsMap_ = nullptr;

                        for(uint8_t i = 0; i < N; ++i)
                                elements_[i] = nullptr;

                        currentElement_ = nullptr;
                        currentUnit_ = -1;
                }

                /**
                 * \brief Reads first element from specified unit.
                 *
                 * Next element from this unit can be read using readNextElement() function.
                 * \param[in] unit rendering unit
                 * \return true if first element from specified unit has been successfully read
                 */
                bool readFirstElement(uint8_t unit = 0)
                {
                        if(unit >= N)
                        {
                                currentUnit_ = -1;
                                return false;
                        }

                        if(elements_[unit] == nullptr)
                        {
                                currentUnit_ = -1;
                                return false;
                        }

                        currentUnit_ = unit;
                        *currentElement_ = elements_[currentUnit_]->begin();

                        if(*currentElement_ == elements_[currentUnit_]->end())
                        {
                                currentUnit_ = -1;
                                return false;
                        }

                        return true;
                }

                /**
                 * \brief Reads next element from unit.
                 *
                 * This function may only be called after using the readFirstElement() function.
                 * \see readFirstElement
                 * \return true if next element from current unit has been successfully read,
                 * false if reading of current unit has been ended
                 */
                bool readNextElement()
                {
                        if(currentUnit_ < 0)
                                return false;

                        ++(*currentElement_);

                        if(*currentElement_ == elements_[currentUnit_]->end())
                        {
                                currentUnit_ = -1;
                                return false;
                        }

                        return true;
                }

                /**
                 * \brief Returns current key.
                 *
                 * This function will return key of the current element, which has been
                 * read with readFirstElement() or readNextElement() functions.
                 * \return pointer to the current key
                 */
                K* getCurrentKey()
                {
                        if(currentUnit_ < 0)
                                return nullptr;

                        return (*(*currentElement_))->key;
                }

                /**
                 * \brief Returns current data.
                 *
                 * This function will return data of the current element, which has been
                 * read with readFirstElement() or readNextElement() functions.
                 * \return pointer to the current data
                 */
                D* getCurrentData()
                {
                        if(currentUnit_ < 0)
                                return nullptr;

                        return &((*(*currentElement_))->data);
                }

        protected:
                /**
                 * Represents element.
                 */
                class Element
                {
                public:
                        K* key;
                        D data;
                        bool isListed;

                };

                /**
                 * Represents map of the elements.
                 */
                typedef std::unordered_map<K*, Element*, std::hash<K*>, std::equal_to<K*>,
                                           RenderingMemoryAllocator<std::pair<K*, Element*>>> ElementsMap;

                /**
                 * Represents container of the elements.
                 */
                typedef std::deque<Element*, RenderingMemoryAllocator<Element*>> ElementsContainer;

                /**
                 * Represents iterator of the container of the elements.
                 */
                typedef typename ElementsContainer::iterator ElementsContainerIterator;

                /**
                 * \brief Adds element to the specified unit.
                 * \param[in] element element, which will be added to the given unit
                 * \param[in] unit unit, which will contain given element
                 * \return true if element has been successfully added
                 */
                bool addElement(Element* element, uint8_t unit = 0)
                {
                        // if element is already added, then no need to add it again
                        if(element->isListed)
                                return true;

                        // cannot write while reading
                        if(unit >= N || currentUnit_ >= 0)
                                return false;

                        // validate
                        if(elements_[unit] == nullptr)
                                return false;

                        // add element
                        element->isListed = true;

                        try
                        {
                                elements_[unit]->push_back(element);
                                return true;
                        }
                        catch(...) {}

                        return false;
                }

                /**
                 * \brief Returns element by given key (creates new if needed).
                 *
                 * Element might be created and/or returned only if currently there are
                 * no read operations performed, i.e. readFirstElement() function was not
                 * called or read operation ended with multiple calls to the readNextElement() function.
                 * \param[in] key key of the requested element
                 * \return pointer to the element
                 */
                Element* requestElement(K* key)
                {
                        // cannot write while reading
                        if(currentUnit_ >= 0)
                                return nullptr;

                        // allocate memory if needed
                        if(elementsMap_ == nullptr)
                        {
                                RenderingMemoryAllocator<std::pair<K*, Element*>> pairAllocator(*memoryBuffer_);
                                RenderingMemoryAllocator<Element*> pointerAllocator(*memoryBuffer_);

                                try
                                {
                                        elementsMap_ = memoryBuffer_->allocateMemory<ElementsMap>();
                                        new(reinterpret_cast<void*>(elementsMap_)) ElementsMap(0, std::hash<K*>(),
                                                                                               std::equal_to<K*>(), pairAllocator);

                                        for(uint8_t i = 0; i < N; ++i)
                                        {
                                                elements_[i] = memoryBuffer_->allocateMemory<ElementsContainer>();
                                                new(reinterpret_cast<void*>(elements_[i])) ElementsContainer(pointerAllocator);
                                        }

                                        currentElement_ = memoryBuffer_->allocateMemory<ElementsContainerIterator>();
                                        new(reinterpret_cast<void*>(currentElement_)) ElementsContainerIterator;
                                }
                                catch(...)
                                {
                                        clear();
                                        return nullptr;
                                }
                        }

                        // find element
                        auto it = elementsMap_->find(key);
                        if(it != elementsMap_->end())
                                return it->second;

                        // try to create new element and insert it in map
                        try
                        {
                                Element* element = memoryBuffer_->allocateMemory<Element>();
                                new(reinterpret_cast<void*>(element)) Element;

                                // initialize element
                                element->key = key;
                                element->isListed = false;

                                // insert element in map
                                auto result = elementsMap_->insert(std::make_pair(key, element));
                                if(!result.second)
                                        return nullptr;

                                // return created element
                                return result.first->second;
                        }
                        catch(...) {}

                        return nullptr;
                }

        private:
                RenderingMemoryBuffer* memoryBuffer_;

                ElementsMap* elementsMap_;
                ElementsContainer* elements_[N];

                ElementsContainerIterator* currentElement_;
                int16_t currentUnit_;

        };

        /**
         * @}
         */

}

#endif
