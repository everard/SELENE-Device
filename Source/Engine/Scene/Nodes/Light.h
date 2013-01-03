// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef LIGHT_H
#define LIGHT_H

#include "../../Core/Math/Matrix.h"
#include "../../Core/Math/Cone.h"
#include "../Scene.h"

namespace selene
{

        /**
         * \addtogroup Scene
         * @{
         */

        /**
         * Represents light. This is base class for all lights.
         */
        class Light: public Scene::Node
        {
        public:
                /**
                 * \brief Constructs light with given name.
                 * \param[in] name name of the light
                 */
                Light(const char* name);
                virtual ~Light();

                /**
                 * \brief Returns volume.
                 * \return volume of the light
                 */
                const Volume& getVolume() const;

                /**
                 * \brief Sets color.
                 * \param[in] color color of the light
                 */
                void setColor(const Vector3d& color);

                /**
                 * \brief Returns color.
                 * \return color of the light
                 */
                const Vector3d& getColor() const;

                /**
                 * \brief Sets intensity.
                 * \param[in] intensity intensity of the light
                 */
                void setIntensity(float intensity);

                /**
                 * \brief Returns intensity.
                 * \return intensity of the light
                 */
                float getIntensity() const;

        protected:
                mutable Volume volume_;
                Vector3d color_;
                float intensity_;

        };

        /**
         * Represents directional light.
         */
        class DirectionalLight: public Light
        {
        public:
                /**
                 * \brief Constructs directional light with given name, direction,
                 * color, intensity and size.
                 * \param[in] name name of the light
                 * \param[in] direction direction of the light
                 * \param[in] color color of the light
                 * \param[in] intensity intensity of the light
                 * \param[in] size size of the light
                 */
                DirectionalLight(const char* name,
                                 const Vector3d& direction = Vector3d(),
                                 const Vector3d& color = Vector3d(1.0f, 1.0f, 1.0f),
                                 float intensity = 1.0f,
                                 float size = 1000.0f);
                ~DirectionalLight();

                /**
                 * \brief Sets direction.
                 * \param[in] direction direction of the light
                 */
                void setDirection(const Vector3d& direction);

                /**
                 * \brief Returns direction.
                 * \return direction of the light
                 */
                const Vector3d& getDirection() const;

                /**
                 * \brief Sets size.
                 *
                 * Size of the light defines maximum distance from the camera's position
                 * at which scene nodes are illuminated by the current light.
                 * \param[in] size size of the light
                 */
                void setSize(float size);

                /**
                 * \brief Returns size.
                 * \see setSize
                 * \return size of the light
                 */
                float getSize() const;

                /**
                 * \brief Returns rendering unit.
                 * \return Renderer::Data::UNIT_LIGHT_DIRECTIONAL if light casts shadows,
                 * Renderer::Data::UNIT_LIGHT_NO_SHADOWS_DIRECTIONAL if it doesn't
                 */
                int16_t getRenderingUnit() const;

                /**
                 * \brief Determines relation between directional light and volume.
                 * \param[in] volume volume
                 * \return OUTSIDE if light is hidden, INSIDE otherwise
                 */
                RELATION determineRelation(const Volume& volume) const;

        protected:
                /**
                 * \brief Updates directional light.
                 */
                void update() const;

        private:
                Vector3d direction_;
                float size_;

        };

        /**
         * Represents point light.
         */
        class PointLight: public Light
        {
        public:
                /**
                 * \brief Constructs point light with given name, position, color, intensity
                 * and radius.
                 * \param[in] name name of the light
                 * \param[in] position position of the light
                 * \param[in] color color of the light
                 * \param[in] intensity intensity of the light
                 * \param[in] radius radius of the light
                 */
                PointLight(const char* name,
                           const Vector3d& position = Vector3d(),
                           const Vector3d& color = Vector3d(1.0f, 1.0f, 1.0f),
                           float intensity = 1.0f,
                           float radius = 1.0f);
                ~PointLight();

