// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "../Macros/Macros.h"
#include <fstream>
#include <vector>
#include <string>

namespace selene
{

        /**
         * Represents file manager.
         * File manager provides access to files (folders which are used in search for files may be specified as well).
         */
        class FileManager
        {
        public:
                /**
                 * \brief Constructs file manager with given file existence check function.
                 * \param[in] fileExists function which is used to check file existence
                 */
                FileManager(bool (*fileExists)(const char*));
                virtual ~FileManager();

                /**
                 * \brief Adds a folder. Contents of this folder will be looked during search for files.
                 * \param[in] folder folder used in search
                 * \return true if folder successfully added
                 */
                bool addFolder(const char* folder);

                /**
                 * \brief Finds file with given name.
                 *
                 * Searches in folders specified with addFolder.
                 * \param[in] fileName name of the file to find
                 * \return c-string containing full path if file was found, nullptr otherwise
                 */
                virtual const char* find(const char* fileName) const;

                /**
                 * \brief Opens file with given file name.
                 *
                 * Looks for file in folders specified with addFolder and opens it.
                 * \param[in] fileName name of the file to open
                 * \return pointer to the std::istream if file was found and opened, nullptr otherwise
                 */
                virtual std::istream* open(const char* fileName) const;

        private:
                // File name
                mutable std::string fileName_;

                // Folders
                std::vector<std::string> folders_;

                // File existence check function
                bool (*fileExists_)(const char*);

        };

}

#endif
