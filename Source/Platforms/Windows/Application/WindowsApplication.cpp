// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "WindowsApplication.h"

namespace selene
{

        WindowsApplication::WindowsApplication(const char* name): Application(name)
        {
                windowClassName_ = "SELENE Device window class";

                hInstance_ = 0;
                hWnd_ = 0;
        }
        WindowsApplication::~WindowsApplication()
        {
                destroy();
        }

        //-------------------------------------------------------------------------------------
        bool WindowsApplication::initialize(uint32_t width, uint32_t height)
        {
                hInstance_ = GetModuleHandle(nullptr);
                width_  = width;
                height_ = height;

                if(registerWindowClass() == 0)
                        return false;

                if((hWnd_ = CreateWindowEx(0, windowClassName_.c_str(), name_.c_str(), WS_POPUP,
                                           CW_USEDEFAULT, CW_USEDEFAULT, width_, height_,
                                           0, 0, hInstance_, 0)) == 0)
                {
                        UnregisterClass(windowClassName_.c_str(), hInstance_);
                        return false;
                }

                SetWindowLongPtr(hWnd_, GWL_USERDATA, (LONG)this);

                if(!onInitialize())
                {
                        destroy();
                        return false;
                }

                return true;
        }

        //-------------------------------------------------------------------------------------
        bool WindowsApplication::run()
        {
                if(hWnd_ == 0)
                        return false;

                ShowWindow(hWnd_, SW_SHOW);
                UpdateWindow(hWnd_);

                float elapsedTime = 0.0f;
                bool isRunning = true;
                WindowsTimer timer;
                MSG msg;

                while(isRunning)
                {
                        if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
                        {
                                if(msg.message == WM_QUIT)
                                {
                                        isRunning = false;
                                }
                                else
                                {
                                        TranslateMessage(&msg);
                                        DispatchMessage(&msg);
                                }
                        }
                        else
                        {
                                timer.reset();

                                onRender(elapsedTime);

                                elapsedTime = timer.getElapsedTime();

                                pressedControlButtons_ = 0;
                        }
                }

                onDestroy();
                destroy();
                return true;
        }

        //-------------------------------------------------------------------------------------
        void WindowsApplication::halt()
        {
                if(hWnd_ != nullptr)
                        PostQuitMessage(0);
        }

        //-------------------------------------------------------------------------------------
        LRESULT WindowsApplication::onMessage(UINT message, WPARAM wParam, LPARAM lParam)
        {
                return DefWindowProc(hWnd_, message, wParam, lParam);
        }

        //-------------------------------------------------------------------------------------
        LRESULT CALLBACK windowProcedure(HWND hWnd, UINT message,
                                         WPARAM wParam, LPARAM lParam)
        {
                WindowsApplication* windowsApplication = (WindowsApplication*)GetWindowLongPtr(hWnd, GWL_USERDATA);
                if(windowsApplication != nullptr)
                        return windowsApplication->processMessages(message, wParam, lParam);
                return DefWindowProc(hWnd, message, wParam, lParam);
        }

        //-------------------------------------------------------------------------------------
        ATOM WindowsApplication::registerWindowClass()
        {
                WNDCLASS windowClass;
                ZeroMemory(&windowClass, sizeof(windowClass));

                windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
                windowClass.lpszClassName = windowClassName_.c_str();
                windowClass.lpfnWndProc = (WNDPROC)windowProcedure;

                windowClass.hCursor = LoadCursor(0, IDC_ARROW);
                windowClass.style = CS_VREDRAW | CS_HREDRAW;

                windowClass.hInstance = hInstance_;
                windowClass.hIcon = 0;

                return RegisterClass(&windowClass);
        }

        //-------------------------------------------------------------------------------------
        void WindowsApplication::destroy()
        {
                if(hWnd_ == 0)
                        return;

                DestroyWindow(hWnd_);
                UnregisterClass(windowClassName_.c_str(), hInstance_);
                hInstance_ = 0;
                hWnd_ = 0;
        }

        //-------------------------------------------------------------------------------------
        LRESULT WindowsApplication::processMessages(UINT message, WPARAM wParam, LPARAM lParam)
        {
                switch(message)
                {
                        case WM_DESTROY:
                                PostQuitMessage(0);
                                break;

                        case WM_KEYUP:
                                onKeyPress((uint8_t)wParam);
                                break;

                        case WM_LBUTTONDOWN:
                                onControlButtonPress(MOUSE_BUTTON_LEFT);
                                break;

                        case WM_LBUTTONUP:
                                pressedControlButtons_ |= MOUSE_BUTTON_LEFT;
                                onControlButtonRelease(MOUSE_BUTTON_LEFT);
                                break;

                        case WM_RBUTTONDOWN:
                                onControlButtonPress(MOUSE_BUTTON_RIGHT);
                                break;

                        case WM_RBUTTONUP:
                                pressedControlButtons_ |= MOUSE_BUTTON_RIGHT;
                                onControlButtonRelease(MOUSE_BUTTON_RIGHT);
                                break;

                        case WM_MOUSEMOVE:
                                cursorPosition_.x = 2.0f * ((float)LOWORD(lParam) / (float)width_);
                                cursorPosition_.y = 2.0f * ((float)HIWORD(lParam) / (float)height_);
                                break;

                        case WM_ERASEBKGND:
                                break;

                        default:
                                return onMessage(message, wParam, lParam);
                }

                return DefWindowProc(hWnd_, message, wParam, lParam);
        }

}