                /**
                 * \brief Sets radius.
                 * \param[in] radius radius of the light
                 */
                void setRadius(float radius);

                /**
                 * \brief Returns radius.
                 * \return radius of the light
                 */
                float getRadius() const;

                /**
                 * \brief Returns rendering unit.
                 * \return Renderer::Data::UNIT_LIGHT_POINT if light casts shadows,
                 * Renderer::Data::UNIT_LIGHT_NO_SHADOWS_POINT if it doesn't
                 */
                int16_t getRenderingUnit() const;

                /**
                 * \brief Determines relation between point light and volume.
                 * \param[in] volume volume
                 * \return INSIDE if bounding sphere of light is inside the volume,
                 * INTERSECTS if bounding sphere of light intersects the volume,
                 * OUTSIDE if bounding sphere of light is outside the volume
                 */
                RELATION determineRelation(const Volume& volume) const;

        protected:
                /**
                 * \brief Updates point light.
                 */
                void update() const;

        private:
                mutable Sphere boundingSphere_;

        };

        /**
         * Represents spot light.
         */
        class SpotLight: public Light
        {
        public:
                /**
                 * \brief Constructs spot light with given name, position, direction, color,
                 * intensity and radius of the base of the light cone.
                 * \param[in] name name of the light
                 * \param[in] position original position of the light
                 * \param[in] direction original direction of the light
                 * \param[in] color color of the light
                 * \param[in] intensity intensity of the light
                 * \param[in] radius radius of the base of the light cone
                 */
                SpotLight(const char* name,
                          const Vector3d& position  = Vector3d(),
                          const Vector3d& direction = Vector3d(1.0f),
                          const Vector3d& color = Vector3d(1.0f, 1.0f, 1.0f),
                          float intensity = 1.0f,
                          float radius = 1.0f);
                ~SpotLight();

                /**
                 * \brief Sets direction.
                 * \param[in] direction original direction of the light
                 */
                void setDirection(const Vector3d& direction);

                /**
                 * \brief Returns direction.
                 * \return modified direction of the light
                 */
                const Vector3d& getDirection() const;

                /**
                 * \brief Sets radius.
                 * \param[in] radius radius of the base of the light cone
                 */
                void setRadius(float radius);

                /**
                 * \brief Returns radius.
                 * \return radius of the base of the light cone
                 */
                float getRadius() const;

                /**
                 * \brief Returns projection parameters.
                 * \return projection parameters (where x is half of field of view,
                 * y - aspect ratio, z - distance to the near clip plane, w - distance to the far clip plane)
                 */
                const Vector4d& getProjectionParameters() const;

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
                 * \brief Returns cosine of theta.
                 * \return cosine of theta (theta is angle between axis of the
                 * bounding cone and its line segment between apex and base)
                 */
                float getCosTheta() const;

                /**
                 * \brief Returns rendering unit.
                 * \return Renderer::Data::UNIT_LIGHT_SPOT if light casts shadows,
                 * Renderer::Data::UNIT_LIGHT_NO_SHADOWS_SPOT if it doesn't
                 */
                int16_t getRenderingUnit() const;

                /**
                 * \brief Determines relation between spot light and volume.
                 * \param[in] volume volume
                 * \return INSIDE if bounding cone of light is inside the volume,
                 * INTERSECTS if bounding cone of light intersects the volume,
                 * OUTSIDE if bounding cone of light is outside the volume
                 */
                RELATION determineRelation(const Volume& volume) const;

        protected:
                /**
                 * \brief Updates spot light.
                 */
                void update() const;

        private:
                mutable Vector3d directions_[NUM_OF_INDICES];
                mutable Cone boundingCone_;

                mutable Matrix viewProjectionMatrix_, viewMatrix_;
                mutable Vector4d projectionParameters_;
                mutable float cosTheta_;

        };

        /**
         * @}
         */

}

#endif
