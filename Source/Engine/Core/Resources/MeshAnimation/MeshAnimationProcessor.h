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
         * \addtogroup Resources
         * @{
         */

        /**
         * Represents mesh animation processor. It handles skeletal animation: starts/stops animations and
         * combines them in different ways.
         * The following example shows how to use mesh animation processor:
         * \code
         * // somewhere in program resource manager and mesh animation processor are declared
         * selene::ResourceManager resourceManager;
         * selene::MeshAnimationProcessor meshAnimationProcessor;
         *
         * ...
         *
         * // get instance of the mesh, whose skeleton shall be animated
         * selene::Resource::Instance<selene::Mesh> meshInstance = resourceManager.requestResource<selene::Mesh>("skin mesh");
         * if((*meshInstance) == nullptr)
         * {
         *         handleError();
         * }
         *
         * // get the skeleton of the mesh
         * std::shared_ptr<selene::Skeleton> skeleton = (*meshInstance).getData().skeleton;
         *
         * // initialize mesh animation processor (this action actually creates skeleton instance)
         * if(!meshAnimationProcessor.initialize(skeleton))
         * {
         *         handleError();
         * }
         *
         * ...
         *
         * // get instance of the animation
         * selene::Resource::Instance<MeshAnimation> animationInstance =
         *         resourceManager.requestResource<selene::MeshAnimation>("mesh animation");
         * // in real application we should also check animationInstance (as we checked meshInstance)
         *
         * // add mesh animation
         * meshAnimationProcessor.addMeshAnimation(animationInstance, 0.0f, 0.0f, 0.0f, 10.0f, 1.0f);
         * // now this animation has index zero in the mesh animation processor
         * // and it can be accessed through MeshAnimationProcessor::getMeshAnimation function
         *
         * // start mesh animation
         * meshAnimationProcessor.getMeshAnimation(0).play();
         *
         * ...
         * float elapsedTime;
         *
         * // animate skeleton
         * meshAnimationProcessor.processMeshAnimations(elapsedTime);
         * \endcode
         * Now meshAnimationProcessor has skeleton instance, which contains pose; latter can be accessed
         * through MeshAnimationProcessor::getSkeletonInstance method:
         * \code
         * const auto& finalBoneTransforms =
         *         meshAnimationProcessor.getSkeletonInstance().getFinalBoneTransforms();
         * \endcode
         *
         * Note, that this class should not be used directly, because mesh animation is handled inside
         * Actor class.
         *
         * This class uses MeshAnimationProcessor::MixableMeshAnimation for animation playing and mixing.
         * When MeshAnimationProcessor::getMeshAnimation is called, reference to the
         * MeshAnimationProcessor::MixableMeshAnimation is returned.
         * \see MeshAnimationProcessor::MixableMeshAnimation
         */
        class MeshAnimationProcessor
        {
        public:
                /**
                 * Represents mixable mesh animation. It contains instance of the actual MeshAnimation and
                 * additional data for animation processing:
                 * - state of the animation: starting, playing, stopping and stopped;
                 * - blend factor of current animation, which shows how much influence it has on the skeleton
                     (zero for none, one for maximum);
                 * - transition times between states (from starting to playing, from playing to stopped);
                 * - blend factor transition time (how fast blend factor will be changed).
                 *
                 * When animation is starting, its blend factor fades-in from zero to the actual value.
                 * When animation is stopping, its blend factor fades-out from the actual value to zero.
                 *
                 * There is a list of MixableMeshAnimation inside of the MeshAnimationProcessor. Each
                 * MixableMeshAnimation is processed, from the first to the last, and the result is applied
                 * to the skeleton. The influence, which MixableMeshAnimation has on the skeleton depends on
                 * the blend factor.
                 *
                 * For example, consider the following: we have two animations, which were previously added with
                 * MeshAnimationProcessor::addMeshAnimation. They both influence the same bone, say, "left_arm".
                 * First animation has it rotated by 90 degrees around Z axis, second - around Y axis.
                 * If they both have blendfactors less that one, then resulting pose is computed as following:
                 * 1. orientation["left_arm"] = orientation["left_arm"].blend(first_animation.orientation["left_arm"],
                 *    first_animation.blendFactor).
                 * 2. orientation["left_arm"] = orientation["left_arm"].blend(second_animation.orientation["left_arm"],
                 *    second_animation.blendFactor).
                 *
                 * If the second animation has blend factor equal to one, then the first animation's influence is
                 * completely discarded (rewrited by the second animation).
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

                        Resource::Instance<MeshAnimation> meshAnimation_;
                        Skeleton::Instance* skeletonInstance_;
                        uint32_t numTimesToPlay_;
                        uint32_t numTimesPlayed_;

                        float blendFactorTransitionTime_;
                        float startingTransitionTime_;
                        float stoppingTransitionTime_;
                        float animationTime_;
                        float elapsedTime_;

                        float animationInterpolationScalar_;
                        LinearInterpolator<float> blendFactor_;
                        float blendFactorInterpolationScalar_;

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

        /**
         * @}
         */

}

#endif
