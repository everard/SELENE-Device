// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef APPLICATION_H
#define APPLICATION_H

#include "../Core/Entity/Entity.h"
#include "../Core/Math/Vector.h"

namespace selene
{

        /// Mouse buttons
        enum
        {
                MOUSE_BUTTON_LEFT   = 0x01,
                MOUSE_BUTTON_RIGHT  = 0x02,
                MOUSE_BUTTON_MIDDLE = 0x04
        };

        /**
         * Represents generic application.
         * This is base class for all applications (to build new application this class MUST be inherited).
         */
        class Application: public Entity
        {
        public:
                /**
                 * \brief Constructs application with given name, width and height of the rendering area.
                 * \param[in] name name of the application
                 * \param[in] width rendering area width in pixels
                 * \param[in] height rendering area height in pixels
                 */
                Application(const char* name, uint32_t width, uint32_t height);
                virtual ~Application();

                /**
                 * \brief Initializes application.
                 *
                 * Performs all needed operations e.g. rendering and window initialization, etc.
                 * \return true if initialization succeeded
                 */
                virtual bool initialize() = 0;

                /**
                 * \brief Runs application. Starts main loop.
                 * \return true if application successfully ended its work
                 */
                virtual bool run() = 0;

                /**
                 * \brief Halts application. Stops main loop.
                 */
                virtual void halt() = 0;

        protected:
                // Width and height of the rendering area
                uint32_t width_, height_;

                // Cursor position
                Vector2d cursorPosition_;

                // Control buttons state
                uint8_t pressedControlButtons_;

                /**
                 * \brief Returns state of the given key.
                 * \param[in] key key
                 * \return state of the key (zero if key is not pressed, one if key is fully pressed)
                 */
                virtual float getKeyState(uint8_t key) = 0;

                /**
                 * \brief Initialization event callback.
                 * \return true if initialization succeded
                 */
                virtual bool onInitialize();

                /**
                 * \brief Destruction event callback.
                 */
                virtual void onDestroy();

                /**
                 * \brief Key press event callback.
                 * \param[in] key pressed key
                 */
                virtual void onKeyPress(uint8_t key);

                /**
                 * \brief Control button press event callback.
                 * \param[in] button pressed control button (may be used as mask)
                 */
                virtual void onControlButtonPress(uint8_t button);

                /**
                 * \brief Control button release event callback.
                 * \param[in] button released control button (may be used as mask)
                 */
                virtual void onControlButtonRelease(uint8_t button);

                /**
                 * \brief Update event callback.
                 * \param[in] elapsedTime elapsed time since last update
                 */
                virtual void onUpdate(float elapsedTime);

                /**
                 * \brief Rendering event callback.
                 * \param[in] elapsedTime elapsed time since last rendering
                 */
                virtual void onRender(float elapsedTime);

        };

}

#endif
