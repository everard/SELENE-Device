// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef WINDOWS_TIMER_H
#define WINDOWS_TIMER_H

#include "../../../Engine/Application/Timer.h"
#include <Windows.h>

namespace selene
{

        // Represents windows timer.
        class WindowsTimer: public Timer
        {
        public:
                WindowsTimer();
                ~WindowsTimer();

                // Resets timer
                void reset();

                // Returns elapsed time
                float getElapsedTime();

        private:
                LARGE_INTEGER currentTime_;
                float invFrequency_;
                LONGLONG ticks_;

        };

}

#endif
