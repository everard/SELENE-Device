// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Camera.h"
#include <utility>

namespace selene
{

        Camera::Camera(const char* name,
                       const Renderer& renderer,
                       const Vector3d& position,
                       const Vector3d& direction,
                       const Vector3d& upVector,
                       const Vector4d& projectionParameters,
                       float distance,
                       Gui* gui):
                Scene::Node(name), projectionMatrix_(), projectionInvMatrix_(), viewProjectionMatrix_(), viewMatrix_(),
                projectionParameters_(), horizontalAngle_(0.0f), verticalAngle_(0.0f), distance_(0.0f),
                strafeDirection_(), target_(), frustum_(), effectsList_(renderer.getEffects()),
                effectsMap_(), invalidEffect_(nullptr, 0, Effect::ParametersList()),
                renderingData_(), gui_(gui)
        {
                for(auto& effect: effectsList_)
                        effectsMap_.insert(std::make_pair(effect.getName(),
                                                          std::ref(effect)));

                setPosition(position);
                setDirection(direction);
                setUpVector(upVector);

                setPerspective(projectionParameters);
                setDistance(distance);

                renderingData_.setCamera(*this);
        }
        Camera::~Camera() {}

        //-------------------------------------------------------------------------------------------------------------
        void Camera::setGui(Gui* gui)
        {
                gui_ = gui;
        }

        //-------------------------------------------------------------------------------------------------------------
        Gui* Camera::getGui() const
        {
                return gui_;
        }

        //-------------------------------------------------------------------------------------------------------------
        Effect& Camera::getEffect(const char* name)
        {
                auto effect = effectsMap_.find(name);
                if(effect == effectsMap_.end())
                        return invalidEffect_;

                return effect->second.get();
        }

        //-------------------------------------------------------------------------------------------------------------
        const Effect& Camera::getEffect(const char* name) const
        {
                auto effect = effectsMap_.find(name);
                if(effect == effectsMap_.end())
                        return invalidEffect_;

                return effect->second.get();
        }

        //-------------------------------------------------------------------------------------------------------------
        const Renderer::EffectsList& Camera::getEffects() const
        {
                return effectsList_;
        }

        //-------------------------------------------------------------------------------------------------------------
        Renderer::Data& Camera::getRenderingData()
        {
                return renderingData_;
        }

        //-------------------------------------------------------------------------------------------------------------
        const Renderer::Data& Camera::getRenderingData() const
        {
                return renderingData_;
        }

        //-------------------------------------------------------------------------------------------------------------
        void Camera::setPerspective(const Vector4d& projectionParameters)
        {
                projectionParameters_ = projectionParameters;
                projectionMatrix_.perspective(projectionParameters_.x,
                                              projectionParameters_.y,
                                              projectionParameters_.z,
                                              projectionParameters_.w);
                projectionInvMatrix_ = projectionMatrix_;
                projectionInvMatrix_.invert();

                requestUpdateOperation();
        }

        //-------------------------------------------------------------------------------------------------------------
        const Vector4d& Camera::getProjectionParameters() const
        {
                return projectionParameters_;
        }

        //-------------------------------------------------------------------------------------------------------------
        void Camera::setUpVector(const Vector3d& upVector)
        {
                upVectors_[ORIGINAL] = upVector;

                if(upVectors_[ORIGINAL].length() > SELENE_EPSILON)
                        upVectors_[ORIGINAL].normalize();

                requestUpdateOperation();
        }

        //-------------------------------------------------------------------------------------------------------------
        const Vector3d& Camera::getUpVector() const
        {
                performUpdateOperation();
                return upVectors_[MODIFIED];
        }

        //-------------------------------------------------------------------------------------------------------------
        void Camera::setDirection(const Vector3d& direction)
        {
                directions_[ORIGINAL] = direction;

                if(directions_[ORIGINAL].length() > SELENE_EPSILON)
                        directions_[ORIGINAL].normalize();

                requestUpdateOperation();
        }

        //-------------------------------------------------------------------------------------------------------------
        const Vector3d& Camera::getDirection() const
        {
                performUpdateOperation();
                return directions_[MODIFIED];
        }

        //-------------------------------------------------------------------------------------------------------------
        void Camera::setDistance(float distance)
        {
                distance_ = distance < SELENE_EPSILON ? 0.0f : distance;
                requestUpdateOperation();
        }

        //-------------------------------------------------------------------------------------------------------------
        float Camera::getDistance() const
        {
                return distance_;
        }

