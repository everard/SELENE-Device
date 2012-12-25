// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "../../Core/Resources/Texture/Texture.h"
#include "../../Core/Helpers/Bag.h"
#include "../../Core/Math/Box.h"
#include "../Scene.h"

namespace selene
{

        /**
         * Represents particle system.
         */
        class ParticleSystem: public Scene::Node
        {
        public:
                /**
                 * Represents particle.
                 */
                class Particle
                {
                public:
                        /**
                         * \brief Constructs particle with given position, velocity, color,
                         * life span and size.
                         * \param[in] position position of the particle
                         * \param[in] velocity velocity of the particle
                         * \param[in] color color of the particle
                         * \param[in] lifeSpan life span of the particle
                         * \param[in] size size of the particle
                         */
                        Particle(const Vector3d& position,
                                 const Vector3d& velocity,
                                 const Vector4d& color,
                                 float lifeSpan,
                                 float size);
                        ~Particle();

                        /**
                         * \brief Returns position.
                         * \return position of the particle
                         */
                        const Vector3d& getPosition() const;

                        /**
                         * \brief Returns color.
                         * \return color of the particle
                         */
                        const Vector4d& getColor() const;

                        /**
                         * \brief Returns size.
                         * \return size of the particle
                         */
                        float getSize() const;

                        /**
                         * \brief Returns true if particle is alive.
                         * \return true if particle is alive
                         */
                        bool isAlive() const;

                        /**
                         * \brief Processes particle.
                         * \param[in] elapsedTime elapsed time since last processing
                         */
                        void process(float elapsedTime);

                private:
                        // Position and velocity
                        Vector3d position_, velocity_;

                        // Color
                        Vector4d color_;

                        // Size and fade-out speed
                        float size_, fadeOutSpeed_;

                };

                /**
                 * \brief Constructs particle system with given name, position,
                 * direction and other parameters.
                 * \param[in] name name of the particle system
                 * \param[in] position position of the particle system
                 * \param[in] direction direction of the emission of particles
                 * \param[in] scattering scattering of the particles
                 * \param[in] velocity velocity of the particles (x contains base value,
                 * y - max variance)
                 * \param[in] lifeSpan life span of the particles (x contains base value,
                 * y - max variance)
                 * \param[in] color color of the particles
                 * \param[in] size size of the particles (x contains base value,
                 * y - max variance)
                 * \param[in] density density of the particles
                 * \param[in] texture texture of the particles
                 */
                ParticleSystem(const char* name,
                               const Vector3d& position = Vector3d(),
                               const Vector3d& direction = Vector3d(),
                               const Vector3d& scattering = Vector3d(),
                               const Vector2d& velocity = Vector2d(),
                               const Vector2d& lifeSpan = Vector2d(1.0f),
                               const Vector4d& color = Vector4d(1.0f, 1.0f, 1.0f, 1.0f),
                               const Vector2d& size = Vector2d(1.0f),
                               float density = 0.0f,
                               const Resource::Instance<Texture>& texture = Resource::Instance<Texture>());
                ~ParticleSystem();

                /**
                 * \brief Clears particle system.
                 */
                void clear();

                /**
                 * \brief Sets direction.
                 * \param[in] direction direction of the emission of particles
                 */
                void setDirection(const Vector3d& direction);

                /**
                 * \brief Returns direction.
                 * \return direction of the emission of particles
                 */
                const Vector3d& getDirection() const;

                /**
                 * \brief Sets texture.
                 * \param[in] texture texture of the particles
                 */
                void setTexture(const Resource::Instance<Texture>& texture);

                /**
                 * \brief Returns texture.
                 * \return const reference to the texture instance
                 */
                const Resource::Instance<Texture>& getTexture() const;

                /**
                 * \brief Returns particles.
                 * \return const reference to the bag which contains particles
                 */
                const Bag<Particle>& getParticles() const;

                /**
                 * \brief Processes particle system.
                 * \param[in] elapsedTime elapsed time since last processing
                 */
                void process(float elapsedTime) const;

                /**
                 * \brief Adds particle.
                 */
                void addParticle();

                /**
                 * \brief Returns rendering unit.
                 * \return UNIT_PARTICLE_SYSTEM
                 */
                int16_t getRenderingUnit() const;

                /**
                 * \brief Determines relation between particle system and volume.
                 * \param[in] volume volume
                 * \return INSIDE if bounding box of the particle system is inside the volume,
                 * INTERSECTS if bounding box of the particle system intersects the volume,
                 * OUTSIDE if bounding box of the particle system is outside the volume
                 */
                RELATION determineRelation(const Volume& volume) const;

        protected:
                /**
                 * \brief Updates particle system.
                 */
                void update() const;

        private:
                // Particles
                mutable Bag<Particle> particles_;

                // Directions
                mutable Vector3d directions_[NUM_OF_INDICES];

                // Previous position
                mutable Vector3d previousPosition_;

                // Bounding box
                mutable Box boundingBox_;

                // Elapsed time
                mutable float elapsedTime_;

                // Velocity, life span and size
                Vector2d velocity_, lifeSpan_, size_;

                // Scattering
                Vector3d scattering_;

                // Color
                Vector4d color_;

                // Density
                float density_;

                // Texture
                Resource::Instance<Texture> texture_;

                /**
                 * \brief Adds particle.
                 * \param[in] position position of the particle
                 * \return true if particle has been successfully added
                 */
                bool addParticle(const Vector3d& position) const;

                /**
                 * \brief Returns random float value.
                 * \return float value in [0; 1] range
                 */
                float getRandomFloatValue() const;

        };

}

#endif
