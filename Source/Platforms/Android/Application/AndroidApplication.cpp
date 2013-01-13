// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "AndroidApplication.h"
#include "../Platform.h"

namespace selene
{

        AndroidApplication::AndroidApplication(const char* name, uint32_t width, uint32_t height): Application(name, width, height)
        {
                state_ = nullptr;
                shouldRun_ = true;
                isPaused_  = false;
        }
        AndroidApplication::~AndroidApplication() {}

        //---------------------------------------------------------------------------------------
        bool AndroidApplication::initialize()
        {
                state_ = Platform::state_;

                if(state_ == nullptr)
                        return false;

                state_->userData = this;
                state_->onAppCmd = commandProcessingCallback;
                state_->onInputEvent = inputProcessingCallback;

                return true;
        }

        //---------------------------------------------------------------------------------------
        bool AndroidApplication::run()
        {
                float elapsedTime = 0.0f;
                timer_.reset();

                while(shouldRun_)
                {
                        int ident;
                        int events;
                        android_poll_source* source;

                        while((ident = ALooper_pollAll(isPaused_ ? -1 : 0, nullptr, &events,
                                                       reinterpret_cast<void**>(&source))) >= 0)
                        {

                                if(source != nullptr)
                                        source->process(state_, source);

                                if(state_->destroyRequested != 0)
                                {
                                        onDestroy();
                                        return true;
                                }
                        }

                        if(!isPaused_)
                        {
                                timer_.reset();

                                onUpdate(elapsedTime);
                                onRender(elapsedTime);

                                elapsedTime = timer_.getElapsedTime();
                                pressedControlButtons_ = 0;
                        }
                }

                return true;
        }

        //---------------------------------------------------------------------------------------
        void AndroidApplication::halt()
        {
                shouldRun_ = false;
        }

        //---------------------------------------------------------------------------------------
        android_app* AndroidApplication::getHandle()
        {
                return state_;
        }

        //---------------------------------------------------------------------------------------
        float AndroidApplication::getKeyState(uint8_t key)
        {
                key = 0;
                return 0.0f;
        }

        //---------------------------------------------------------------------------------------
        void AndroidApplication::processCommand(android_app* app, int32_t cmd)
        {
                if(!shouldRun_)
                        return;

                switch(cmd)
                {
                        case APP_CMD_INIT_WINDOW:
                        {
                                if(app->window == nullptr)
                                {
                                        shouldRun_ = false;
                                        return;
                                }

                                int32_t width  = ANativeWindow_getWidth(app->window);
                                int32_t height = ANativeWindow_getHeight(app->window);

                                if(width <= 0 || height <= 0)
                                {
                                        shouldRun_ = false;
                                        return;
                                }

                                Platform::defaultScreenWidth_  = static_cast<uint32_t>(width);
                                Platform::defaultScreenHeight_ = static_cast<uint32_t>(height);

                                width_  = Platform::defaultScreenWidth_;
                                height_ = Platform::defaultScreenHeight_;

                                if(!onInitialize())
                                {
                                        shouldRun_ = false;
                                        return;
                                }

                                break;
                        }

                        case APP_CMD_TERM_WINDOW:
                        {
                                onDestroy();
                                break;
                        }

                        case APP_CMD_LOST_FOCUS:
                        {
                                isPaused_ = true;
                                renderer_.discardContext();
                                break;
                        }

                        case APP_CMD_GAINED_FOCUS:
                        {
                                if(isPaused_)
                                {
                                        renderer_.createContext();
                                        timer_.reset();
                                        isPaused_ = false;
                                }

                                break;
                        }
                }
        }

        //---------------------------------------------------------------------------------------
        int32_t AndroidApplication::processInputEvent(android_app* app, AInputEvent* event)
        {
                if(AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
                {
                        int32_t x = AMotionEvent_getX(event, 0);
                        int32_t y = AMotionEvent_getY(event, 0);

                        cursorShift_ = cursorPosition_;

                        cursorPosition_.x = 2.0f * static_cast<float>(x) / static_cast<float>(width_);
                        cursorPosition_.y = 2.0f * static_cast<float>(y) / static_cast<float>(height_);

                        cursorShift_ = cursorPosition_ - cursorShift_;

                        if((AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK) == AMOTION_EVENT_ACTION_UP)
                                pressedControlButtons_ = CONTROL_BUTTON_0;

                        return 1;
                }

                return 0;
        }

        //---------------------------------------------------------------------------------------
        void AndroidApplication::commandProcessingCallback(android_app* app, int32_t cmd)
        {
                AndroidApplication* application = static_cast<AndroidApplication*>(app->userData);

                if(application == nullptr)
                        return;

                application->processCommand(app, cmd);
        }

        //---------------------------------------------------------------------------------------
        int32_t AndroidApplication::inputProcessingCallback(android_app* app, AInputEvent* event)
        {
                AndroidApplication* application = static_cast<AndroidApplication*>(app->userData);

                if(application == nullptr)
                        return 0;

                return application->processInputEvent(app, event);
        }

}
