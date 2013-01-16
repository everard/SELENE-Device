// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GUI.h"

namespace selene
{

        Gui::Element::Element(std::function<void (int32_t, uint8_t)> callbackFunction,
                              const Vector4d* backgroundColors,
                              const Vector4d* textColors,
                              const Vector2d& fontSize,
                              const Vector2d& position,
                              const Vector2d& size,
                              const char* text)
        {
                callbackFunction_ = callbackFunction;

                memcpy(backgroundColors_, backgroundColors, sizeof(backgroundColors_));
                memcpy(textColors_, textColors, sizeof(textColors_));

                fontSize_ = fontSize;
                position_ = position;
                size_ = size;

                if(text != nullptr)
                        text_ = text;

                id_ = -1;

                gui_ = nullptr;
        }
        Gui::Element::~Element() {}

        //------------------------------------------------------------------------------
        int32_t Gui::Element::getId() const
        {
                return id_;
        }

        //------------------------------------------------------------------------------
        const Vector4d& Gui::Element::getBackgroundColor(uint8_t type) const
        {
                if(type >= NUM_OF_GUI_ELEMENT_COLOR_TYPES)
                        return blackColor_;

                return backgroundColors_[type];
        }

        //------------------------------------------------------------------------------
        const Vector4d& Gui::Element::getTextColor(uint8_t type) const
        {
                if(type >= NUM_OF_GUI_ELEMENT_COLOR_TYPES)
                        return blackColor_;

                return textColors_[type];
        }

        //------------------------------------------------------------------------------
        const Vector2d& Gui::Element::getFontSize() const
        {
                return fontSize_;
        }

        //------------------------------------------------------------------------------
        const Vector2d& Gui::Element::getPosition() const
        {
                return position_;
        }

        //------------------------------------------------------------------------------
        const Vector2d& Gui::Element::getSize() const
        {
                return size_;
        }

        //------------------------------------------------------------------------------
        void Gui::Element::setText(const char* text)
        {
                if(text == nullptr)
                        return;

                text_ = text;

                onChange();
        }

        //------------------------------------------------------------------------------
        const std::string& Gui::Element::getText() const
        {
                return text_;
        }

        //------------------------------------------------------------------------------
        RELATION Gui::Element::determineRelation(const Vector2d& cursorPosition) const
        {
                Vector2d p = cursorPosition - position_;

                if(p.x >= 0.0f && p.x <= size_.x && p.y >= 0.0f && p.y <= size_.y)
                        return INSIDE;

                return OUTSIDE;
        }

        //------------------------------------------------------------------------------
        void Gui::Element::onChange()
        {
                if(gui_ != nullptr)
                        gui_->clearFlags(GUI_UPDATED);
        }

        //------------------------------------------------------------------------------
        void Gui::Element::setGui(Gui* gui)
        {
                gui_ = gui;
        }

        //------------------------------------------------------------------------------
        void Gui::Element::setId(int32_t id)
        {
                id_ = id;
        }

        //------------------------------------------------------------------------------
        void Gui::Element::process(const Vector2d& cursorPosition, uint8_t, uint8_t)
        {
                RELATION relation = determineRelation(cursorPosition);

                if(relation == OUTSIDE && is(GUI_ELEMENT_TOUCHED))
                        clearFlags(GUI_ELEMENT_TOUCHED);
                else if(relation == INSIDE && !is(GUI_ELEMENT_TOUCHED))
                        setFlags(GUI_ELEMENT_TOUCHED);
        }

        Gui::Gui()
        {
                nextElementId_ = 0;
        }
        Gui::~Gui()
        {
                destroy();
        }

        //------------------------------------------------------------------------------
        void Gui::destroy()
        {
                nextElementId_ = 0;
                elements_.clear();
                clearFlags(GUI_UPDATED);
        }

        //------------------------------------------------------------------------------
        int32_t Gui::addElement(Gui::Element* element)
        {
                if(element == nullptr)
                        return -1;

                std::unique_ptr<Element> elementPointer(element);

                if(nextElementId_ < 0)
                        return -1;

                elementPointer->setId(nextElementId_);
                elementPointer->setGui(this);

                auto result =
                        elements_.insert(std::make_pair(nextElementId_,
                                                        std::shared_ptr<Element>(elementPointer.release())));

                if(!result.second)
                        return -1;

                clearFlags(GUI_UPDATED);

                return nextElementId_++;
        }

        //------------------------------------------------------------------------------
        bool Gui::removeElement(int32_t elementId)
        {
                auto it = elements_.find(elementId);

                if(it == elements_.end())
                        return false;

                elements_.erase(it);
                clearFlags(GUI_UPDATED);

                return true;
        }

        //------------------------------------------------------------------------------
        std::weak_ptr<Gui::Element> Gui::getElement(int32_t elementId)
        {
                auto it = elements_.find(elementId);

                if(it == elements_.end())
                        return std::weak_ptr<Element>();

                return std::weak_ptr<Element>(it->second);
        }

        //------------------------------------------------------------------------------
        const std::map<int32_t, std::shared_ptr<Gui::Element>>& Gui::getElements() const
        {
                return elements_;
        }

        //------------------------------------------------------------------------------
        bool Gui::setActiveElement(int32_t elementId)
        {
                auto it = elements_.find(elementId);

                if(it == elements_.end())
                        return false;

                setActiveElement(it->second);

                return true;
        }

        //------------------------------------------------------------------------------
        void Gui::hideAllElements()
        {
                for(auto it = elements_.begin(); it != elements_.end(); ++it)
                        it->second->setFlags(GUI_ELEMENT_HIDDEN);
        }

        //------------------------------------------------------------------------------
        void Gui::process(const Vector2d& cursorPosition,
                          uint8_t pressedControlButtons,
                          uint8_t key)
        {
                if(is(GUI_HIDDEN | GUI_DISABLED))
                        return;

                cursorPosition_ = cursorPosition;

                for(auto it = elements_.begin(); it != elements_.end(); ++it)
                {
                        Element& element = *it->second.get();

                        if(element.is(GUI_ELEMENT_HIDDEN))
                                continue;

                        element.process(cursorPosition, pressedControlButtons, key);
                }
        }

        //------------------------------------------------------------------------------
        const Vector2d& Gui::getCursorPosition() const
        {
                return cursorPosition_;
        }

        //------------------------------------------------------------------------------
        void Gui::setActiveElement(const std::shared_ptr<Gui::Element>& element)
        {
                if(!activeElement_.expired())
                {
                        std::shared_ptr<Element> activeElement = activeElement_.lock();
                        activeElement->clearFlags(GUI_ELEMENT_SELECTED);
                }

                activeElement_ = element;

                if(!activeElement_.expired())
                {
                        std::shared_ptr<Element> activeElement = activeElement_.lock();
                        activeElement->setFlags(GUI_ELEMENT_SELECTED);
                }
        }

}
