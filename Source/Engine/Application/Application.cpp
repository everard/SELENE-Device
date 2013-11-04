// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Application.h"

namespace selene
{

        Application::Application(const char* name, uint32_t width, uint32_t height):
                Entity(name), width_(width), height_(height), pressedControlButtonsMask_(0) {}
        Application::~Application() {}

        //------------------------------------------------------------------------------------
        uint32_t Application::getWidth() const
        {
                return width_;
        }

        //------------------------------------------------------------------------------------
        uint32_t Application::getHeight() const
        {
                return height_;
        }

        //------------------------------------------------------------------------------------
        void Application::setPressedControlButtonsMask(uint8_t mask)
        {
                pressedControlButtonsMask_ = mask;
        }

        //------------------------------------------------------------------------------------
        uint8_t Application::getPressedControlButtonsMask() const
        {
                return pressedControlButtonsMask_;
        }

}
