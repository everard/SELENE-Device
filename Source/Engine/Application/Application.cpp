// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Application.h"

namespace selene
{

        Application::Application(const char* name): Entity(name)
        {
                width_ = height_ = 0;
                pressedControlButtons_ = 0;
        }
        Application::~Application() {}

        //------------------------------------------------------
        bool Application::onInitialize()
        {
                return true;
        }

        //------------------------------------------------------
        void Application::onDestroy() {}

        //------------------------------------------------------
        void Application::onKeyPress(uint8_t key)
        {
                key = 0;
        }

        //------------------------------------------------------
        void Application::onControlButtonPress(uint8_t button)
        {
                button = 0;
        }

        //------------------------------------------------------
        void Application::onControlButtonRelease(uint8_t button)
        {
                button = 0;
        }

        //------------------------------------------------------
        void Application::onUpdate(float elapsedTime)
        {
                elapsedTime = 0.0f;
        }

        //------------------------------------------------------
        void Application::onRender(float elapsedTime)
        {
                elapsedTime = 0.0f;
        }

}
