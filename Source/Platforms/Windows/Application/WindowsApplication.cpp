// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "WindowsApplication.h"

namespace selene
{

        WindowsApplication::WindowsApplication(const char* name, uint32_t width, uint32_t height):
                Application(name, width, height), windowClassName_("SELENE Device window class"),
                hInstance_(nullptr), hWnd_(nullptr), cursorPosition_(), cursorShift_(),
                renderer_(), isActive_(true) {}
        WindowsApplication::~WindowsApplication()
        {
                destroy();
        }

        //-------------------------------------------------------------------------------------------------------
        bool WindowsApplication::initialize()
        {
                hInstance_ = GetModuleHandle(nullptr);

                if(registerWindowClass() == 0)
                        return false;

                if((hWnd_ = CreateWindowEx(0, windowClassName_.c_str(), getName(), WS_POPUP,
                                           CW_USEDEFAULT, CW_USEDEFAULT, getWidth(),
                                           getHeight(), 0, 0, hInstance_, 0)) == 0)
                {
                        UnregisterClass(windowClassName_.c_str(), hInstance_);
                        return false;
                }

                SetWindowLongPtr(hWnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

                if(!onInitialize())
                {
                        destroy();
                        return false;
                }

                return true;
        }

        //-------------------------------------------------------------------------------------------------------
        bool WindowsApplication::run()
        {
                if(hWnd_ == nullptr)
                        return false;

                ShowWindow(hWnd_, SW_SHOW);
                UpdateWindow(hWnd_);

                float elapsedTime = 0.0f;
                bool isRunning = true;
                WindowsTimer timer;
                MSG msg;

                POINT point, centralPoint;
                centralPoint.x = getWidth()  >> 1;
                centralPoint.y = getHeight() >> 1;

                float coefficientX = 2.0f / static_cast<float>(getWidth());
                float coefficientY = 2.0f / static_cast<float>(getHeight());

                ShowCursor(FALSE);

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

                                if(!isActive_)
                                        continue;

                                SetCursorPos(centralPoint.x, centralPoint.y);

                                onUpdate(elapsedTime);
                                onRender(elapsedTime);

                                elapsedTime = timer.getElapsedTime();

                                GetCursorPos(&point);
                                cursorShift_.define(static_cast<float>(point.x - centralPoint.x) * coefficientX,
                                                    static_cast<float>(point.y - centralPoint.y) * coefficientY);
                                cursorPosition_ += cursorShift_;

                                cursorPosition_.x = cursorPosition_.x < 0.0f ? 0.0f : cursorPosition_.x;
                                cursorPosition_.y = cursorPosition_.y < 0.0f ? 0.0f : cursorPosition_.y;

                                cursorPosition_.x = cursorPosition_.x > 2.0f ? 2.0f : cursorPosition_.x;
                                cursorPosition_.y = cursorPosition_.y > 2.0f ? 2.0f : cursorPosition_.y;

                                setPressedControlButtonsMask(0);
                        }
                }

                onDestroy();
                destroy();
                return true;
        }

        //-------------------------------------------------------------------------------------------------------
        void WindowsApplication::halt()
        {
                if(hWnd_ != nullptr)
                        PostQuitMessage(0);
        }

        //-------------------------------------------------------------------------------------------------------
        Renderer& WindowsApplication::getRenderer()
        {
                return renderer_;
        }

        //-------------------------------------------------------------------------------------------------------
        float WindowsApplication::getKeyState(uint8_t key) const
        {
                if(::GetAsyncKeyState(key) != 0)
                        return 1.0f;

                return 0.0f;
        }

        //-------------------------------------------------------------------------------------------------------
        Vector2d WindowsApplication::getCursorPosition(uint8_t) const
        {
                return cursorPosition_;
        }

        //-------------------------------------------------------------------------------------------------------
        Vector2d WindowsApplication::getCursorShift(uint8_t) const
        {
                return cursorShift_;
        }

        //-------------------------------------------------------------------------------------------------------
        uint8_t WindowsApplication::getNumCursors() const
        {
                return 1;
        }

        //-------------------------------------------------------------------------------------------------------
        HWND WindowsApplication::getWindowHandle()
        {
                return hWnd_;
        }

        //-------------------------------------------------------------------------------------------------------
        LRESULT WindowsApplication::onMessage(UINT message, WPARAM wParam, LPARAM lParam)
        {
                return DefWindowProc(hWnd_, message, wParam, lParam);
        }

        //-------------------------------------------------------------------------------------------------------
        LRESULT CALLBACK WindowsApplication::windowProcedure(HWND hWnd, UINT message,
                                                             WPARAM wParam, LPARAM lParam)
        {
                WindowsApplication* windowsApplication =
                        reinterpret_cast<WindowsApplication*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
                if(windowsApplication != nullptr)
                        return windowsApplication->processMessages(message, wParam, lParam);
                return DefWindowProc(hWnd, message, wParam, lParam);
        }

        //-------------------------------------------------------------------------------------------------------
        ATOM WindowsApplication::registerWindowClass()
        {
                WNDCLASS windowClass;
                ZeroMemory(&windowClass, sizeof(windowClass));

                windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
                windowClass.lpszClassName = windowClassName_.c_str();
                windowClass.lpfnWndProc = reinterpret_cast<WNDPROC>(windowProcedure);

                windowClass.hCursor = LoadCursor(0, IDC_ARROW);
                windowClass.style = CS_VREDRAW | CS_HREDRAW;

                windowClass.hInstance = hInstance_;
                windowClass.hIcon = nullptr;

                return RegisterClass(&windowClass);
        }

        //-------------------------------------------------------------------------------------------------------
        void WindowsApplication::destroy()
        {
                renderer_.destroy();

                if(hWnd_ == nullptr)
                        return;

                DestroyWindow(hWnd_);
                UnregisterClass(windowClassName_.c_str(), hInstance_);
                hInstance_ = nullptr;
                hWnd_ = nullptr;
        }

        //-------------------------------------------------------------------------------------------------------
        LRESULT WindowsApplication::processMessages(UINT message, WPARAM wParam, LPARAM lParam)
        {
                switch(message)
                {
                        case WM_DESTROY:
                                PostQuitMessage(0);
                                break;

                        case WM_ACTIVATE:
                                if(LOWORD(wParam) != WA_INACTIVE)
                                        isActive_ = true;
                                else
                                        isActive_ = false;
                                break;

                        case WM_KEYUP:
                                onKeyPress(static_cast<uint8_t>(wParam));
                                break;

                        case WM_LBUTTONDOWN:
                                onControlButtonPress(CONTROL_BUTTON_0);
                                break;

                        case WM_LBUTTONUP:
                                setPressedControlButtonsMask(getPressedControlButtonsMask() | CONTROL_BUTTON_0);
                                onControlButtonRelease(CONTROL_BUTTON_0);
                                break;

                        case WM_RBUTTONDOWN:
                                onControlButtonPress(CONTROL_BUTTON_1);
                                break;

                        case WM_RBUTTONUP:
                                setPressedControlButtonsMask(getPressedControlButtonsMask() | CONTROL_BUTTON_1);
                                onControlButtonRelease(CONTROL_BUTTON_1);
                                break;

                        case WM_MOUSEMOVE:
                                break;

                        case WM_ERASEBKGND:
                                break;

                        default:
                                return onMessage(message, wParam, lParam);
                }

                return DefWindowProc(hWnd_, message, wParam, lParam);
        }

}
