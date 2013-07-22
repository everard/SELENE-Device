// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "ParticleSystem.h"

#include "../../Rendering/Renderer.h"
#include <limits>

namespace selene
{

        ParticleSystem::Particle::Particle(const Vector3d& position,
                                           const Vector3d& velocity,
                                           const Vector4d& color,
                                           float lifeSpan,
                                           float size):
                position_(position), velocity_(velocity), size_(size),
                fadeOutSpeed_(0.0f), color_(color)
        {
                if(lifeSpan > SELENE_EPSILON)
                        fadeOutSpeed_ = color_.w / lifeSpan;
        }
        ParticleSystem::Particle::~Particle() {}

        //-------------------------------------------------------------------------------------------------------------
        const Vector3d& ParticleSystem::Particle::getPosition() const
        {
                return position_;
        }

        //-------------------------------------------------------------------------------------------------------------
        const Vector4d& ParticleSystem::Particle::getColor() const
        {
                return color_;
        }

        //-------------------------------------------------------------------------------------------------------------
        float ParticleSystem::Particle::getSize() const
        {
                return size_;
        }

        //-------------------------------------------------------------------------------------------------------------
        bool ParticleSystem::Particle::isAlive() const
        {
                return (color_.w > 0.0f);
        }

        //-------------------------------------------------------------------------------------------------------------
        void ParticleSystem::Particle::process(float elapsedTime)
        {
                position_ += velocity_ * elapsedTime;
                color_.w -= fadeOutSpeed_ * elapsedTime;
                if(color_.w < 0.0f)
                        color_.w = 0.0f;
        }

        ParticleSystem::ParticleSystem(const char* name,
                                       const Vector3d& position,
                                       const Vector3d& direction,
                                       const Vector3d& scattering,
                                       const Vector2d& velocity,
                                       const Vector2d& lifeSpan,
                                       const Vector4d& color,
                                       const Vector2d& size,
                                       float density,
                                       const Resource::Instance<Texture>& texture):
                Scene::Node(name), particles_(150), previousPosition_(), boundingBox_(),
                elapsedTime_(0.0f), velocity_(velocity), lifeSpan_(lifeSpan),
                size_(size), scattering_(scattering), color_(color),
                density_(density), texture_(texture)
        {
                setPosition(position);
                setDirection(direction);
                previousPosition_ = getPosition();
        }
        ParticleSystem::~ParticleSystem() {}

        //-------------------------------------------------------------------------------------------------------------
        void ParticleSystem::clear()
        {
                previousPosition_ = getPosition();
                elapsedTime_ = 0.0f;

                particles_.clear();
                boundingBox_.define(getPosition(), 0.0f, 0.0f, 0.0f);
        }

        //-------------------------------------------------------------------------------------------------------------
        void ParticleSystem::setDirection(const Vector3d& direction)
        {
                directions_[ORIGINAL] = direction;

                if(directions_[ORIGINAL].length() > SELENE_EPSILON)
                        directions_[ORIGINAL].normalize();

                requestUpdateOperation();
        }

        //-------------------------------------------------------------------------------------------------------------
        const Vector3d& ParticleSystem::getDirection() const
        {
                performUpdateOperation();
                return directions_[MODIFIED];
        }

        //-------------------------------------------------------------------------------------------------------------
        void ParticleSystem::setTexture(const Resource::Instance<Texture>& texture)
        {
                texture_ = texture;
        }

        //-------------------------------------------------------------------------------------------------------------
        const Resource::Instance<Texture>& ParticleSystem::getTexture() const
        {
                return texture_;
        }

        //-------------------------------------------------------------------------------------------------------------
        const Bag<ParticleSystem::Particle>& ParticleSystem::getParticles() const
        {
                return particles_;
        }

