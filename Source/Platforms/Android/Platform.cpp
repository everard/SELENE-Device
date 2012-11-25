// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "../../Engine/Application/Application.h"
#include "Platform.h"

#include <android/native_window.h>
#include <unistd.h>
#include <cstdlib>

namespace selene
{

        android_app* Platform::state_ = nullptr;
        uint32_t Platform::defaultScreenWidth_  = 1;
        uint32_t Platform::defaultScreenHeight_ = 1;

        Platform::FileManager::FileManager(): selene::FileManager(Platform::fileExists) {}
        Platform::FileManager::~FileManager() {}

        //---------------------------------------------
        bool Platform::fileExists(const char* fileName)
        {
                return (access(fileName, F_OK) == 0);
        }

        //---------------------------------------------
        uint32_t Platform::getDefaultScreenWidth()
        {
                return Platform::defaultScreenWidth_;
        }

        //---------------------------------------------
        uint32_t Platform::getDefaultScreenHeight()
        {
                return Platform::defaultScreenHeight_;
        }

}

void android_main(android_app* state)
{
        app_dummy();

        selene::Platform::state_ = state;

        selene::Platform::Application* application = selene::Platform::createApplication();
        if(application == nullptr)
                return;

        if(application->initialize())
                application->run();

        delete application;
        exit(0);
}
