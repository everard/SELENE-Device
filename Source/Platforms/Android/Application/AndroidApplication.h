// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef ANDROID_APPLICATION_H
#define ANDROID_APPLICATION_H

#include "../../../Engine/Application/Application.h"
#include "../Rendering/OpenGLES/GLESRenderer.h"
#include "AndroidTimer.h"

#include <android_native_app_glue.h>

namespace selene
{

        /**
         * \addtogroup Android
         * @{
         */

        /**
         * Represents Android application.
         */
        class AndroidApplication: public Application
        {
        public:
                /**
                 * \brief Constructs Android application with given name and size of the rendering area.
                 * \param[in] name name of the application
                 */
                AndroidApplication(const char* name, uint32_t, uint32_t);
                AndroidApplication(const AndroidApplication&) = delete;
                ~AndroidApplication();
                AndroidApplication& operator =(const AndroidApplication&) = delete;

                // Application interface implementation
                bool initialize();
                bool run();
                void halt();
                Renderer& getRenderer();
                float getKeyState(uint8_t) const;
                Vector2d getCursorPosition(uint8_t) const;
                Vector2d getCursorShift(uint8_t) const;
                uint8_t getNumCursors() const;

                /**
                 * \brief Returns Android application handle.
                 * \return Android application handle
                 */
                android_app* getHandle();

        private:
                GlesRenderer renderer_;

                Vector2d cursorPosition_;
                Vector2d cursorShift_;

                android_app* state_;
                AndroidTimer timer_;
                bool shouldRun_, isPaused_, isInitialized_;

                /**
                 * \brief Processes command.
                 *
                 * This method is called from selene::AndroidApplication::commandProcessingCallback.
                 * \param[in] app Android application handle
                 * \param[in] cmd Android command
                 */
                void processCommand(android_app* app, int32_t cmd);

                /**
                 * \brief Processes input event.
                 *
                 * This method is called from selene::AndroidApplication::inputProcessingCallback.
                 * \param[in] app Android application handle
                 * \param[in] event input event
                 * \return result of the event processing (see Android NDK documentation)
                 */
                int32_t processInputEvent(android_app* app, AInputEvent* event);

                /**
                 * \brief Command processing callback.
                 * \param[in] app Android application handle
                 * \param[in] cmd Android command
                 */
                static void commandProcessingCallback(android_app* app, int32_t cmd);

                /**
                 * \brief Input processing callback.
                 * \param[in] app Android application handle
                 * \param[in] event input event
                 * \return result of the event processing (see Android NDK documentation)
                 */
                static int32_t inputProcessingCallback(android_app* app, AInputEvent* event);

        };

        /**
         * @}
         */

}

#endif
