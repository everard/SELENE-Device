// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "../../Engine/Application/Application.h"
#include "Platform.h"
#include <io.h>

namespace selene
{

        uint32_t Platform::defaultScreenWidth_  = 640;
        uint32_t Platform::defaultScreenHeight_ = 480;

        Platform::FileManager::FileManager(): selene::FileManager(Platform::fileExists) {}
        Platform::FileManager::~FileManager() {}

        //---------------------------------------------------------------------------------
        bool Platform::fileExists(const char* fileName)
        {
                return (_access(fileName, 0) == 0);
        }

        //---------------------------------------------------------------------------------
        uint32_t Platform::getDefaultScreenWidth()
        {
                return Platform::defaultScreenWidth_;
        }

        //---------------------------------------------------------------------------------
        uint32_t Platform::getDefaultScreenHeight()
        {
                return Platform::defaultScreenHeight_;
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
