// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "CollisionManager.h"
#include <limits>

namespace selene
{

        CollisionManager::Geometry::Geometry():
                lineSegments(nullptr), sweptCircles(nullptr) {}
        CollisionManager::Geometry::~Geometry() {}

        CollisionManager::CollisionManager():
                geometry_(), maxRecursion_(0), recursion_(0), radius_(0.0f) {}
        CollisionManager::~CollisionManager() {}

        //-----------------------------------------------------------------------------------------------------------
        bool CollisionManager::determineIntersection(const Ray2d& ray, float& distanceToIntersection,
                                                     CollisionManager::Geometry geometry)
        {
                if(geometry.lineSegments == nullptr)
                        return false;

                std::vector<PLineSegment2d>& lineSegments = *geometry.lineSegments;

                float distance;
                bool intersects = false;
                distanceToIntersection = std::numeric_limits<float>::max();

                // walk through all geometry
                for(auto it = lineSegments.begin(); it != lineSegments.end(); ++it)
                {
                        LineSegment2d& lineSegment = *(*it);
                        if(ray.intersects(lineSegment, distance))
                        {
                                if(distance < 0.0f)
                                        continue;

                                if(distance < distanceToIntersection)
                                {
                                        distanceToIntersection = distance;
                                        intersects = true;
                                }
                        }
                }

                return intersects;
        }

        //-----------------------------------------------------------------------------------------------------------
        Vector2d CollisionManager::move(const SweptCircle& sweptCircle,
                                        CollisionManager::Geometry geometry,
                                        uint8_t maxRecursion)
        {
                if(sweptCircle.getSweepDistance() < SELENE_EPSILON)
                        return sweptCircle.getCenter();

                geometry_ = geometry;
                if(geometry_.lineSegments == nullptr && geometry_.sweptCircles == nullptr)
                        return sweptCircle.getCenter() + sweptCircle.getDirection() * sweptCircle.getSweepDistance();

                maxRecursion_ = maxRecursion;
                recursion_ = 0;
                radius_ = sweptCircle.getRadius();
                Vector2d position = move(sweptCircle.getCenter() / radius_, sweptCircle.getDirection(),
                                         sweptCircle.getSweepDistance() / radius_);
                position *= radius_;
                return position;
        }

        //-----------------------------------------------------------------------------------------------------------
        bool CollisionManager::determineCollision(const Vector2d& position,
                                                  const Vector2d& velocity,
                                                  const SweptCircle& sweptCircle,
                                                  Vector2d& intersectionPoint,
                                                  float& distanceToIntersection)
        {
                float sweepDistance = sweptCircle.getSweepDistance() / radius_;
                float sweptRadius = sweptCircle.getRadius() / radius_;
                Vector2d sweptCenter = sweptCircle.getCenter() / radius_;
                if(sweepDistance < SELENE_EPSILON)
                {
                        // swept circle is actually a circle
                        Circle circle(sweptCenter, sweptRadius);
                        return determineCollision(position, velocity, circle,
                                                  intersectionPoint, distanceToIntersection);
                }

                // compute bounding geometry: 2 circles and 2 line segments
                Vector2d destination = sweptCenter + sweptCircle.getDirection() * sweepDistance;
                Vector2d vertices[2] = {sweptCenter, destination};
                Line2d connector(vertices);
                const Vector2d& normal = connector.getNormal();

                // circles
                Circle circles[2] = {Circle(sweptCenter, sweptRadius), Circle(destination, sweptRadius)};

                // line segments
                LineSegment2d lineSegments[2];
                vertices[0] = sweptCenter + normal * sweptRadius;
                vertices[1] = destination + normal * sweptRadius;
                lineSegments[0].define(vertices);

                vertices[0] = destination - normal * sweptRadius;
                vertices[1] = sweptCenter - normal * sweptRadius;
                lineSegments[1].define(vertices);

                // perform collision detection
                Vector2d geometryIntersectionPoint;
                float distance;
                bool collides = false;
                distanceToIntersection = std::numeric_limits<float>::max();

                // line segments
                for(uint32_t i = 0; i < 2; ++i)
                {
                        if(!determineCollision(position, velocity, lineSegments[i],
                                               geometryIntersectionPoint, distance))
                                continue;

                        if(distance >= distanceToIntersection)
                                continue;

                        intersectionPoint = geometryIntersectionPoint;
                        distanceToIntersection = distance;
                        collides = true;
                }

                // circles
                for(uint32_t i = 0; i < 2; ++i)
                {
                        if(!determineCollision(position, velocity, circles[i],
                                               geometryIntersectionPoint, distance))
                                continue;

                        if(distance >= distanceToIntersection)
                                continue;

                        intersectionPoint = geometryIntersectionPoint;
                        distanceToIntersection = distance;
                        collides = true;
                }

                return collides;
        }

