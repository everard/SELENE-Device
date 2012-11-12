// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "../../Engine/Application/Application.h"
#include "Platform.h"
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

int main()
{
        selene::Platform::Application* application = selene::Platform::createApplication();
        if(application == nullptr)
                return 0;

        if(application->initialize())
                application->run();

        delete application;
        return 0;
}
