// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef PLATFORM_H
#define PLATFORM_H

#include "Rendering/OpenGLES/Resources/GLESTexture.h"
#include "Rendering/OpenGLES/Resources/GLESMesh.h"

#include "Rendering/OpenGLES/GLESRenderer.h"
#include "Application/AndroidApplication.h"
#include "Application/AndroidTimer.h"

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
