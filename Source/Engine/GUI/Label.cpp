// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Label.h"

namespace selene
{

        Label::Label(std::function<void (int32_t, uint8_t)> callbackFunction,
                     const Vector4d* backgroundColors,
                     const Vector4d* textColors,
                     const Vector2d& fontSize,
                     const Vector2d& position,
                     const Vector2d& size,
                     const char* text): Gui::Element(callbackFunction, backgroundColors, textColors,
                                                     fontSize, position, size, text) {}
        Label::~Label() {}

}
