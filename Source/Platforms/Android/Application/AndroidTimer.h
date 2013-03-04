// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef ANDROID_TIMER_H
#define ANDROID_TIMER_H

#include "../../../Engine/Core/Macros/Macros.h"
#include "../../../Engine/Application/Timer.h"

#include <ctime>

namespace selene
{

        /**
         * \addtogroup Android
         * @{
         */

        /**
         * Represents Android timer.
         */
        class AndroidTimer: public Timer
        {
        public:
                AndroidTimer();
                ~AndroidTimer();

                // Timer interface implementation
                void reset();
                float getElapsedTime();

        private:
                timespec currentTime_;

        };

        /**
         * @}
         */

}

#endif
