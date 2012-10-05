// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef WINDOWS_APPLICATION_H
#define WINDOWS_APPLICATION_H

#include "../../../Engine/Application/Application.h"
#include "WindowsTimer.h"

namespace selene
{

        // Represents windows application.
        class WindowsApplication: public Application
        {
        public:
                WindowsApplication(const char* name, const char* windowClassName);
                ~WindowsApplication();

                // Initializes application
                bool initialize(uint32_t width, uint32_t height);

                // Runs application
                bool run();

        protected:
                // Window class name
                std::string windowClassName_;

                // Applicaton instance
                HINSTANCE hInstance_;

                // Window handle
                HWND hWnd_;

                // Windows message event callback
                virtual LRESULT onMessage(UINT message, WPARAM wParam, LPARAM lParam);

        private:
                // Window procedure
                static friend LRESULT CALLBACK windowProcedure(HWND hWnd, UINT message,
                                                               WPARAM wParam, LPARAM lParam);

                // Registers window class
                ATOM registerWindowClass();

                // Destroys window
                void destroy();

                // Processes messages
                LRESULT processMessages(UINT message, WPARAM wParam, LPARAM lParam);

        };

}

#endif