        //-------------------------------------------------------------------------------------------------------------
        void Camera::rotateHorizontally(float angle)
        {
                horizontalAngle_ += angle;
                requestUpdateOperation();
        }

        //-------------------------------------------------------------------------------------------------------------
        void Camera::rotateVertically(float angle)
        {
                verticalAngle_ += angle;
                requestUpdateOperation();
        }

        //-------------------------------------------------------------------------------------------------------------
        void Camera::moveForward(float amount)
        {
                performUpdateOperation();
                positions_[ORIGINAL] += amount * directions_[MODIFIED];
                requestUpdateOperation();
        }

        //-------------------------------------------------------------------------------------------------------------
        void Camera::moveUpwards(float amount)
        {
                performUpdateOperation();
                positions_[ORIGINAL] += amount * upVectors_[MODIFIED];
                requestUpdateOperation();
        }

        //-------------------------------------------------------------------------------------------------------------
        void Camera::strafe(float amount)
        {
                performUpdateOperation();
                positions_[ORIGINAL] += amount * strafeDirection_;
                requestUpdateOperation();
        }

        //-------------------------------------------------------------------------------------------------------------
        const Matrix& Camera::getProjectionMatrix() const
        {
                return projectionMatrix_;
        }

        //-------------------------------------------------------------------------------------------------------------
        const Matrix& Camera::getProjectionInvMatrix() const
        {
                return projectionInvMatrix_;
        }

        //-------------------------------------------------------------------------------------------------------------
        const Matrix& Camera::getViewProjectionMatrix() const
        {
                performUpdateOperation();
                return viewProjectionMatrix_;
        }

        //-------------------------------------------------------------------------------------------------------------
        const Matrix& Camera::getViewMatrix() const
        {
                performUpdateOperation();
                return viewMatrix_;
        }

        //-------------------------------------------------------------------------------------------------------------
        const Vector3d& Camera::getStrafeDirection() const
        {
                performUpdateOperation();
                return strafeDirection_;
        }

        //-------------------------------------------------------------------------------------------------------------
        const Volume& Camera::getFrustum() const
        {
                performUpdateOperation();
                return frustum_;
        }

        //-------------------------------------------------------------------------------------------------------------
        int16_t Camera::getRenderingUnit() const
        {
                return -1;
        }

        //-------------------------------------------------------------------------------------------------------------
        RELATION Camera::determineRelation(const Volume& volume) const
        {
                volume.getNumPlanes();
                return OUTSIDE;
        }

        //-------------------------------------------------------------------------------------------------------------
        void Camera::update() const
        {
                // compute vectors
                computeVectors();

                // construct view matrix
                viewMatrix_.lookAt(positions_[MODIFIED], target_, upVectors_[MODIFIED]);

                // compute view-projection matrix
                viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;

                // define frustum
                frustum_.define(viewProjectionMatrix_);
        }

        //-------------------------------------------------------------------------------------------------------------
        void Camera::computeVectors() const
        {
                // construct horizontal rotation quaternion
                float halfAngle = horizontalAngle_ * 0.5f;
                Quaternion horizontalRotationQuaternion(upVectors_[ORIGINAL] * sin(halfAngle),
                                                        cos(halfAngle));

                // rotate direction
                Quaternion direction = Quaternion(directions_[ORIGINAL]) * horizontalRotationQuaternion;

                // compute strafe direction
                strafeDirection_ = Vector3d(direction.x, direction.y, direction.z).cross(upVectors_[ORIGINAL]);
                strafeDirection_.normalize();

                // construct vertical rotation quaternion
                halfAngle = verticalAngle_ * 0.5f;
                Quaternion verticalRotationQuaternion(strafeDirection_ * sin(halfAngle),
                                                      cos(halfAngle));

                // rotate up vector
                Quaternion upVector = Quaternion(upVectors_[ORIGINAL]) * verticalRotationQuaternion;

                // rotate direction once again
                direction = direction * verticalRotationQuaternion;

                // define modified direction and up vector
                directions_[MODIFIED].define(direction.x, direction.y, direction.z);
                upVectors_[MODIFIED].define(upVector.x, upVector.y, upVector.z);

                directions_[MODIFIED].normalize();
                upVectors_[MODIFIED].normalize();

                // compute modified position and target
                if(distance_ >= SELENE_EPSILON)
                {
                        target_ = positions_[MODIFIED];
                        positions_[MODIFIED] -= distance_ * directions_[MODIFIED];
                }
                else
                        target_ = positions_[MODIFIED] + directions_[MODIFIED];
        }

}
