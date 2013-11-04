// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Platform.h"
#include <iostream>
#include <fstream>

namespace selene
{

        uint32_t Platform::defaultScreenWidth_  = 640;
        uint32_t Platform::defaultScreenHeight_ = 480;

        Platform::Texture::Texture(const char* name): selene::Texture(name) {}

        //------------------------------------------------------------------------------------
        bool Platform::Texture::retain()
        {
                return true;
        }

        //------------------------------------------------------------------------------------
        void Platform::Texture::discard() {}

        Platform::Mesh::Mesh(const char* name): selene::Mesh(name) {}

        //------------------------------------------------------------------------------------
        bool Platform::Mesh::retain()
        {
                return true;
        }

        //------------------------------------------------------------------------------------
        void Platform::Mesh::discard() {}

        Platform::Application::Application(const char* name, uint32_t width, uint32_t height):
                selene::Application(name, width, height), renderer_() {}

        //------------------------------------------------------------------------------------
        bool Platform::Application::initialize()
        {
                return onInitialize();
        }

        //------------------------------------------------------------------------------------
        bool Platform::Application::run()
        {
                onDestroy();
                return true;
        }

        //------------------------------------------------------------------------------------
        void Platform::Application::halt() {}

        //------------------------------------------------------------------------------------
        Renderer& Platform::Application::getRenderer()
        {
                return renderer_;
        }

        //------------------------------------------------------------------------------------
        float Platform::Application::getKeyState(uint8_t) const
        {
                return 0.0f;
        }

        //------------------------------------------------------------------------------------
        Vector2d Platform::Application::getCursorPosition(uint8_t) const
        {
                return Vector2d();
        }

        //------------------------------------------------------------------------------------
        Vector2d Platform::Application::getCursorShift(uint8_t) const
        {
                return Vector2d();
        }

        //------------------------------------------------------------------------------------
        uint8_t Platform::Application::getNumCursors() const
        {
                return 1;
        }

        //------------------------------------------------------------------------------------
        bool Platform::Application::NullRenderer::initialize(const Parameters&)
        {
                std::cout << "initializing renderer" << std::endl;
                return true;
        }

        //------------------------------------------------------------------------------------
        void Platform::Application::NullRenderer::destroy() {}

        //------------------------------------------------------------------------------------
        void Platform::Application::NullRenderer::render(const Camera&) {}

        //------------------------------------------------------------------------------------
        void Platform::Timer::reset() {}

        //------------------------------------------------------------------------------------
        float Platform::Timer::getElapsedTime()
        {
                return 0.1f;
        }

        Platform::FileManager::FileManager(): selene::FileManager(Platform::fileExists) {}
        Platform::FileManager::~FileManager() {}

        //------------------------------------------------------------------------------------
        bool Platform::fileExists(const char* fileName)
        {
                return std::ifstream(fileName).good();
        }

        //------------------------------------------------------------------------------------
        uint32_t Platform::getDefaultScreenWidth()
        {
                return Platform::defaultScreenWidth_;
        }

        //------------------------------------------------------------------------------------
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
