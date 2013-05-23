// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef PLATFORM_H
#define PLATFORM_H

#ifndef PLATFORM_ANDROID
#define PLATFORM_ANDROID
#endif

#include "Rendering/OpenGLES/Resources/GLESTexture.h"
#include "Rendering/OpenGLES/Resources/GLESMesh.h"

#include "Rendering/OpenGLES/GLESRenderer.h"
#include "Application/AndroidApplication.h"
#include "Application/AndroidTimer.h"

#include <android/asset_manager.h>
#include <android/log.h>
#include <set>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "SELENE-Device", __VA_ARGS__))
#define CHECK_GLES_ERROR(...) {\
                                      for(GLint error = glGetError(); error; error = glGetError())\
                                      {\
                                              LOGI("****************************** after %s() glError (0x%x)\n",\
                                                   __VA_ARGS__, error);\
                                      }\
                              }

namespace selene
{

        /**
         * \addtogroup Android
         * \brief Implementation of platform-dependent layer for Android OS.
         * @{
         */

        /**
         * Represents platform. Holds platform-dependent functions and application entry point.
         */
        class Platform
        {
        public:
                typedef AndroidApplication Application;
                typedef AndroidTimer Timer;

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

                        // File manager interface implementation
                        virtual const char* find(const char* fileName) const;
                        virtual std::istream* open(const char* fileName) const;

                private:
                        AAssetManager* assetManager_;

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
                friend void ::android_main(android_app* state);
                friend class AndroidApplication;
                friend class GlesRenderer;

                static android_app* state_;
                static uint32_t defaultScreenWidth_;
                static uint32_t defaultScreenHeight_;

        };

        /**
         * @}
         */

}

#endif
