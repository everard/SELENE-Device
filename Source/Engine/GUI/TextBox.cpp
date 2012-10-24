// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "TextBox.h"
#include <algorithm>
#include <iterator>
#include <cstring>

namespace selene
{

        TextBox::TextBox(std::function<void (int32_t, uint8_t)> callbackFunction,
                         const Vector4d* backgroundColors,
                         const Vector4d* textColors,
                         const Vector2d& fontSize,
                         const Vector2d& position,
                         const Vector2d& size,
                         const char* text): Gui::Element(callbackFunction, backgroundColors, textColors,
                                                         fontSize, position, size, text)
        {
                if(text_.length() > 0)
                        std::copy(text_.begin(), text_.end(), std::back_inserter(textVector_));
        }
        TextBox::~TextBox() {}

        //---------------------------------------------------
        void TextBox::setText(const char* text)
        {
                if(text == nullptr)
                        return;

                text_ = text;
                textVector_.clear();
                if(text_.length() > 0)
                        std::copy(text_.begin(), text_.end(), std::back_inserter(textVector_));

                setFlags(GUI_ELEMENT_UPDATED);
        }

        //---------------------------------------------------
        const std::string& TextBox::getText() const
        {
                if(!is(GUI_ELEMENT_UPDATED))
                {
                        setFlags(GUI_ELEMENT_UPDATED);
                        text_.clear();
                        if(textVector_.size() > 0)
                                std::copy(textVector_.begin(), textVector_.end(), std::back_inserter(text_));
                }

                return text_;
        }

        //---------------------------------------------------
        void TextBox::process(const Vector2d& cursorPosition,
                              uint8_t pressedControlButtons,
                              uint8_t key)
        {
                if(gui_ == nullptr)
                        return;

                if(is(GUI_ELEMENT_SELECTED))
                {
                        if(isprint(key))
                                textVector_.push_back(key);
                        else if(key == '\b' && textVector_.size() > 0)
                                textVector_.pop_back();

                        if(key != 0)
                                clearFlags(GUI_ELEMENT_UPDATED);
                }

                if(determineRelation(cursorPosition) != OUTSIDE)
                {
                        if(IS_SET(pressedControlButtons, CONTROL_BUTTON_0))
                                gui_->setActiveElement(id_);
                        else if(!is(GUI_ELEMENT_TOUCHED))
                                setFlags(GUI_ELEMENT_TOUCHED);
                }
                else
                {
                        if(is(GUI_ELEMENT_TOUCHED))
                                clearFlags(GUI_ELEMENT_TOUCHED);

                        if(IS_SET(pressedControlButtons, CONTROL_BUTTON_0) &&
                           is(GUI_ELEMENT_SELECTED))
                                clearFlags(GUI_ELEMENT_SELECTED);
                }
        }

}
