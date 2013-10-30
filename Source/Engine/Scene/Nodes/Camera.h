// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef CAMERA_H
#define CAMERA_H

#include "../../Rendering/Renderer.h"
#include "../../GUI/GUI.h"

#include "../../Core/Math/Matrix.h"
#include "../../Core/Math/Sphere.h"
#include "../Scene.h"

namespace selene
{

        /**
         * \addtogroup Scene
         * @{
         */

        /**
         * Represents scene camera. Contains rendering data, which can be passed to the renderer.
         * \see Renderer::Data
         */
        class Camera: public Scene::Node
        {
        public:
                /**
                 * \brief Constructs camera.
                 * \param[in] name name of the camera
                 * \param[in] renderer renderer which shall be used by the camera
                 * \param[in] position original position of the camera
                 * \param[in] direction original direction of the camera
                 * \param[in] upVector original up vector of the camera
                 * \param[in] projectionParameters projection parameters (where x is half of field of view,
                 * y - aspect ratio, z - distance to the near clip plane, w - distance to the far clip plane)
                 * \param[in] distance distance from target
                 * \param[in] gui GUI
                 */
                Camera(const char* name,
                       const Renderer& renderer,
                       const Vector3d& position  = Vector3d(),
                       const Vector3d& direction = Vector3d(1.0f),
                       const Vector3d& upVector  = Vector3d(0.0f, 1.0f),
                       const Vector4d& projectionParameters = Vector4d(45.0f, 0.75f, 1.0f, 1000.0f),
                       float distance = 0.0f,
                       Gui* gui = nullptr);
                Camera(const Camera&) = delete;
                virtual ~Camera();
                Camera& operator =(const Camera&) = delete;

                /**
                 * \brief Sets GUI.
                 * \param[in] gui GUI
                 */
                void setGui(Gui* gui);

                /**
                 * \brief Returns GUI, associated with current camera.
                 * \return pointer to the GUI
                 */
                Gui* getGui() const;

                /**
                 * \brief Returns effect.
                 * \param[in] name name of the effect
                 * \return reference to the effect
                 */
                Effect& getEffect(const char* name);

                /**
                 * \brief Returns effect.
                 * \param[in] name name of the effect
                 * \return const reference to the effect
                 */
                const Effect& getEffect(const char* name) const;

                /**
                 * \brief Returns list of the effects.
                 * \return list of the effects
                 */
                const Renderer::EffectsList& getEffects() const;

                /**
                 * \brief Returns rendering data.
                 * \return reference to the rendering data
                 */
                Renderer::Data& getRenderingData();

                /**
                 * \brief Returns rendering data.
                 * \return const reference to the rendering data
                 */
                const Renderer::Data& getRenderingData() const;

                /**
                 * \brief Sets perspective.
                 * \param[in] projectionParameters projection parameters (where x is half of field of view,
                 * y - aspect ratio, z - distance to the near clip plane, w - distance to the far clip plane)
                 */
                void setPerspective(const Vector4d& projectionParameters);

                /**
                 * \brief Returns projection parameters.
                 * \see setPerspective
                 * \return projection parameters
                 */
                const Vector4d& getProjectionParameters() const;

                /**
                 * \brief Sets up vector.
                 * \param[in] upVector original up vector
                 */
                void setUpVector(const Vector3d& upVector);

                /**
                 * \brief Returns up vector.
                 * \return modified up vector
                 */
                const Vector3d& getUpVector() const;

                /**
                 * \brief Sets direction.
                 * \param[in] direction original direction
                 */
                void setDirection(const Vector3d& direction);

                /**
                 * \brief Returns direction.
                 * \return modified direction
                 */
                const Vector3d& getDirection() const;

                /**
                 * \brief Sets distance from target.
                 *
                 * If set to zero, then camera becomes first person camera. Otherwise it becomes third person camera,
                 * whose target resides in its local position (set by Scene::Node::setPosition method) and its actual
                 * position resides in point, determined by the following formula.
                 *
                 * [real position of the camera] = [local position] - [camera direction] * [distance from target]
                 * \param[in] distance distance from camera target
                 */
                void setDistance(float distance);

                /**
                 * \brief Returns distance from target.
                 * \see setDistance
                 * \return distance from target
                 */
                float getDistance() const;

                /**
                 * \brief Rotates horizontally.
                 * \param[in] angle rotation angle in radians
                 */
                void rotateHorizontally(float angle);

                /**
                 * \brief Rotates vertically.
                 * \param[in] angle rotation angle in radians
                 */
                void rotateVertically(float angle);

                /**
                 * \brief Moves forward.
                 * \param[in] amount amount of movement
                 */
                void moveForward(float amount);

                /**
                 * \brief Moves upwards.
                 * \param[in] amount amount of movement
                 */
                void moveUpwards(float amount);

                /**
                 * \brief Strafes.
                 * \param[in] amount amount of movement
                 */
                void strafe(float amount);

                /**
                 * \brief Returns projection matrix.
                 * \return projection matrix
                 */
                const Matrix& getProjectionMatrix() const;

                /**
                 * \brief Returns inverse of the projection matrix.
                 * \return inverse of the projection matrix
                 */
                const Matrix& getProjectionInvMatrix() const;

                /**
                 * \brief Returns view-projection matrix.
                 * \return view-projection matrix
                 */
                const Matrix& getViewProjectionMatrix() const;

                /**
                 * \brief Returns view matrix.
                 * \return view matrix
                 */
                const Matrix& getViewMatrix() const;

                /**
                 * \brief Returns strafe direction.
                 * \return strafe direction
                 */
                const Vector3d& getStrafeDirection() const;

                /**
                 * \brief Returns frustum.
                 * \return frustum
                 */
                const Volume& getFrustum() const;

                /**
                 * \brief Returns rendering unit.
                 * \return -1
                 */
                int16_t getRenderingUnit() const;

                /**
                 * \brief Determines relation between camera and volume.
                 * \param[in] volume volume
                 * \return OUTSIDE
                 */
                RELATION determineRelation(const Volume& volume) const;

        protected:
                /**
                 * Represents map of the effects.
                 */
                typedef std::map<const char*,
                                 std::reference_wrapper<Effect>,
                                 Utility::StringComparator> EffectsMap;

                mutable Matrix projectionMatrix_, projectionInvMatrix_;
                mutable Matrix viewProjectionMatrix_, viewMatrix_;
                Vector4d projectionParameters_;

                mutable Vector3d directions_[NUM_OF_INDICES], upVectors_[NUM_OF_INDICES];
                float horizontalAngle_, verticalAngle_, distance_;
                mutable Vector3d strafeDirection_, target_;
                mutable Volume frustum_;

                Renderer::EffectsList effectsList_;
                EffectsMap effectsMap_;
                Effect invalidEffect_;

                Renderer::Data renderingData_;
                Gui* gui_;

                /**
                 * \brief Updates camera.
                 */
                void update() const;

                /**
                 * \brief Computes vectors.
                 */
                void computeVectors() const;

        };

        /**
         * @}
         */

}

#endif
