// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "FileManager.h"

namespace selene
{

        FileManager::FileManager(bool (*fileExists)(const char*))
        {
                fileExists_ = fileExists;
        }
        FileManager::~FileManager() {}

        //-----------------------------------------------------------------------
        bool FileManager::addFolder(const char* folder)
        {
                if(folder == nullptr)
                        return false;

                folders_.push_back(std::string(folder));
                return true;
        }

        //-----------------------------------------------------------------------
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

        //-----------------------------------------------------------------------
        bool FileManager::open(const char* fileName, std::ifstream& stream) const
        {
                const char* fullFileName = find(fileName);
                if(fullFileName == nullptr)
                        return false;

                stream.open(fullFileName, std::ios::binary | std::ios::in);
                return stream.is_open();
        }

}
