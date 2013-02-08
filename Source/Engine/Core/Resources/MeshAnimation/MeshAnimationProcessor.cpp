// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "MeshAnimationProcessor.h"
#include <utility>

namespace selene
{

        MeshAnimationProcessor::MixableMeshAnimation::MixableMeshAnimation(const Resource::Instance<MeshAnimation>& meshAnimation,
                                                                           float blendFactorTransitionTime,
                                                                           float startingTransitionTime,
                                                                           float stoppingTransitionTime,
                                                                           float animationTime,
                                                                           float blendFactor,
                                                                           Skeleton::Instance* skeletonInstance)
        {
                skeletonInstance_ = skeletonInstance;

                meshAnimation_ = meshAnimation;

                numTimesToPlay_ = 0;
                numTimesPlayed_ = 0;

                blendFactorTransitionTime_ = blendFactorTransitionTime > SELENE_EPSILON ? blendFactorTransitionTime : 0.0f;
                startingTransitionTime_ = startingTransitionTime > SELENE_EPSILON ? startingTransitionTime : 0.0f;
                stoppingTransitionTime_ = stoppingTransitionTime > SELENE_EPSILON ? stoppingTransitionTime : 0.0f;
                animationTime_ = animationTime > SELENE_EPSILON ? animationTime : 1.0f;
                elapsedTime_ = animationInterpolationScalar_ = 0.0f;

                blendFactor = blendFactor > SELENE_EPSILON ? blendFactor : 0.0f;
                blendFactorInterpolationScalar_ = 1.0f;
                blendFactor_.setValue(blendFactor, 0);
                blendFactor_.setValue(blendFactor, 1);

                state_ = STOPPED;
        }
        MeshAnimationProcessor::MixableMeshAnimation::~MixableMeshAnimation() {}

        //----------------------------------------------------------------------------------------------------
        void MeshAnimationProcessor::MixableMeshAnimation::play(float blendFactor, uint32_t numTimes)
        {
                setBlendFactor(blendFactor);
                play(numTimes);
        }

        //----------------------------------------------------------------------------------------------------
        void MeshAnimationProcessor::MixableMeshAnimation::play(uint32_t numTimes)
        {
                numTimesToPlay_ = numTimes;
                numTimesPlayed_ = 0;

                switch(state_)
                {
                        case STOPPING_ANIMATION:
                                if(startingTransitionTime_ < SELENE_EPSILON)
                                {
                                        elapsedTime_ = 0.0f;
                                        state_ = PLAYING;
                                }
                                else
                                {
                                        elapsedTime_ *= startingTransitionTime_ / stoppingTransitionTime_;
                                        state_ = INTERPOLATING;
                                }
                                break;

                        case INTERPOLATING:
                        case PLAYING:
                                break;

                        default:
                                elapsedTime_ = 0.0f;
                                animationInterpolationScalar_ = 0.0f;

                                if(startingTransitionTime_ < SELENE_EPSILON)
                                        state_ = PLAYING;
                                else
                                        state_ = INTERPOLATING;
                }
        }

        //----------------------------------------------------------------------------------------------------
        void MeshAnimationProcessor::MixableMeshAnimation::stop()
        {
                if(state_ == STOPPING_ANIMATION || state_ == STOPPED)
                        return;

                if(stoppingTransitionTime_ < SELENE_EPSILON)
                {
                        state_ = STOPPED;
                        return;
                }

                if(state_ == INTERPOLATING)
                        elapsedTime_ *= stoppingTransitionTime_ / startingTransitionTime_;
                else
                {
                        animationInterpolationScalar_ = elapsedTime_ / animationTime_;
                        elapsedTime_ = stoppingTransitionTime_;
                }

                state_ = STOPPING_ANIMATION;
        }

        //----------------------------------------------------------------------------------------------------
        void MeshAnimationProcessor::MixableMeshAnimation::setBlendFactor(float blendFactor)
        {
                blendFactor = blendFactor > SELENE_EPSILON ? blendFactor : 0.0f;

                blendFactor_.setValue(blendFactor_.getInterpolatedValue(blendFactorInterpolationScalar_), 0);
                blendFactor_.setValue(blendFactor, 1);

                blendFactorInterpolationScalar_ = 0.0f;
        }

