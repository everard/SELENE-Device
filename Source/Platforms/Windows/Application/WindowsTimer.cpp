// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "WindowsTimer.h"

namespace selene
{

        WindowsTimer::WindowsTimer()
        {
                QueryPerformanceFrequency(&currentTime_);
                invFrequency_ = 1.0f / (float)currentTime_.QuadPart;
                QueryPerformanceCounter(&currentTime_);
                ticks_ = currentTime_.QuadPart;
        }
        WindowsTimer::~WindowsTimer() {}

        //---------------------------------------------------------------------
        void WindowsTimer::reset()
        {
                QueryPerformanceCounter(&currentTime_);
                ticks_ = currentTime_.QuadPart;
        }

        //---------------------------------------------------------------------
        float WindowsTimer::getElapsedTime()
        {
                QueryPerformanceCounter(&currentTime_);
                return (float)(currentTime_.QuadPart - ticks_) * invFrequency_;
        }

}
