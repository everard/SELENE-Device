// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef LINUX_APPLICATION_H
#define LINUX_APPLICATION_H

#include "../../../Engine/Application/Application.h"
#include "LinuxTimer.h"

namespace selene
{

        // Represents linux application.
        class LinuxApplication: public Application
        {
        public:
                LinuxApplication(const char* name, uint32_t width, uint32_t height);
                ~LinuxApplication();

                // Initializes application
                bool initialize();

                // Runs application
                bool run();

                // Halts application
                void halt();

        protected:
                // Returns state of the given key
                float getKeyState(uint8_t key);

        private:
                // Destroys window
                void destroy();

        };

}

#endif