        //----------------------------------------------------------------------------------------------------
        void MeshAnimationProcessor::MixableMeshAnimation::process(float elapsedTime)
        {
                if(state_ == STOPPED || skeletonInstance_ == nullptr)
                        return;

                MeshAnimation* meshAnimation = *meshAnimation_;
                if(meshAnimation == nullptr)
                        return;

                if(animationTime_ < SELENE_EPSILON)
                        return;

                // compute blend factor
                if(blendFactorInterpolationScalar_ < 1.0f)
                {
                        if(blendFactorTransitionTime_ >= SELENE_EPSILON)
                                blendFactorInterpolationScalar_ += elapsedTime / blendFactorTransitionTime_;
                        else
                                blendFactorInterpolationScalar_ = 1.0f;
                }

                float blendFactor = blendFactor_.getInterpolatedValue(blendFactorInterpolationScalar_);

                // stop animation
                if(state_ == STOPPING_ANIMATION)
                {
                        elapsedTime_ -= elapsedTime;

                        if(elapsedTime_ > 0.0f)
                        {
                                const MeshAnimation::Key& key =
                                        meshAnimation->getInterpolatedKey(animationInterpolationScalar_);
                                skeletonInstance_->blendPose(key,
                                                             blendFactor * elapsedTime_ / stoppingTransitionTime_);
                        }
                        else
                                state_ = STOPPED;

                        return;
                }

                // play animation
                elapsedTime_ += elapsedTime;

                if(state_ == INTERPOLATING)
                {
                        if(elapsedTime_ <= startingTransitionTime_)
                        {
                                const MeshAnimation::Key& key =
                                        meshAnimation->getInterpolatedKey(animationInterpolationScalar_);
                                skeletonInstance_->blendPose(key,
                                                             blendFactor * elapsedTime_ / startingTransitionTime_);
                        }
                        else
                        {
                                elapsedTime_ -= startingTransitionTime_;
                                elapsedTime_ += animationInterpolationScalar_ * animationTime_;
                                state_ = PLAYING;
                        }
                }

                if(state_ == PLAYING)
                {
                        if(elapsedTime_ > animationTime_)
                        {
                                float intPart;
                                std::modf(elapsedTime_ / animationTime_, &intPart);
                                elapsedTime_ -= intPart * animationTime_;

                                if(numTimesToPlay_ != 0)
                                        numTimesPlayed_ += static_cast<uint32_t>(intPart);
                        }

                        const MeshAnimation::Key& key =
                                meshAnimation->getInterpolatedKey(elapsedTime_ / animationTime_);
                        skeletonInstance_->blendPose(key, blendFactor);

                        if(numTimesToPlay_ != 0 && numTimesPlayed_ >= numTimesToPlay_)
                                stop();
                }
        }

        MeshAnimationProcessor::MeshAnimationProcessor() {}
        MeshAnimationProcessor::~MeshAnimationProcessor()
        {
                destroy();
        }

        //----------------------------------------------------------------------------------------------------
        bool MeshAnimationProcessor::initialize(const std::shared_ptr<Skeleton>& skeleton)
        {
                destroy();
                return skeletonInstance_.initialize(skeleton);
        }

        //----------------------------------------------------------------------------------------------------
        void MeshAnimationProcessor::destroy()
        {
                mixableMeshAnimations_.clear();
                skeletonInstance_.destroy();
        }

        //----------------------------------------------------------------------------------------------------
        const Skeleton::Instance& MeshAnimationProcessor::getSkeletonInstance() const
        {
                return skeletonInstance_;
        }

        //----------------------------------------------------------------------------------------------------
        bool MeshAnimationProcessor::addMeshAnimation(const Resource::Instance<MeshAnimation>& meshAnimation,
                                                      float blendFactorTransitionTime,
                                                      float startingTransitionTime,
                                                      float stoppingTransitionTime,
                                                      float animationTime,
                                                      float blendFactor)
        {
                if(*meshAnimation == nullptr)
                        return false;

                std::unique_ptr<MixableMeshAnimation> mixableMeshAnimation(new(std::nothrow) MixableMeshAnimation(meshAnimation,
                                                                                                                  blendFactorTransitionTime,
                                                                                                                  startingTransitionTime,
                                                                                                                  stoppingTransitionTime,
                                                                                                                  animationTime,
                                                                                                                  blendFactor,
                                                                                                                  &skeletonInstance_));

                if(!mixableMeshAnimation)
                        return false;

                try
                {
                        mixableMeshAnimations_.push_back(std::move(mixableMeshAnimation));
                }
                catch(...)
                {
                        return false;
                }

                return true;
        }

        //----------------------------------------------------------------------------------------------------
        bool MeshAnimationProcessor::removeMeshAnimation(uint32_t index)
        {
                if(index >= mixableMeshAnimations_.size())
                        return false;

                auto it = mixableMeshAnimations_.begin() + index;
                mixableMeshAnimations_.erase(it);
                return true;
        }

        //----------------------------------------------------------------------------------------------------
        MeshAnimationProcessor::MixableMeshAnimation& MeshAnimationProcessor::getMeshAnimation(uint32_t index)
        {
                if(index >= mixableMeshAnimations_.size())
                        return emptyMixableMeshAnimation_;

                return *mixableMeshAnimations_[index];
        }

        //----------------------------------------------------------------------------------------------------
        void MeshAnimationProcessor::processMeshAnimations(float elapsedTime)
        {
                for(auto it  = mixableMeshAnimations_.begin();
                         it != mixableMeshAnimations_.end();
                         ++it)
                        (*it)->process(elapsedTime);
        }

}
