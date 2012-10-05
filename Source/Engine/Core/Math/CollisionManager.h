// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef COLLISION_MANAGER_H
#define COLLISION_MANAGER_H

#include "Circle.h"
#include "Line.h"
#include "Ray.h"
#include <vector>

namespace selene
{

        /**
         * Represents collision manager.
         */
        class CollisionManager
        {
        public:
                /**
                 * Represents geometry.
                 */
                class Geometry
                {
                public:
                        std::vector<PLineSegment2d>* lineSegments;
                        std::vector<PSweptCircle>* sweptCircles;

                        Geometry();
                        ~Geometry();

                };

                CollisionManager();
                ~CollisionManager();

                /**
                 * \brief Determines intersection between ray and geometry.
                 * \param[in] ray ray
                 * \param[out] distanceToIntersection distance to the intersection point
                 * \param[in] geometry geometry
                 * \return true if ray intersects the geometry
                 */
                bool determineIntersection(const Ray2d& ray, float& distanceToIntersection,
                                           Geometry geometry);

                /**
                 * \brief Moves object represented by swept circle through geometry.
                 * \param[in] sweptCircle swept circle which represents moving object
                 * \param[in] geometry geometry
                 * \param[in] maxRecursion maximum recursion during movement of the object
                 * (if set to zero, then object does not slide at all)
                 * \return new position of the object
                 */
                Vector2d move(const SweptCircle& sweptCircle,
                              Geometry geometry, uint8_t maxRecursion);

        private:
                // Geometry
                Geometry geometry_;

                // Recursion
                uint8_t maxRecursion_;
                uint8_t recursion_;

                // Radius
                float radius_;

                /**
                 * \brief Determines collision between object and swept circle.
                 * \param[in] position current position of the object
                 * \param[in] velocity current velocity of the object
                 * \param[in] sweptCircle swept circle
                 * \param[out] intersectionPoint point of intersection
                 * \param[out] distanceToIntersection distance to the intersection point
                 * \return true if object collides with swept circle
                 */
                bool determineCollision(const Vector2d& position,
                                        const Vector2d& velocity,
                                        const SweptCircle& sweptCircle,
                                        Vector2d& intersectionPoint,
                                        float& distanceToIntersection);

                /**
                 * \brief Determines collision between object and circle.
                 * \param[in] position current position of the object
                 * \param[in] velocity current velocity of the object
                 * \param[in] circle circle
                 * \param[out] intersectionPoint point of intersection
                 * \param[out] distanceToIntersection distance to the intersection point
                 * \return true if object collides with circle
                 */
                bool determineCollision(const Vector2d& position,
                                        const Vector2d& velocity,
                                        const Circle& circle,
                                        Vector2d& intersectionPoint,
                                        float& distanceToIntersection);

                /**
                 * \brief Determines collision between object and line segment.
                 * \param[in] position current position of the object
                 * \param[in] velocity current velocity of the object
                 * \param[in] lineSegment line segment
                 * \param[out] intersectionPoint point of intersection
                 * \param[out] distanceToIntersection distance to the intersection point
                 * \return true if object collides with line segment
                 */
                bool determineCollision(const Vector2d& position,
                                        const Vector2d& velocity,
                                        const LineSegment2d& lineSegment,
                                        Vector2d& intersectionPoint,
                                        float& distanceToIntersection);

                /**
                 * \brief Determines collision between object and geometry.
                 * \param[in] position current position of the object
                 * \param[in] velocity current velocity of the object (normalized)
                 * \param[in] distanceToTravel distance which object must travel with the given velocity
                 * \param[out] intersectionPoint point of intersection
                 * \param[out] distanceToIntersection distance to the intersection point
                 * \return true if object collides with geometry
                 */
                bool determineCollision(const Vector2d& position,
                                        const Vector2d& velocity,
                                        float distanceToTravel,
                                        Vector2d& intersectionPoint,
                                        float& distanceToIntersection);

                /**
                 * \brief Moves object through geometry.
                 * \param[in] position current position of the object
                 * \param[in] velocity current velocity of the object (normalized)
                 * \param[in] distanceToTravel distance which object must travel with the given velocity
                 * \return new position of the object
                 */
                Vector2d move(const Vector2d& position,
                              const Vector2d& velocity,
                              float distanceToTravel);

        };

}

#endif
