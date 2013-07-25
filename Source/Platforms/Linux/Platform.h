// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef PLATFORM_H
#define PLATFORM_H

#include "../../Engine/Core/Resources/Texture/Texture.h"
#include "../../Engine/Core/Resources/Mesh/Mesh.h"

#include "../../Engine/Application/Application.h"
#include "../../Engine/Application/Timer.h"

#include "../../Engine/Rendering/Renderer.h"

#ifndef PLATFORM_LINUX
#define PLATFORM_LINUX
#endif

namespace selene
{

        /**
         * \addtogroup Linux
         * \brief Implementation of platform-dependent layer for Linux OS.
         * @{
         */

        /**
         * Represents platform. Holds platform-dependent functions and application entry point.
         */
        class Platform
        {
        public:
                /**
                 * Represents texture.
                 */
                class Texture: public selene::Texture
                {
                public:
                        /**
                         * \brief Constructs texture with given name.
                         * \param[in] name name of the texture
                         */
                        Texture(const char* name = nullptr);
                        Texture(const Texture&) = delete;
                        ~Texture() = default;
                        Texture& operator =(const Texture&) = delete;

                        // Texture interface implementation
                        bool retain();
                        void discard();

                };

                /**
                 * Represents mesh.
                 */
                class Mesh: public selene::Mesh
                {
                public:
                        /**
                         * \brief Constructs mesh with given name.
                         * \param[in] name name of the mesh
                         */
                        Mesh(const char* name = nullptr);
                        Mesh(const Mesh&) = delete;
                        ~Mesh() = default;
                        Mesh& operator =(const Mesh&) = delete;

                        // Mesh interface implementation
                        bool retain();
                        void discard();

                };

                /**
                 * Represents application.
                 */
                class Application: public selene::Application
                {
                public:
                        /**
                         * \brief Constructs application with given name, width and height of the rendering area.
                         * \param[in] name name of the application
                         * \param[in] width rendering area width in pixels
                         * \param[in] height rendering area height in pixels
                         */
                        Application(const char* name, uint32_t width, uint32_t height);
                        Application(const Application&) = delete;
                        ~Application() = default;
                        Application& operator =(const Application&) = delete;

                        // Application interface implementation
                        bool initialize();
                        bool run();
                        void halt();

                protected:
                        /**
                         * Represents renderer.
                         */
                        class Renderer: public selene::Renderer
                        {
                        public:
                                // Renderer interface implementation
                                bool initialize(const Parameters&);
                                void destroy();
                                void render(const Camera&);

                        private:
                                friend class Application;

                                Renderer() = default;
                                Renderer(const Renderer&) = delete;
                                ~Renderer() = default;
                                Renderer& operator =(const Renderer&) = delete;

                        };

                        // Application interface implementation
                        float getKeyState(uint8_t);

                        Renderer renderer_;

                };

                /**
                 * Represents timer.
                 */
                class Timer: public selene::Timer
                {
                public:
                        Timer() = default;
                        Timer(const Timer&) = default;
                        ~Timer() = default;
                        Timer& operator =(const Timer&) = default;

                        // Timer interface implementation
                        void reset();
                        float getElapsedTime();

                };

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

        /**
         * @}
         */

}

#endif
