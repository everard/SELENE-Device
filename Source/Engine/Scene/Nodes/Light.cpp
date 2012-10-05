// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Light.h"

#include "../../Rendering/Renderer.h"

namespace selene
{

        Light::Light(const char* name): Scene::Node(name)
        {
                intensity_ = 1.0f;
        }
        Light::~Light() {}

        //----------------------------------------------------------------------
        const Volume& Light::getVolume() const
        {
                performUpdateOperation();
                return volume_;
        }

        //----------------------------------------------------------------------
        void Light::setColor(const Vector3d& color)
        {
                color_ = color;
        }

        //----------------------------------------------------------------------
        const Vector3d& Light::getColor() const
        {
                return color_;
        }

        //----------------------------------------------------------------------
        void Light::setIntensity(float intensity)
        {
                intensity_ = intensity;
        }

        //----------------------------------------------------------------------
        float Light::getIntensity() const
        {
                return intensity_;
        }

        DirectionalLight::DirectionalLight(const char* name,
                                           const Vector3d& direction,
                                           const Vector3d& color,
                                           float intensity,
                                           float size): Light(name)
        {
                setDirection(direction);
                color_ = color;

                intensity_ = intensity;
                size_ = size;
        }
        DirectionalLight::~DirectionalLight() {}

        //----------------------------------------------------------------------
        void DirectionalLight::setDirection(const Vector3d& direction)
        {
                direction_ = direction;

                if(direction_.length() > SELENE_EPSILON)
                        direction_.normalize();

                requestUpdateOperation();
        }

        //----------------------------------------------------------------------
        const Vector3d& DirectionalLight::getDirection() const
        {
                return direction_;
        }

        //----------------------------------------------------------------------
        void DirectionalLight::setSize(float size)
        {
                size_ = size;
                requestUpdateOperation();
        }

        //----------------------------------------------------------------------
        float DirectionalLight::getSize() const
        {
                return size_;
        }

        //----------------------------------------------------------------------
        int16_t DirectionalLight::getRenderingUnit() const
        {
                return Renderer::UNIT_LIGHT_DIRECTIONAL;
        }

        //----------------------------------------------------------------------
        RELATION DirectionalLight::determineRelation(const Volume& volume) const
        {
                if(is(HIDDEN))
                        return OUTSIDE;

                volume.getNumPlanes();

                return INSIDE;
        }

        //----------------------------------------------------------------------
        void DirectionalLight::update() const {}

        PointLight::PointLight(const char* name,
                               const Vector3d& position,
                               const Vector3d& color,
                               float intensity,
                               float radius): Light(name)
        {
                positions_[ORIGINAL] = position;
                color_ = color;

                intensity_ = intensity;
                boundingSphere_.setRadius(radius);
        }
        PointLight::~PointLight() {}

        //----------------------------------------------------------------------
        void PointLight::setRadius(float radius)
        {
                boundingSphere_.setRadius(radius);
                requestUpdateOperation();
        }

        //----------------------------------------------------------------------
        float PointLight::getRadius() const
        {
                return boundingSphere_.getRadius();
        }

        //----------------------------------------------------------------------
        int16_t PointLight::getRenderingUnit() const
        {
                return Renderer::UNIT_LIGHT_POINT;
        }

        //----------------------------------------------------------------------
        RELATION PointLight::determineRelation(const Volume& volume) const
        {
                if(is(HIDDEN))
                        return OUTSIDE;

                performUpdateOperation();

                return boundingSphere_.determineRelation(volume);
        }

        //----------------------------------------------------------------------
        void PointLight::update() const
        {
                // update bounding sphere
                boundingSphere_.setCenter(positions_[MODIFIED]);

                // update light volume
                float radius = boundingSphere_.getRadius();

                static const uint8_t numPlanes = 6;
                static const Vector3d planeNormals[numPlanes] =
                {
                        Vector3d(-1.0f, 0.0f, 0.0f),
                        Vector3d( 1.0f, 0.0f, 0.0f),

                        Vector3d(0.0f, -1.0f, 0.0f),
                        Vector3d(0.0f,  1.0f, 0.0f),

                        Vector3d(0.0f, 0.0f, -1.0f),
                        Vector3d(0.0f, 0.0f,  1.0f)
                };

                Vector3d planeOffsets[numPlanes] =
                {
                        Vector3d( radius, 0.0f, 0.0f),
                        Vector3d(-radius, 0.0f, 0.0f),

                        Vector3d(0.0f,  radius, 0.0f),
                        Vector3d(0.0f, -radius, 0.0f),

                        Vector3d(0.0f, 0.0f,  radius),
                        Vector3d(0.0f, 0.0f, -radius)
                };

                Plane planes[numPlanes];

                for(uint8_t i = 0; i < numPlanes; ++i)
                        planes[i].define(planeNormals[i], planeOffsets[i]);

                volume_.define(planes, numPlanes);
        }

