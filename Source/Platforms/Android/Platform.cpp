// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "../../Engine/Application/Application.h"
#include "Platform.h"

#include <android/sensor.h>
#include <unistd.h>

namespace selene
{

        uint32_t Platform::defaultScreenWidth_  = 0;
        uint32_t Platform::defaultScreenHeight_ = 0;

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

        // TO DO: get real screen size
        selene::Platform::defaultScreenWidth_  = 640;
        selene::Platform::defaultScreenHeight_ = 480;

        selene::Platform::Application* application = selene::Platform::createApplication();
        if(application == nullptr)
                return;

        if(application->initialize())
                application->run();

        delete application;
}
