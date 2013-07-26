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
                LOGI("****************************** OPENING FILE %s", fileName);
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

                LOGI("****************************** SUCCESS");
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
