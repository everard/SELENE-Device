// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef LINUX_TIMER_H
#define LINUX_TIMER_H

#include "../../../Engine/Core/Macros/Macros.h"
#include "../../../Engine/Application/Timer.h"

#include <ctime>

namespace selene
{

        // Represents linux timer.
        class LinuxTimer: public Timer
        {
        public:
                LinuxTimer();
                ~LinuxTimer();

                // Resets timer
                void reset();

                // Returns elapsed time
                float getElapsedTime();

        private:
                timespec currentTime_;

        };

}

#endif