        //-----------------------------------------------------------------------------------------------------------
        bool CollisionManager::determineCollision(const Vector2d& position,
                                                  const Vector2d& velocity,
                                                  const Circle& circle,
                                                  Vector2d& intersectionPoint,
                                                  float& distanceToIntersection)
        {
                float radiusSum = circle.getRadius() + 1.0f;
                Vector2d differenceVector = position - circle.getCenter();
                float difference = differenceVector.dot(differenceVector);
                if(difference < (radiusSum * radiusSum - SELENE_EPSILON))
                        return false;

                Line2d line(-velocity, circle.getCenter());

                float distanceToLine = line.distance(position);
                if(distanceToLine < 0.0f)
                        return false;

                Vector2d lineIntersectionPoint = position + velocity * distanceToLine;
                Vector2d edge = lineIntersectionPoint - circle.getCenter();

                float edgeLength = edge.length();
                if(edgeLength > radiusSum)
                        return false;

                if(edgeLength > SELENE_EPSILON)
                {
                        float d = circle.getRadius() * edgeLength / radiusSum;
                        float a = circle.getRadius() * circle.getRadius() - d * d + SELENE_EPSILON;
                        if(a <= 0.0f)
                                return false;

                        a = sqrt(a);

                        edge.normalize();

                        lineIntersectionPoint = circle.getCenter() + edge * (d + SELENE_EPSILON);

                        Ray2d ray(lineIntersectionPoint, -velocity);
                        Circle collidingCircle(position, 1.0f);
                        if(!ray.intersects(collidingCircle, distanceToLine))
                                return false;

                        distanceToLine -= a;
                        distanceToIntersection = distanceToLine + SELENE_EPSILON;
                        intersectionPoint = lineIntersectionPoint - velocity * (a + SELENE_EPSILON);
                }
                else
                {
                        distanceToLine -= radiusSum;
                        if(distanceToLine < -SELENE_EPSILON)
                                return false;

                        distanceToIntersection = distanceToLine;
                        intersectionPoint = circle.getCenter() - velocity * circle.getRadius();
                }

                return true;
        }

        //-----------------------------------------------------------------------------------------------------------
        bool CollisionManager::determineCollision(const Vector2d& position,
                                                  const Vector2d& velocity,
                                                  const LineSegment2d& lineSegment,
                                                  Vector2d& intersectionPoint,
                                                  float& distanceToIntersection)
        {
                const Line2d& line = lineSegment.getLine();
                Vector2d circleIntersectionPoint;
                Vector2d lineIntersectionPoint;

                // ignore backfacing
                if(line.getNormal().dot(velocity) >= 0.0f)
                        return false;

                // compute circle intersection point
                circleIntersectionPoint = position - line.getNormal();

                // compute distance to line from circle intersection point
                float distanceToLine = line.distance(circleIntersectionPoint);
                if(distanceToLine < 0.0f)
                {
                        Circle circle(position, 1.0f);
                        lineIntersectionPoint = lineSegment.getClosestVertex(circleIntersectionPoint);
                        Ray2d velocityRay(lineIntersectionPoint, -velocity);
                        if(!velocityRay.intersects(circle, distanceToLine))
                                return false;
                }
                else
                {
                        Ray2d velocityRay(circleIntersectionPoint, velocity);

                        // determine intersection between line and velocity ray
                        if(!velocityRay.intersects(line, distanceToLine))
                                return false;

                        lineIntersectionPoint = circleIntersectionPoint + velocity * distanceToLine;
                        if(lineSegment.determineRelation(lineIntersectionPoint) != INSIDE)
                        {
                                Circle circle(position, 1.0f);
                                lineIntersectionPoint = lineSegment.getClosestVertex(lineIntersectionPoint);
                                velocityRay.define(lineIntersectionPoint, -velocity);

                                if(!velocityRay.intersects(circle, distanceToLine))
                                        return false;
                        }
                }

                if(distanceToLine < 0.0f)
                        return false;

                distanceToIntersection = distanceToLine;
                intersectionPoint = lineIntersectionPoint;
                return true;
        }