        SpotLight::SpotLight(const char* name,
                             const Vector3d& position,
                             const Vector3d& direction,
                             const Vector3d& color,
                             float intensity,
                             float radius): Light(name)
        {
                positions_[ORIGINAL]  = position;
                directions_[ORIGINAL] = direction;
                color_ = color;

                intensity_ = intensity;
                boundingCone_.setRadius(radius);
        }
        SpotLight::~SpotLight() {}

        //----------------------------------------------------------------------
        void SpotLight::setDirection(const Vector3d& direction)
        {
                directions_[ORIGINAL] = direction;
                requestUpdateOperation();
        }

        //----------------------------------------------------------------------
        const Vector3d& SpotLight::getDirection() const
        {
                performUpdateOperation();
                return directions_[MODIFIED];
        }

        //----------------------------------------------------------------------
        void SpotLight::setRadius(float radius)
        {
                boundingCone_.setRadius(radius);
                requestUpdateOperation();
        }

        //----------------------------------------------------------------------
        float SpotLight::getRadius() const
        {
                return boundingCone_.getRadius();
        }

        //----------------------------------------------------------------------
        const Vector4d& SpotLight::getProjectionParameters() const
        {
                performUpdateOperation();
                return projectionParameters_;
        }

        //----------------------------------------------------------------------
        const Matrix& SpotLight::getViewProjectionMatrix() const
        {
                performUpdateOperation();
                return viewProjectionMatrix_;
        }

        //----------------------------------------------------------------------
        const Matrix& SpotLight::getViewMatrix() const
        {
                performUpdateOperation();
                return viewMatrix_;
        }

        //----------------------------------------------------------------------
        float SpotLight::getCosTheta() const
        {
                performUpdateOperation();
                return cosTheta_;
        }

        //----------------------------------------------------------------------
        int16_t SpotLight::getRenderingUnit() const
        {
                return Renderer::UNIT_LIGHT_SPOT;
        }

        //----------------------------------------------------------------------
        RELATION SpotLight::determineRelation(const Volume& volume) const
        {
                if(is(HIDDEN))
                        return OUTSIDE;

                performUpdateOperation();

                return boundingCone_.determineRelation(volume);
        }

        //----------------------------------------------------------------------
        void SpotLight::update() const
        {
                // compute direction
                Quaternion directionQuaternion = rotations_[MODIFIED] * Quaternion(directions_[ORIGINAL]);
                directions_[MODIFIED].define(directionQuaternion.x,
                                             directionQuaternion.y,
                                             directionQuaternion.z);

                // update bounding cone
                boundingCone_.setApex(positions_[MODIFIED]);
                boundingCone_.setDirection(directions_[MODIFIED]);

                // compute matrices and other parameters
                Vector3d direction = directions_[MODIFIED];
                float directionLength = direction.length();

                if(directionLength > SELENE_EPSILON)
                        direction.normalize();
                else
                        directionLength = 1.0f;

                float tanTheta = boundingCone_.getRadius() / directionLength;
                cosTheta_ = sqrt(1.0f / (1.0f + tanTheta * tanTheta));

                float fov = 2.0f * atan(tanTheta) * (180.0f / SELENE_PI);

                Matrix projectionMatrix;
                Vector3d upVector;

                if(fabs(direction.x) < SELENE_EPSILON && fabs(direction.y) < SELENE_EPSILON)
                        upVector = Vector3d(0.0f, 1.0f, 0.0f) * direction;
                else
                        upVector = Vector3d(0.0f, 0.0f, 1.0f) * direction;

                viewMatrix_.lookAt(positions_[MODIFIED],
                                   positions_[MODIFIED] + direction, upVector);

                projectionParameters_.define(fov, 1.0f, 0.1f, directionLength);

                projectionMatrix.perspective(projectionParameters_.x, projectionParameters_.y,
                                             projectionParameters_.z, projectionParameters_.w);

                viewProjectionMatrix_ = viewMatrix_ * projectionMatrix;

                volume_.define(viewProjectionMatrix_);
        }

}
