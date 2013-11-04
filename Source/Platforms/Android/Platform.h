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
#define CHECK_GLES_ERROR(...)\
{\
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
                        FileManager(const FileManager&) = delete;
                        ~FileManager();
                        FileManager& operator =(const FileManager&) = delete;

                        // FileManager interface implementation
                        const char* find(const char* fileName) const;
                        std::istream* open(const char* fileName) const;

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

                /**
                 * \brief Creates compatible context.
                 * \param[out] surface EGL surface
                 * \param[out] context OpenGL ES context
                 * \param[out] display EGL display
                 * \param[out] surfaceWidth EGL surface width
                 * \param[out] surfaceHeight EGL surface height
                 * \return true if current hardware meets minimum requirements and OpenGL ES context has
                 * been successfully created
                 */
                static bool createCompatibleContext(EGLSurface& surface,
                                                    EGLContext& context,
                                                    EGLDisplay& display,
                                                    EGLint& surfaceWidth,
                                                    EGLint& surfaceHeight);

                /**
                 * \brief Destroys context.
                 * \param[in] surface EGL surface
                 * \param[in] context OpenGL ES context
                 * \param[in] display EGL display
                 */
                static void destroyContext(EGLSurface& surface,
                                           EGLContext& context,
                                           EGLDisplay& display);

                /**
                 * \brief Initialization callback.
                 * \param[in] app Android application handle
                 * \param[in] cmd Android command
                 */
                static void initializationCallback(android_app* app, int32_t cmd);

        private:
                friend void ::android_main(android_app* state);
                friend class AndroidApplication;
                friend class GlesRenderer;

                static android_app* state_;
                static uint32_t defaultScreenWidth_;
                static uint32_t defaultScreenHeight_;
                static bool isInitialized_;
                static bool shouldHaltExecution_;

        };

        /**
         * @}
         */

}

#endif