        //-----------------------------------------------------------------------------------------------------------
        bool CollisionManager::determineCollision(const Vector2d& position,
                                                  const Vector2d& velocity,
                                                  float distanceToTravel,
                                                  Vector2d& intersectionPoint,
                                                  float& distanceToIntersection)
        {
                Vector2d geometryIntersectionPoint, vertices[2];
                float distance;
                bool collides = false;
                distanceToIntersection = std::numeric_limits<float>::max();

                if(geometry_.lineSegments != nullptr)
                {
                        std::vector<PLineSegment2d>& lineSegments = *geometry_.lineSegments;

                        // walk through all line segments
                        for(auto it = lineSegments.begin(); it != lineSegments.end(); ++it)
                        {
                                LineSegment2d& originalLineSegment = *(*it);

                                // convert line segment to circle space
                                vertices[0] = originalLineSegment.getVertices()[0] / radius_;
                                vertices[1] = originalLineSegment.getVertices()[1] / radius_;
                                LineSegment2d lineSegment(vertices);

                                if(!determineCollision(position, velocity, lineSegment,
                                                       geometryIntersectionPoint, distance))
                                        continue;

                                if(distance > distanceToTravel || distance >= distanceToIntersection)
                                        continue;

                                intersectionPoint = geometryIntersectionPoint;
                                distanceToIntersection = distance;
                                collides = true;
                        }
                }

                if(geometry_.sweptCircles != nullptr)
                {
                        std::vector<PSweptCircle>& sweptCircles = *geometry_.sweptCircles;

                        // walk through all swept circles
                        for(auto it = sweptCircles.begin(); it != sweptCircles.end(); ++it)
                        {
                                SweptCircle& sweptCircle = *(*it);

                                if(!determineCollision(position, velocity, sweptCircle,
                                                       geometryIntersectionPoint, distance))
                                        continue;

                                if(distance > distanceToTravel || distance >= distanceToIntersection)
                                        continue;

                                intersectionPoint = geometryIntersectionPoint;
                                distanceToIntersection = distance;
                                collides = true;
                        }
                }

                return collides;
        }

        //-----------------------------------------------------------------------------------------------------------
        Vector2d CollisionManager::move(const Vector2d& position,
                                        const Vector2d& velocity,
                                        float distanceToTravel)
        {
                if(recursion_ > maxRecursion_)
                        return position;

                ++recursion_;

                Vector2d intersectionPoint;
                Vector2d destination = position + velocity * distanceToTravel;
                float distance;

                // determine colision
                if(!determineCollision(position, velocity, distanceToTravel,
                                       intersectionPoint, distance))
                        return destination;

                // move close to line segment
                Vector2d newPosition = position + velocity * (distance - SELENE_EPSILON);

                // compute sliding line
                Vector2d lineNormal = newPosition - intersectionPoint;
                lineNormal.normalize();
                Line2d slidingLine(lineNormal, intersectionPoint);

                // project velocity to the sliding line
                float d = slidingLine.distance(destination);
                Vector2d newVelocity = destination - lineNormal * d - intersectionPoint;

                distanceToTravel = newVelocity.length();
                if(distanceToTravel > SELENE_EPSILON)
                        newVelocity.normalize();
                else
                        return newPosition;

                return move(newPosition, newVelocity, distanceToTravel);
        }

}
