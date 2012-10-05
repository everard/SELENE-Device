// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "TextBox.h"
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
                                                         fontSize, position, size, text) {}
        TextBox::~TextBox() {}

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
                                text_ += key;
                        else if(key == '\b' && text_.length() > 0)
                                text_.pop_back();

                        if(key != 0)
                                onChange();
                }

                if(determineRelation(cursorPosition) != OUTSIDE)
                {
                        if(IS_SET(pressedControlButtons, MOUSE_BUTTON_LEFT))
                                gui_->setActiveElement(id_);
                        else if(!is(GUI_ELEMENT_TOUCHED))
                                setFlags(GUI_ELEMENT_TOUCHED);
                }
                else
                {
                        if(is(GUI_ELEMENT_TOUCHED))
                                clearFlags(GUI_ELEMENT_TOUCHED);

                        if(IS_SET(pressedControlButtons, MOUSE_BUTTON_LEFT) &&
                           is(GUI_ELEMENT_SELECTED))
                                clearFlags(GUI_ELEMENT_SELECTED);
                }
        }

}
