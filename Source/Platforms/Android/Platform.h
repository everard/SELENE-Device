// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef PLATFORM_H
#define PLATFORM_H

#include "Rendering/OpenGLES/Resources/GLESTexture.h"
#include "Rendering/OpenGLES/Resources/GLESMesh.h"

#include "Rendering/OpenGLES/GLESRenderer.h"
#include "Application/AndroidApplication.h"
#include "Application/AndroidTimer.h"

#include <android/asset_manager.h>
#include <set>

namespace selene
{

        /**
         * Represents platform. Holds platform-dependent functions and application entry point.
         */
        class Platform
        {
        public:
                typedef AndroidApplication Application;
                typedef AndroidTimer Timer;

                typedef GlesRenderer Renderer;
                typedef GlesTexture Texture;
                typedef GlesMesh Mesh;

                /**
                 * Represents file manager. This file manager handles platform-dependent file management.
                 */
                class FileManager: public selene::FileManager
                {
                public:
                        FileManager();
                        ~FileManager();

                        virtual const char* find(const char* fileName) const;
                        virtual std::istream* open(const char* fileName) const;

                private:
                        AAssetManager* assetManager_;
                        std::set<std::string> fileSystemHierarchy_;

                        /**
                         * \brief Recursively reads contents of the assets directory.
                         * \param[in] directoryName name of the directory
                         */
                        void readAssetsDirectory(const std::string& directoryName);

                };

                /**
                 * \brief Checks file existence.
                 * \param[in] fileName name of the file
                 * \return true if file with given name exists
                 */
                static bool fileExists(const char* fileName);

                /**
                 * \brief Creates application. This is entry point of the program.
                 * \return pointer to the Application object
                 */
                static Application* createApplication();

                /**
                 * \brief Returns default screen width.
                 * \return default screen width
                 */
                static uint32_t getDefaultScreenWidth();

                /**
                 * \brief Returns default screen height.
                 * \return default screen height
                 */
                static uint32_t getDefaultScreenHeight();

        private:
                friend class AndroidApplication;
                friend void ::android_main(android_app* state);

                // Android state
                static android_app* state_;

                // Default screen width and height
                static uint32_t defaultScreenWidth_;
                static uint32_t defaultScreenHeight_;

        };

}

#endif
