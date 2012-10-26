// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Application.h"

namespace selene
{

        Application::Application(const char* name, uint32_t width, uint32_t height): Entity(name)
        {
                width_  = width;
                height_ = height;
                pressedControlButtons_ = 0;
        }
        Application::~Application() {}

}
