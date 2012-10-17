// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "FileManager.h"
#include <memory>
#include <new>

namespace selene
{

        FileManager::FileManager(bool (*fileExists)(const char*))
        {
                fileExists_ = fileExists;
        }
        FileManager::~FileManager() {}

        //---------------------------------------------------------
        bool FileManager::addFolder(const char* folder)
        {
                if(folder == nullptr)
                        return false;

                folders_.push_back(std::string(folder));
                return true;
        }

        //---------------------------------------------------------
        const char* FileManager::find(const char* fileName) const
        {
                // validate
                if(fileName == nullptr || fileExists_ == nullptr)
                        return nullptr;

                // check current folder
                if(fileExists_(fileName))
                        return fileName;

                // search all folders
                for(auto it = folders_.begin(); it != folders_.end(); ++it)
                {
                        fileName_ = (*it) + fileName;

                        // check
                        if(fileExists_(fileName_.c_str()))
                                return fileName_.c_str();
                }

                return nullptr;
        }

        //---------------------------------------------------------
        std::istream* FileManager::open(const char* fileName) const
        {
                const char* fullFileName = find(fileName);
                if(fullFileName == nullptr)
                        return nullptr;

                std::unique_ptr<std::ifstream> stream(new(std::nothrow) std::ifstream(fullFileName, std::ios::binary | std::ios::in));

                if(stream.get() == nullptr)
                        return nullptr;

                if(!stream->good())
                        return nullptr;

                return stream.release();
        }

}
