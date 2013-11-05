// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "../../Engine/Application/Application.h"
#include "Platform.h"

#include <android/native_window.h>
#include <unistd.h>

#include <sstream>
#include <cstdlib>

namespace selene
{

        android_app* Platform::state_ = nullptr;

        uint32_t Platform::defaultScreenWidth_  = 1;
        uint32_t Platform::defaultScreenHeight_ = 1;

        bool Platform::isInitialized_ = false;
        bool Platform::shouldHaltExecution_ = false;

        Platform::FileManager::FileManager():
                selene::FileManager(Platform::fileExists),
                assetManager_(Platform::state_->activity->assetManager) {}
        Platform::FileManager::~FileManager() {}

        //----------------------------------------------------------------------------------------------------------
        const char* Platform::FileManager::find(const char* fileName) const
        {
                // validate
                if(fileName == nullptr || assetManager_ == nullptr)
                        return nullptr;

                // check current folder
                AAsset* asset = AAssetManager_open(assetManager_, fileName, AASSET_MODE_STREAMING);
                if(asset != nullptr)
                {
                        AAsset_close(asset);
                        return fileName;
                }

                // search all folders
                try
                {
                        for(auto it = folders_.begin(); it != folders_.end(); ++it)
                        {
                                fileName_ = (*it) + fileName;

                                asset = AAssetManager_open(assetManager_, fileName_.c_str(), AASSET_MODE_STREAMING);
                                if(asset != nullptr)
                                {
                                        AAsset_close(asset);
                                        return fileName_.c_str();
                                }
                        }
                }
                catch(...) {}

                return nullptr;
        }

        //----------------------------------------------------------------------------------------------------------
        std::istream* Platform::FileManager::open(const char* fileName) const
        {
                const char* fullFileName = find(fileName);
                if(fullFileName == nullptr || assetManager_ == nullptr)
                        return nullptr;

                AAsset* asset = AAssetManager_open(assetManager_, fullFileName, AASSET_MODE_BUFFER);
                if(asset == nullptr)
                        return nullptr;

                std::unique_ptr<std::stringstream> stream(new(std::nothrow) std::stringstream(std::ios::binary |
                                                                                              std::ios::out |
                                                                                              std::ios::in));

                if(stream.get() == nullptr)
                {
                        AAsset_close(asset);
                        return nullptr;
                }

                const char* assetBuffer = static_cast<const char*>(AAsset_getBuffer(asset));
                off_t assetSize = AAsset_getLength(asset);

                stream->write(assetBuffer, static_cast<std::streamsize>(assetSize));
                AAsset_close(asset);

                if(!stream->good())
                        return nullptr;

                return stream.release();
        }

        //----------------------------------------------------------------------------------------------------------
        bool Platform::fileExists(const char* fileName)
        {
                return (access(fileName, F_OK) == 0);
        }

        //----------------------------------------------------------------------------------------------------------
        uint32_t Platform::getDefaultScreenWidth()
        {
                return Platform::defaultScreenWidth_;
        }

        //----------------------------------------------------------------------------------------------------------
        uint32_t Platform::getDefaultScreenHeight()
        {
                return Platform::defaultScreenHeight_;
        }

        //----------------------------------------------------------------------------------------------------------
        bool Platform::createCompatibleContext(EGLSurface& surface, EGLContext& context, EGLDisplay& display,
                                               EGLint& surfaceWidth, EGLint& surfaceHeight)
        {
                const EGLint attributes[] =
                {
                        EGL_BLUE_SIZE,  8,
                        EGL_GREEN_SIZE, 8,
                        EGL_RED_SIZE,   8,
                        EGL_ALPHA_SIZE, 8,
                        EGL_DEPTH_SIZE, 16,
                        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                        EGL_NONE
                };

                const EGLint attributesList[] =
                {
                        EGL_CONTEXT_CLIENT_VERSION, 2,
                        EGL_NONE
                };

                android_app* state = Platform::state_;
                if(state == nullptr)
                        return false;

                EGLConfig config;
                EGLint numConfigs;

                display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
                if(display == EGL_NO_DISPLAY)
                        return false;

                if(!eglInitialize(display, 0, 0))
                        return false;

                if(!eglChooseConfig(display, attributes, &config, 1, &numConfigs))
                        return false;

                EGLint format;
                if(!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format))
                        return false;

