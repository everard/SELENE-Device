// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Skeleton.h"

namespace selene
{

        //-----------------------------------------------------------------------------------------------
        void Skeleton::Transform::identity()
        {
                position.define(0.0f);
                rotation.define(0.0f, 0.0f, 0.0f, 1.0f);
        }

        //-----------------------------------------------------------------------------------------------
        Skeleton::Transform Skeleton::Transform::operator +(const Skeleton::Transform& transform)
        {
                Transform combinedTransform;
                combinedTransform.position = position + rotation.rotate(transform.position);
                combinedTransform.rotation = rotation * transform.rotation;
                return combinedTransform;
        }

        //-----------------------------------------------------------------------------------------------
        Skeleton::Transform operator -(const Skeleton::Transform& transform)
        {
                Skeleton::Transform invertedTransform;
                invertedTransform.rotation = transform.rotation.conjugate();
                invertedTransform.position = -invertedTransform.rotation.rotate(transform.position);
                return invertedTransform;
        }

        //-----------------------------------------------------------------------------------------------
        Skeleton::Transform Skeleton::Transform::operator -(const Skeleton::Transform& transform)
        {
                Transform initialTransform;
                Quaternion conjugatedRotation = transform.rotation.conjugate();
                initialTransform.rotation = conjugatedRotation * rotation;
                initialTransform.position = conjugatedRotation.rotate(position - transform.position);
                return initialTransform;
        }

        Skeleton::Instance::Instance()
        {
                isUpdated_ = false;
        }
        Skeleton::Instance::~Instance() {}

        //-----------------------------------------------------------------------------------------------
        bool Skeleton::Instance::initialize(std::shared_ptr<Skeleton> skeleton)
        {
                destroy();
                skeleton_ = skeleton;

                if(!skeleton)
                        return false;

                uint16_t numBones = skeleton->bones_.getSize();

                if(!combinedBoneTransforms_.create(numBones) ||
                   !localBoneTransforms_.create(numBones) ||
                   !finalBoneTransforms_.create(numBones))
                {
                        destroy();
                        return false;
                }

                localBoneTransforms_ = skeleton->initialLocalBoneTransforms_;
                return true;
        }

        //-----------------------------------------------------------------------------------------------
        void Skeleton::Instance::destroy()
        {
                localBoneTransforms_.destroy();
                combinedBoneTransforms_.destroy();
                finalBoneTransforms_.destroy();
                skeleton_.reset();
                isUpdated_ = false;
        }

        //-----------------------------------------------------------------------------------------------
        const Array<Skeleton::Transform, uint16_t>& Skeleton::Instance::getFinalBoneTransforms() const
        {
                if(!isUpdated_)
                {
                        computeFinalBoneTransforms();
                        isUpdated_ = true;
                }

                return finalBoneTransforms_;
        }

        //-----------------------------------------------------------------------------------------------
        const Array<Skeleton::Transform, uint16_t>& Skeleton::Instance::getCombinedBoneTransforms() const
        {
                if(!isUpdated_)
                {
                        computeFinalBoneTransforms();
                        isUpdated_ = true;
                }

                return combinedBoneTransforms_;
        }

        //-----------------------------------------------------------------------------------------------
        void Skeleton::Instance::setInitialPose()
        {
                if(skeleton_.expired())
                        return;

                auto skeleton = skeleton_.lock();
                const auto& initialLocalBoneTransforms = skeleton->initialLocalBoneTransforms_;

                if(initialLocalBoneTransforms.getSize() != localBoneTransforms_.getSize())
                        return;

                localBoneTransforms_ = initialLocalBoneTransforms;
                isUpdated_ = false;
        }

