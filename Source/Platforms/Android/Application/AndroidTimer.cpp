// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "AndroidTimer.h"

namespace selene
{

        AndroidTimer::AndroidTimer(): currentTime_()
        {
                clock_gettime(CLOCK_MONOTONIC, &currentTime_);
        }
        AndroidTimer::~AndroidTimer() {}

        //-------------------------------------------------------------------------------------------------
        void AndroidTimer::reset()
        {
                clock_gettime(CLOCK_MONOTONIC, &currentTime_);
        }

        //-------------------------------------------------------------------------------------------------
        float AndroidTimer::getElapsedTime()
        {
                timespec ts;
                clock_gettime(CLOCK_MONOTONIC, &ts);

                ts.tv_sec -= currentTime_.tv_sec;
                double delta = static_cast<double>(ts.tv_nsec) - static_cast<double>(currentTime_.tv_nsec);
                delta *= 0.000000001;

                return static_cast<float>(ts.tv_sec) + static_cast<float>(delta);
        }

}