                ANativeWindow_setBuffersGeometry(state->window, 0, 0, format);

                surface = eglCreateWindowSurface(display, config, state->window, nullptr);
                if(surface == EGL_NO_SURFACE)
                        return false;

                context = eglCreateContext(display, config, nullptr, attributesList);
                if(context == EGL_NO_CONTEXT)
                        return false;

                if(eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
                        return false;

                eglQuerySurface(display, surface, EGL_WIDTH,  &surfaceWidth);
                eglQuerySurface(display, surface, EGL_HEIGHT, &surfaceHeight);

                return true;
        }

        //----------------------------------------------------------------------------------------------------------
        void Platform::destroyContext(EGLSurface& surface, EGLContext& context, EGLDisplay& display)
        {
                if(display != EGL_NO_DISPLAY)
                {
                        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

                        if(context != EGL_NO_CONTEXT)
                        {
                                eglDestroyContext(display, context);
                                context = EGL_NO_CONTEXT;
                        }

                        if(surface != EGL_NO_SURFACE)
                        {
                                eglDestroySurface(display, surface);
                                surface = EGL_NO_SURFACE;
                        }

                        eglTerminate(display);
                        display = EGL_NO_DISPLAY;
                }
        }

        //----------------------------------------------------------------------------------------------------------
        void Platform::initializationCallback(android_app* app, int32_t cmd)
        {
                switch(cmd)
                {
                        case APP_CMD_INIT_WINDOW:
                        {
                                Platform::isInitialized_ = true;

                                if(app->window == nullptr)
                                {
                                        Platform::shouldHaltExecution_ = true;
                                        return;
                                }

                                EGLSurface surface = EGL_NO_SURFACE;
                                EGLContext context = EGL_NO_CONTEXT;
                                EGLDisplay display = EGL_NO_DISPLAY;

                                EGLint surfaceWidth = 0, surfaceHeight = 0;

                                bool isDeviceCompatible =
                                        selene::Platform::createCompatibleContext(surface, context, display,
                                                                                  surfaceWidth, surfaceHeight);
                                selene::Platform::destroyContext(surface, context, display);

                                if(!isDeviceCompatible)
                                {
                                        Platform::shouldHaltExecution_ = true;
                                        return;
                                }

                                if(surfaceWidth <= 0 || surfaceHeight <= 0)
                                {
                                        Platform::shouldHaltExecution_ = true;
                                        return;
                                }

                                selene::Platform::defaultScreenWidth_  = static_cast<uint32_t>(surfaceWidth);
                                selene::Platform::defaultScreenHeight_ = static_cast<uint32_t>(surfaceHeight);
                                break;
                        }

                        case APP_CMD_TERM_WINDOW:
                                Platform::isInitialized_ = true;
                                Platform::shouldHaltExecution_ = true;
                                break;

                        case APP_CMD_LOST_FOCUS:
                        case APP_CMD_GAINED_FOCUS:
                                break;
                }
        }

}

void android_main(android_app* state)
{
        app_dummy();

        selene::Platform::state_ = state;
        state->onAppCmd = selene::Platform::initializationCallback;

        while(!selene::Platform::isInitialized_)
        {
                int ident;
                int events;
                android_poll_source* source;

                while((ident = ALooper_pollAll(0, nullptr, &events, reinterpret_cast<void**>(&source))) >= 0)
                {
                        if(source != nullptr)
                                source->process(state, source);

                        if(state->destroyRequested != 0)
                        {
                                exit(0);
                                return;
                        }
                }
        }

        if(selene::Platform::shouldHaltExecution_)
        {
                exit(0);
                return;
        }

        selene::Platform::Application* application = selene::Platform::createApplication();
        if(application == nullptr)
        {
                exit(0);
                return;
        }

        if(application->initialize())
                application->run();

        delete application;
        exit(0);
}
