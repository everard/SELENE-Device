// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "LinuxTimer.h"
#include <ctime>

namespace selene
{

        LinuxTimer::LinuxTimer()
        {
                timespec ts;
                clock_gettime(CLOCK_REALTIME, &ts);
                currentTime_ = static_cast<uint64_t>(ts.tv_sec) * 1000000000 + ts.tv_nsec;
        }
        LinuxTimer::~LinuxTimer() {}

        //--------------------------------
        void LinuxTimer::reset()
        {
                timespec ts;
                clock_gettime(CLOCK_REALTIME, &ts);
                currentTime_ = static_cast<uint64_t>(ts.tv_sec) * 1000000000 + ts.tv_nsec;
        }

        //--------------------------------
        float LinuxTimer::getElapsedTime()
        {
                timespec ts;
                clock_gettime(CLOCK_REALTIME, &ts);
                uint64_t time = static_cast<uint64_t>(ts.tv_sec) * 1000000000 + ts.tv_nsec;

                return static_cast<float>(static_cast<double>(time - currentTime_) * 0.000000001);
        }

}
