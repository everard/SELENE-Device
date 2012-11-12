// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "LinuxTimer.h"

namespace selene
{

        LinuxTimer::LinuxTimer()
        {
                clock_gettime(CLOCK_MONOTONIC, &currentTime_);
        }
        LinuxTimer::~LinuxTimer() {}

        //--------------------------------
        void LinuxTimer::reset()
        {
                clock_gettime(CLOCK_MONOTONIC, &currentTime_);
        }

        //--------------------------------
        float LinuxTimer::getElapsedTime()
        {
                timespec ts;
                clock_gettime(CLOCK_MONOTONIC, &ts);

                ts.tv_sec -= currentTime_.tv_sec;
                double delta = static_cast<double>(ts.tv_nsec) - static_cast<double>(currentTime_.tv_nsec);
                delta *= 0.000000001;

                return static_cast<float>(ts.tv_sec) + static_cast<float>(delta);
        }

}
