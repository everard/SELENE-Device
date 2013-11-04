// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef APPLICATION_H
#define APPLICATION_H

#include "../Core/Entity/Entity.h"
#include "../Core/Math/Vector.h"

namespace selene
{

        /**
         * \addtogroup Application
         * \brief Application and timer.
         * @{
         */

        // Forward declaration of classes
        class Renderer;

        /// Control button. For different platforms has different meanings.
        /// For example, on Windows and Linux platforms this enum represents
        /// states of the mouse buttons.
        enum CONTROL_BUTTON
        {
                CONTROL_BUTTON_0 = 0x01,
                CONTROL_BUTTON_1 = 0x02,
                CONTROL_BUTTON_2 = 0x04,
                CONTROL_BUTTON_3 = 0x08,
                CONTROL_BUTTON_4 = 0x10,
                CONTROL_BUTTON_5 = 0x20,
                CONTROL_BUTTON_6 = 0x40,
                CONTROL_BUTTON_7 = 0x80
        };

        /**
         * Represents generic application.
         * This is base class for all applications (to build new application this class MUST be inherited).
         * Current class contains interface for initialization, running and stopping the application (though
         * it does not provide any implementation, since each platform has its own application class, derived
         * from the current). It also contains different callbacks, which must be implemented in the actual application.
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
                Application(const Application&) = delete;
                virtual ~Application();
                Application& operator =(const Application&) = delete;

                /**
                 * \brief Returns width of the rendering area.
                 * \return width of the rendering area in pixels
                 */
                uint32_t getWidth() const;

                /**
                 * \brief Returns height of the rendering area.
                 * \return height of the rendering area in pixels
                 */
                uint32_t getHeight() const;

                /**
                 * \brief Sets mask of pressed control buttons.
                 * \param[in] mask mask of pressed control buttons
                 */
                void setPressedControlButtonsMask(uint8_t mask);

                /**
                 * \brief Returns mask of pressed control buttons.
                 * \return mask of pressed control buttons
                 */
                uint8_t getPressedControlButtonsMask() const;

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

                /**
                 * \brief Returns renderer.
                 * \return reference to the renderer
                 */
                virtual Renderer& getRenderer() = 0;

                /**
                 * \brief Returns state of the given key.
                 * \param[in] key key
                 * \return state of the key (zero if key is not pressed, one if key is fully pressed)
                 */
                virtual float getKeyState(uint8_t key) const = 0;

                /**
                 * \brief Returns position of the cursor.
                 *
                 * Usually application has only one cursor, but it can have more. For example, some
                 * platforms do not have hardware keyboard and use touchscreen for input. If multi-touch
                 * is supported, then such platforms can have more than one cursor.
                 * \param[in] index index of the cursor
                 * \return position of the cursor
                 */
                virtual Vector2d getCursorPosition(uint8_t index) const = 0;

                /**
                 * \brief Returns shift of the cursor.
                 * \see getCursorPosition
                 * \param[in] index index of the cursor
                 * \return shift of the cursor
                 */
                virtual Vector2d getCursorShift(uint8_t index) const = 0;

                /**
                 * \brief Returns number of cursors.
                 * \return number of cursors
                 */
                virtual uint8_t getNumCursors() const = 0;

        protected:
                /**
                 * \brief Initialization event callback.
                 * \return true if initialization succeeded
                 */
                virtual bool onInitialize() = 0;

                /**
                 * \brief Destruction event callback.
                 */
                virtual void onDestroy() = 0;

                /**
                 * \brief Key press event callback.
                 * \param[in] key pressed key
                 */
                virtual void onKeyPress(uint8_t key) = 0;

                /**
                 * \brief Control button press event callback.
                 * \param[in] button pressed control button (may be used as mask)
                 */
                virtual void onControlButtonPress(uint8_t button) = 0;

                /**
                 * \brief Control button release event callback.
                 * \param[in] button released control button (may be used as mask)
                 */
                virtual void onControlButtonRelease(uint8_t button) = 0;

                /**
                 * \brief Update event callback.
                 * \param[in] elapsedTime elapsed time since last update
                 */
                virtual void onUpdate(float elapsedTime) = 0;

                /**
                 * \brief Rendering event callback.
                 * \param[in] elapsedTime elapsed time since last rendering
                 */
                virtual void onRender(float elapsedTime) = 0;

        private:
                uint32_t width_, height_;
                uint8_t pressedControlButtonsMask_;

        };

        /**
         * @}
         */

}

#endif
