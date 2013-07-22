// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef CIRCLE_H
#define CIRCLE_H

#include "Vector.h"

namespace selene
{

        /**
         * \addtogroup Math
         * @{
         */

        /**
         * Represents circle in 2D space.
         */
        class Circle
        {
        public:
                /**
                 * \brief Constructs circle with given center and radius.
                 * \param[in] center center of the circle
                 * \param[in] radius radius of the circle
                 */
                Circle(const Vector2d& center, float radius);
                Circle(const Circle&) = default;
                ~Circle();
                Circle& operator =(const Circle&) = default;

                /**
                 * \brief Defines circle with given center and radius.
                 * \param[in] center center of the circle
                 * \param[in] radius radius of the circle
                 */
                void define(const Vector2d& center, float radius);

                /**
                 * \brief Sets center.
                 * \param[in] center center of the circle
                 */
                void setCenter(const Vector2d& center);

                /**
                 * \brief Returns center.
                 * \return center of the circle
                 */
                const Vector2d& getCenter() const;

                /**
                 * \brief Sets radius.
                 * \param[in] radius radius of the circle
                 */
                void setRadius(float radius);

                /**
                 * \brief Returns radius.
                 * \return radius of the circle
                 */
                float getRadius() const;

        private:
                Vector2d center_;
                float radius_;

        };

        /**
         * Represents swept circle in 2D space.
         */
        class SweptCircle
        {
        public:
                /**
                 * \brief Constructs swept circle with given center, radius and direction.
                 * \param[in] center center of the swept circle
                 * \param[in] radius radius of the swept circle
                 * \param[in] direction sweep direction (length of this vector defines sweep distance)
                 */
                SweptCircle(const Vector2d& center, float radius,
                            const Vector2d& direction);
                SweptCircle(const SweptCircle&) = default;
                SweptCircle();
                ~SweptCircle();
                SweptCircle& operator =(const SweptCircle&) = default;

                /**
                 * \brief Defines swept circle with given center, radius and direction.
                 * \param[in] center center of the swept circle
                 * \param[in] radius radius of the swept circle
                 * \param[in] direction sweep direction (length of this vector defines sweep distance)
                 */
                void define(const Vector2d& center, float radius,
                            const Vector2d& direction);

                /**
                 * \brief Sets center.
                 * \param[in] center center of the swept circle
                 */
                void setCenter(const Vector2d& center);

                /**
                 * \brief Returns center.
                 * \return center of the swept circle
                 */
                const Vector2d& getCenter() const;

                /**
                 * \brief Sets direction.
                 * \param[in] direction sweep direction (length of this vector defines sweep distance)
                 */
                void setDirection(const Vector2d& direction);

                /**
                 * \brief Returns direction.
                 * \return sweep direction (normalized)
                 */
                const Vector2d& getDirection() const;

                /**
                 * \brief Sets radius.
                 * \param[in] radius radius of the swept circle
                 */
                void setRadius(float radius);

                /**
                 * \brief Returns radius.
                 * \return radius of the swept circle
                 */
                float getRadius() const;

                /**
                 * \brief Returns sweep distance.
                 * \return sweep distance
                 */
                float getSweepDistance() const;

        private:
                Vector2d center_, direction_;
                float radius_, sweepDistance_;

        };

        typedef SweptCircle* PSweptCircle;

        /**
         * @}
         */

}

#endif
