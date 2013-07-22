// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Button.h"

namespace selene
{

        Button::Button(std::function<void (int32_t, uint8_t)> callbackFunction,
                       const Vector4d* backgroundColors,
                       const Vector4d* textColors,
                       const Vector2d& fontSize,
                       const Vector2d& position,
                       const Vector2d& size,
                       const char* text):
                Gui::Element(callbackFunction, backgroundColors, textColors,
                             fontSize, position, size, text) {}
        Button::~Button() {}

        //--------------------------------------------------------------------------------------
        void Button::process(const Vector2d& cursorPosition,
                             uint8_t pressedControlButtons,
                             uint8_t)
        {
                if(determineRelation(cursorPosition) != OUTSIDE)
                {
                        if(!is(GUI_ELEMENT_TOUCHED))
                                setFlags(GUI_ELEMENT_TOUCHED);

                        if(IS_SET(pressedControlButtons, CONTROL_BUTTON_0) && callbackFunction_)
                                callbackFunction_(id_, GUI_MESSAGE_ON_CLICK);
                }
                else if(is(GUI_ELEMENT_TOUCHED))
                        clearFlags(GUI_ELEMENT_TOUCHED);
        }

}
