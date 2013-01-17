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
                isInitialized_ = false;
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

                        if(!isPaused_ && isInitialized_)
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
        float AndroidApplication::getKeyState(uint8_t)
        {
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

                                if(!renderer_.initializeGlesContext())
                                {
                                        shouldRun_ = false;
                                        return;
                                }

                                int32_t nativeWindowWidth  = ANativeWindow_getWidth(state_->window);
                                int32_t nativeWindowHeight = ANativeWindow_getHeight(state_->window);

                                if(nativeWindowWidth <= 1 || nativeWindowHeight <= 1)
                                {
                                        shouldRun_ = false;
                                        return;
                                }

                                Platform::defaultScreenWidth_  = static_cast<uint32_t>(nativeWindowWidth);
                                Platform::defaultScreenHeight_ = static_cast<uint32_t>(nativeWindowHeight);

                                width_  = Platform::defaultScreenWidth_;
                                height_ = Platform::defaultScreenHeight_;

                                if(!onInitialize())
                                {
                                        shouldRun_ = false;
                                        return;
                                }

                                isInitialized_ = true;
                                break;
                        }

                        case APP_CMD_TERM_WINDOW:
                        {
                                isInitialized_ = false;
                                onDestroy();
                                renderer_.destroy();
                                break;
                        }

                        case APP_CMD_LOST_FOCUS:
                        {
                                isPaused_ = true;
                                renderer_.discard();
                                break;
                        }

                        case APP_CMD_GAINED_FOCUS:
                        {
                                if(isPaused_)
                                {
                                        renderer_.retain();
                                        timer_.reset();
                                        isPaused_ = false;
                                }

                                break;
                        }
                }
        }

        //---------------------------------------------------------------------------------------
        int32_t AndroidApplication::processInputEvent(android_app*, AInputEvent* event)
        {
                static int32_t previousX = 0;
                static int32_t previousY = 0;

                if(AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
                {
                        int32_t x = AMotionEvent_getX(event, 0);
                        int32_t y = AMotionEvent_getY(event, 0);

                        cursorPosition_.x = 2.0f * static_cast<float>(x) / static_cast<float>(width_);
                        cursorPosition_.y = 2.0f * static_cast<float>(y) / static_cast<float>(height_);

                        int32_t action = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
                        if(action == AMOTION_EVENT_ACTION_UP)
                        {
                                cursorShift_.define(0.0f);
                                pressedControlButtons_ = CONTROL_BUTTON_0;
                                onControlButtonRelease(CONTROL_BUTTON_0);
                        }
                        else if(action == AMOTION_EVENT_ACTION_DOWN)
                        {
                                previousX = x;
                                previousY = y;
                                onControlButtonPress(CONTROL_BUTTON_0);
                        }
                        else
                        {
                                cursorShift_.x = static_cast<float>(x - previousX) / static_cast<float>(width_);//cursorPosition_ - cursorShift_;
                                cursorShift_.y = static_cast<float>(y - previousY) / static_cast<float>(height_);//cursorPosition_ - cursorShift_;

                                previousX = x;
                                previousY = y;
                        }

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
