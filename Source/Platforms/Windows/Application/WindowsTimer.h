// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef WINDOWS_TIMER_H
#define WINDOWS_TIMER_H

#include "../../../Engine/Application/Timer.h"
#include <Windows.h>

namespace selene
{

        /**
         * \addtogroup Windows
         * @{
         */

        /**
         * Represents Windows timer.
         */
        class WindowsTimer: public Timer
        {
        public:
                WindowsTimer();
                WindowsTimer(const WindowsTimer&) = delete;
                ~WindowsTimer();
                WindowsTimer& operator =(const WindowsTimer&) = delete;

                // Timer interface implementation
                void reset();
                float getElapsedTime();

        private:
                LARGE_INTEGER currentTime_;
                float invFrequency_;
                LONGLONG ticks_;

        };

        /**
         * @}
         */

}

#endif
