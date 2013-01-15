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

        // Represents android application.
        class AndroidApplication: public Application
        {
        public:
                AndroidApplication(const char* name, uint32_t width, uint32_t height);
                ~AndroidApplication();

                // Initializes application
                bool initialize();

                // Runs application
                bool run();

                // Halts application
                void halt();

                // Returns android application handle
                android_app* getHandle();

        protected:
                GlesRenderer renderer_;

                // Returns state of the given key
                float getKeyState(uint8_t key);

        private:
                android_app* state_;
                AndroidTimer timer_;
                bool shouldRun_, isPaused_, isInitialized_;

                // Processes commands and input
                void processCommand(android_app* app, int32_t cmd);
                int32_t processInputEvent(android_app* app, AInputEvent* event);

                // Android callbacks
                static void commandProcessingCallback(android_app* app, int32_t cmd);
                static int32_t inputProcessingCallback(android_app* app, AInputEvent* event);

        };

}

#endif
