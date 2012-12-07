// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef MESH_ANIMATION_PROCESSOR_H
#define MESH_ANIMATION_PROCESSOR_H

#include "../../Math/LinearInterpolator.h"
#include "MeshAnimation.h"
#include <vector>

namespace selene
{

        /**
         * Represents mesh animation processor. It handles skeletal animation: starts/stops animations and
         * combines them in different ways.
         */
        class MeshAnimationProcessor
        {
        public:
                /**
                 * Represents mixable mesh animation.
                 */
                class MixableMeshAnimation
                {
                public:
                        /**
                         * \brief Constructs mixable mesh animation with given mesh animation instance, blend factor
                         * transition time, starting transition time, stopping transition time, animation time, blend
                         * factor and skeleton instance.
                         * \param[in] meshAnimation mesh animation instance
                         * \param[in] blendFactorTransitionTime specifies how much time it takes to change blend factor
                         * \param[in] startingTransitionTime specifies how much time it takes to start the animation
                         * \param[in] stoppingTransitionTime specifies how much time it takes to stop the animation
                         * \param[in] animationTime animation time
                         * \param[in] blendFactor blend factor (if set to zero, then current animation has no effect
                         * on skeleton)
                         * \param[in] skeletonInstance skeleton instance
                         */
                        MixableMeshAnimation(const Resource::Instance<MeshAnimation>& meshAnimation = Resource::Instance<MeshAnimation>(),
                                             float blendFactorTransitionTime = 0.0f,
                                             float startingTransitionTime    = 0.0f,
                                             float stoppingTransitionTime    = 0.0f,
                                             float animationTime = 0.0f,
                                             float blendFactor   = 0.0f,
                                             Skeleton::Instance* skeletonInstance = nullptr);
                        ~MixableMeshAnimation();

                        /**
                         * \brief Plays mesh animation.
                         * \param[in] blendFactor blend factor (if set to zero, then current animation has no effect
                         * on skeleton)
                         * \param[in] numTimes number of times to play animation
                         */
                        void play(float blendFactor, uint32_t numTimes = 0);

                        /**
                         * \brief Plays mesh animation.
                         * \param[in] numTimes number of times to play animation
                         */
                        void play(uint32_t numTimes = 0);

                        /**
                         * \brief Stops mesh animation.
                         */
                        void stop();

                        /**
                         * \brief Sets blend factor.
                         * \param[in] blendFactor blend factor (if set to zero, then current animation has no effect
                         * on skeleton)
                         */
                        void setBlendFactor(float blendFactor);

                private:
                        friend class MeshAnimationProcessor;

                        /// Mesh animation state
                        enum STATE
                        {
                                STOPPING_ANIMATION,
                                INTERPOLATING,
                                PLAYING,
                                STOPPED
                        };

                        // Mesh animation
                        Resource::Instance<MeshAnimation> meshAnimation_;

                        // Skeleton instance
                        Skeleton::Instance* skeletonInstance_;

                        // Number of times to play animation
                        uint32_t numTimesToPlay_;

                        // Number of times animation has been played
                        uint32_t numTimesPlayed_;

                        // Animation times
                        float blendFactorTransitionTime_;
                        float startingTransitionTime_;
                        float stoppingTransitionTime_;
                        float animationTime_;
                        float elapsedTime_;

                        // Animation interpolation scalar
                        float animationInterpolationScalar_;

                        // Blend factor
                        LinearInterpolator<float> blendFactor_;

                        // Blend factor interpolation scalar
                        float blendFactorInterpolationScalar_;

                        // Mesh animation state
                        STATE state_;

                        /**
                         * \brief Processes mesh animation.
                         * \param[in] elapsedTime elapsed time since last processing
                         */
                        void process(float elapsedTime);

                };

                MeshAnimationProcessor();
                ~MeshAnimationProcessor();

                /**
                 * \brief Initializes mesh animation processor.
                 * \param[in] skeleton const reference to the shared pointer to the skeleton
                 * \return true if mesh animation processor has been successfully initialized
                 */
                bool initialize(const std::shared_ptr<Skeleton>& skeleton);

                /**
                 * \brief Destroys mesh animation processor.
                 */
                void destroy();

                /**
                 * \brief Returns skeleton instance.
                 * \return const reference to the skeleton instance
                 */
                const Skeleton::Instance& getSkeletonInstance() const;

                /**
                 * \brief Adds mesh animation.
                 * \param[in] meshAnimation mesh animation instance
                 * \param[in] blendFactorTransitionTime specifies how much time it takes to change blend factor
                 * \param[in] startingTransitionTime specifies how much time it takes to start the animation
                 * \param[in] stoppingTransitionTime specifies how much time it takes to stop the animation
                 * \param[in] animationTime animation time
                 * \param[in] blendFactor blend factor (if set to zero, then current animation has no effect
                 * on skeleton)
                 * \return true if animation successfully added
                 */
                bool addMeshAnimation(const Resource::Instance<MeshAnimation>& meshAnimation,
                                      float blendFactorTransitionTime,
                                      float startingTransitionTime,
                                      float stoppingTransitionTime,
                                      float animationTime,
                                      float blendFactor);

                /**
                 * \brief Removes mesh animation.
                 * \param[in] index index of the animation which will be removed
                 * \return true if animation was successfully removed
                 */
                bool removeMeshAnimation(uint32_t index);

                /**
                 * \brief Returns mesh animation.
                 * \param[in] index index of the animation
                 * \return reference to the mixable mesh animation
                 */
                MixableMeshAnimation& getMeshAnimation(uint32_t index);

                /**
                 * \brief Processes mesh animations.
                 * \param[in] elapsedTime elapsed time since last processing
                 */
                void processMeshAnimations(float elapsedTime);

        private:
                std::vector<std::unique_ptr<MixableMeshAnimation>> mixableMeshAnimations_;
                MixableMeshAnimation emptyMixableMeshAnimation_;
                Skeleton::Instance skeletonInstance_;

        };

}

#endif
