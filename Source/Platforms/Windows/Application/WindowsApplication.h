// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef WINDOWS_APPLICATION_H
#define WINDOWS_APPLICATION_H

#include "../../../Engine/Application/Application.h"
#include "../Rendering/Direct3D9/D3D9Renderer.h"
#include "WindowsTimer.h"

namespace selene
{

        /**
         * \addtogroup Windows
         * @{
         */

        /**
         * Represents Windows application.
         */
        class WindowsApplication: public Application
        {
        public:
                /**
                 * \brief Constructs Windows application with given name and size of the rendering area.
                 * \param[in] name name of the application
                 * \param[in] width width of the rendering area in pixels
                 * \param[in] height height of the rendering area in pixels
                 */
                WindowsApplication(const char* name, uint32_t width, uint32_t height);
                WindowsApplication(const WindowsApplication&) = delete;
                ~WindowsApplication();
                WindowsApplication& operator =(const WindowsApplication&) = delete;

                // Application interface implementation
                bool initialize();
                bool run();
                void halt();
                Renderer& getRenderer();
                float getKeyState(uint8_t key) const;
                Vector2d getCursorPosition(uint8_t) const;
                Vector2d getCursorShift(uint8_t) const;
                uint8_t getNumCursors() const;

                /**
                 * \brief Returns window handle.
                 * \return handle to the application window
                 */
                HWND getWindowHandle();

        protected:
                /**
                 * \brief Windows message receive event callback.
                 *
                 * May be overridden in actual application, but doing so will make application
                 * platform-dependent.
                 * \param[in] message message code
                 * \param[in] wParam WPARAM of the Windows message
                 * \param[in] lParam LPARAM of the Windows message
                 * \return result of the Windows message processing
                 */
                virtual LRESULT onMessage(UINT message, WPARAM wParam, LPARAM lParam);

        private:
                std::string windowClassName_;
                HINSTANCE hInstance_;
                HWND hWnd_;

                Vector2d cursorPosition_;
                Vector2d cursorShift_;

                D3d9Renderer renderer_;
                bool isActive_;

                /**
                 * \brief Window procedure. Calls WindowsApplication::processMessages.
                 * \param[in] hWnd window handle
                 * \param[in] message message code
                 * \param[in] wParam WPARAM of the Windows message
                 * \param[in] lParam LPARAM of the Windows message
                 * \return result of the Windows message processing
                 */
                static LRESULT CALLBACK windowProcedure(HWND hWnd, UINT message,
                                                        WPARAM wParam, LPARAM lParam);

                /**
                 * \brief Registers window class.
                 * \return non-zero value if window class has been successfully registered
                 */
                ATOM registerWindowClass();

                /**
                 * \brief Destroys application.
                 */
                void destroy();

                /**
                 * \brief Processes Windows messages.
                 * \param[in] message message code
                 * \param[in] wParam WPARAM of the Windows message
                 * \param[in] lParam LPARAM of the Windows message
                 * \return result of the Windows message processing
                 */
                LRESULT processMessages(UINT message, WPARAM wParam, LPARAM lParam);

        };

        /**
         * @}
         */

}

#endif
