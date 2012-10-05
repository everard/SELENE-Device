// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef TIMER_H
#define TIMER_H

namespace selene
{

        /**
         * Represents generic timer.
         * This is base class for all timers.
         */
        class Timer
        {
        public:
                Timer();
                virtual ~Timer();

                /**
                 * \brief Resets timer.
                 */
                virtual void reset() = 0;

                /**
                 * \brief Returns elapsed time since last reset.
                 * \return elapsed time in seconds since last reset
                 */
                virtual float getElapsedTime() = 0;

        };

}

#endif
