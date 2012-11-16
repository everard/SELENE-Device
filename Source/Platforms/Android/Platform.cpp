// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "../../Engine/Application/Application.h"
#include "Platform.h"

#include <android_native_app_glue.h>
#include <android/sensor.h>
#include <unistd.h>

namespace selene
{

        Platform::FileManager::FileManager(): selene::FileManager(Platform::fileExists) {}
        Platform::FileManager::~FileManager() {}

        //---------------------------------------------
        bool Platform::fileExists(const char* fileName)
        {
                return (access(fileName, F_OK) == 0);
        }

}

void android_main(android_app* state)
{
        app_dummy();

        selene::Platform::Application* application = selene::Platform::createApplication();
        if(application == nullptr)
                return;

        if(application->initialize())
                application->run();

        delete application;
}