        //-------------------------------------------------------------------------------------------------------------
        void ParticleSystem::process(float elapsedTime) const
        {
                if(is(Scene::Node::HIDDEN))
                        return;

                performUpdateOperation();

                elapsedTime_ += elapsedTime;

                // process particles
                uint32_t i = 0;
                Vector3d minBound, maxBound;
                minBound.define(std::numeric_limits<float>::max());
                maxBound.define(std::numeric_limits<float>::min());

                while(i < particles_.getSize())
                {
                        Particle& particle = particles_[i];
                        particle.process(elapsedTime);

                        if(!particle.isAlive())
                        {
                                if(!particles_.remove(i))
                                        ++i;
                        }
                        else
                        {
                                const Vector3d& particlePosition = particle.getPosition();

                                if(minBound.x > particlePosition.x)
                                        minBound.x = particlePosition.x;

                                if(minBound.y > particlePosition.y)
                                        minBound.y = particlePosition.y;

                                if(minBound.z > particlePosition.z)
                                        minBound.z = particlePosition.z;

                                if(maxBound.x < particlePosition.x)
                                        maxBound.x = particlePosition.x;

                                if(maxBound.y < particlePosition.y)
                                        maxBound.y = particlePosition.y;

                                if(maxBound.z < particlePosition.z)
                                        maxBound.z = particlePosition.z;

                                ++i;
                        }
                }

                // update bounding box
                Vector3d boxCenter = (maxBound + minBound) * 0.5f;
                Vector3d boxSize = maxBound - minBound;
                boundingBox_.define(boxCenter, boxSize.x, boxSize.y, boxSize.z);

                // create particles if needed
                if(density_ <= SELENE_EPSILON)
                        return;

                float numParticles = std::floor(elapsedTime_ * density_);
                if(numParticles >= 1.0f)
                {
                        // add particle at current position
                        addParticle(getPosition());

                        // get previous position and compute delta
                        Vector3d position = previousPosition_;
                        Vector3d delta = (getPosition() - position) / numParticles;

                        // create and process particles
                        float particleElapsedTime = numParticles / density_;
                        float deltaTime = particleElapsedTime / density_;
                        elapsedTime_ -= particleElapsedTime;

                        for(i = 1; i < static_cast<uint32_t>(numParticles); ++i)
                        {
                                if(addParticle(position))
                                {
                                        particles_[particles_.getSize() - 1].process(particleElapsedTime);

                                        particleElapsedTime -= deltaTime;
                                        position += delta;
                                }
                        }
                }

                previousPosition_ = getPosition();
        }

        //-------------------------------------------------------------------------------------------------------------
        void ParticleSystem::addParticle()
        {
                addParticle(getPosition());
        }

        //-------------------------------------------------------------------------------------------------------------
        int16_t ParticleSystem::getRenderingUnit() const
        {
                return Renderer::Data::UNIT_PARTICLE_SYSTEM;
        }

        //-------------------------------------------------------------------------------------------------------------
        RELATION ParticleSystem::determineRelation(const Volume& volume) const
        {
                if(is(Scene::Node::HIDDEN))
                        return OUTSIDE;

                return boundingBox_.determineRelation(volume);
        }

        //-------------------------------------------------------------------------------------------------------------
        void ParticleSystem::update() const
        {
                if(velocity_.x > SELENE_EPSILON)
                {
                        Quaternion direction = rotations_[MODIFIED] * Quaternion(directions_[ORIGINAL]);
                        directions_[MODIFIED] = Vector3d(direction.x, direction.y, direction.z);

                        if(directions_[MODIFIED].length() > SELENE_EPSILON)
                                directions_[MODIFIED].normalize();
                }
                else
                        directions_[MODIFIED] = directions_[ORIGINAL];
        }

        //-------------------------------------------------------------------------------------------------------------
        bool ParticleSystem::addParticle(const Vector3d& position) const
        {
                Vector3d velocity;

                if(velocity_.x > SELENE_EPSILON)
                {
                        if(velocity_.y > SELENE_EPSILON)
                        {
                                Vector3d randomVector(getRandomFloatValue(),
                                                      getRandomFloatValue(),
                                                      getRandomFloatValue());

                                velocity = directions_[MODIFIED] * (velocity_.x + velocity_.y * getRandomFloatValue());
                                velocity += scattering_ * getRandomFloatValue();
                        }
                        else
                                velocity = directions_[MODIFIED] * velocity_.x;
                }

                float size = size_.x;
                if(size_.y > SELENE_EPSILON)
                        size += size_.y * getRandomFloatValue();

                float lifeSpan = lifeSpan_.x;
                if(lifeSpan_.y > SELENE_EPSILON)
                        lifeSpan += lifeSpan_.y * getRandomFloatValue();

                return particles_.add(Particle(position, velocity, color_, lifeSpan, size));
        }

        //-------------------------------------------------------------------------------------------------------------
        float ParticleSystem::getRandomFloatValue() const
        {
                return static_cast<float>(128 - rand() % 255) * (1.0f / 128.0f);
        }

}