        //-----------------------------------------------------------------------------------------------
        void Skeleton::Instance::blendPose(const Array<BoneTransform, uint16_t>& boneTransforms,
                                           float blendFactor)
        {
                if(skeleton_.expired())
                        return;

                if(boneTransforms.isEmpty())
                        return;

                auto skeleton = skeleton_.lock();

                if(blendFactor >= 1.0f)
                {
                        for(uint32_t i = 0; i < boneTransforms.getSize(); ++i)
                        {
                                const BoneTransform& boneTransform = boneTransforms[i];
                                int32_t boneIndex = skeleton->getBoneIndex(boneTransform.boneName);
                                if(boneIndex < 0)
                                        continue;

                                localBoneTransforms_[boneIndex] = boneTransform.transform;
                        }
                }
                else
                {
                        for(uint32_t i = 0; i < boneTransforms.getSize(); ++i)
                        {
                                const BoneTransform& boneTransform = boneTransforms[i];
                                int32_t boneIndex = skeleton->getBoneIndex(boneTransform.boneName);
                                if(boneIndex < 0)
                                        continue;

                                Transform& localBoneTransform = localBoneTransforms_[boneIndex];

                                localBoneTransform.rotation =
                                        localBoneTransform.rotation.lerp(boneTransform.transform.rotation,
                                                                         blendFactor);
                                localBoneTransform.position =
                                        localBoneTransform.position.lerp(boneTransform.transform.position,
                                                                         blendFactor);
                        }
                }

                isUpdated_ = false;
        }

        //-----------------------------------------------------------------------------------------------
        int32_t Skeleton::Instance::getBoneIndex(const std::string& boneName) const
        {
                if(skeleton_.expired())
                        return -1;

                auto skeleton = skeleton_.lock();
                return skeleton->getBoneIndex(boneName);
        }

        //-----------------------------------------------------------------------------------------------
        void Skeleton::Instance::computeFinalBoneTransforms() const
        {
                if(skeleton_.expired())
                        return;

                auto skeleton = skeleton_.lock();
                const auto& bones = skeleton->bones_;

                if(bones.getSize() != localBoneTransforms_.getSize())
                        return;

                for(uint16_t i = 0; i < localBoneTransforms_.getSize(); ++i)
                {
                        int32_t parent = bones[i].parent;
                        if(parent >= 0 && parent < localBoneTransforms_.getSize())
                                combinedBoneTransforms_[i] = combinedBoneTransforms_[parent] + localBoneTransforms_[i];
                        else
                                combinedBoneTransforms_[i] = localBoneTransforms_[i];

                        finalBoneTransforms_[i] = combinedBoneTransforms_[i] + bones[i].offsetTransform;
                }
        }

        Skeleton::Skeleton() {}
        Skeleton::~Skeleton() {}

        //-----------------------------------------------------------------------------------------------
        bool Skeleton::initialize()
        {
                // allocate memory
                uint16_t numBones = bones_.getSize();

                Array<Transform, uint16_t> combinedBoneTransforms, localBoneTransforms;
                if(!initialLocalBoneTransforms_.create(numBones) ||
                   !combinedBoneTransforms.create(numBones) ||
                   !localBoneTransforms.create(numBones))
                {
                        destroy();
                        return false;
                }

                bonesMap_.clear();
                for(uint16_t i = 0; i < bones_.getSize(); ++i)
                {
                        bonesMap_.insert(std::pair<std::string, uint16_t>(bones_[i].name, i));
                        localBoneTransforms[i] = combinedBoneTransforms[i] = -bones_[i].offsetTransform;
                }

                for(int32_t i = (bones_.getSize() - 1); i >= 0; --i)
                {
                        int32_t parent = bones_[i].parent;

                        if(parent >= 0 && parent < bones_.getSize())
                                localBoneTransforms[i] = combinedBoneTransforms[i] - combinedBoneTransforms[parent];

                        initialLocalBoneTransforms_[i] = localBoneTransforms[i];
                }

                return true;
        }

        //-----------------------------------------------------------------------------------------------
        void Skeleton::destroy()
        {
                bonesMap_.clear();
                bones_.destroy();
                initialLocalBoneTransforms_.destroy();
        }

        //-----------------------------------------------------------------------------------------------
        Array<Skeleton::Bone, uint16_t>& Skeleton::getBones()
        {
                return bones_;
        }

        //-----------------------------------------------------------------------------------------------
        const Array<Skeleton::Bone, uint16_t>& Skeleton::getBones() const
        {
                return bones_;
        }

        //-----------------------------------------------------------------------------------------------
        int32_t Skeleton::getBoneIndex(const std::string& boneName) const
        {
                auto it = bonesMap_.find(boneName);
                if(it == bonesMap_.end())
                        return -1;

                if(it->second >= bones_.getSize())
                        return -1;

                return static_cast<int32_t>(it->second);
        }

        //-----------------------------------------------------------------------------------------------
        Skeleton& Skeleton::operator =(const Skeleton& skeleton)
        {
                bones_ = skeleton.bones_;
                initialize();
                return *this;
        }

}
