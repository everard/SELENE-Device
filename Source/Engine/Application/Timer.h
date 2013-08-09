// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef TIMER_H
#define TIMER_H

namespace selene
{

        /**
         * \addtogroup Application
         * @{
         */

        /**
         * Represents generic timer.
         * This is base class for all timers. It provides interface for time handling on different platforms.
         * Basically, usage of timers can be described with following code:
         * \code
         * Timer timer;
         * t.reset();
         * doSomething();
         * float elapsedTime = t.getElapsedTime();
         * \endcode
         * Now elapsedTime holds time passed since the start of the doSomething() function.
         * Of course, in examle above class Timer can not be used directly, it should be replaced by the actual
         * Platform::Timer.
         */
        class Timer
        {
        public:
                Timer();
                Timer(const Timer&) = delete;
                virtual ~Timer();
                Timer& operator =(const Timer&) = delete;

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

        /**
         * @}
         */

}

#endif
