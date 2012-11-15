// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef ANDROID_APPLICATION_H
#define ANDROID_APPLICATION_H

#include "../../../Engine/Application/Application.h"
#include "AndroidTimer.h"

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

        protected:
                // Returns state of the given key
                float getKeyState(uint8_t key);

        };

}

#endif
