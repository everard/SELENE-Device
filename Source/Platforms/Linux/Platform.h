// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef PLATFORM_H
#define PLATFORM_H

#include "Rendering/OpenGL/Resources/GLTexture.h"
#include "Rendering/OpenGL/Resources/GLMesh.h"

#include "Rendering/OpenGL/GLRenderer.h"
#include "Application/LinuxApplication.h"
#include "Application/LinuxTimer.h"

namespace selene
{

        /**
         * Represents platform. Holds platform-dependent functions and application entry point.
         */
        class Platform
        {
        public:
                typedef LinuxApplication Application;
                typedef LinuxTimer Timer;

                typedef GlRenderer Renderer;
                typedef GlTexture Texture;
                typedef GlMesh Mesh;

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
                static uint32_t defaultScreenWidth_;
                static uint32_t defaultScreenHeight_;

        };

}

#endif
