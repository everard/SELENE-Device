// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef SKELETON_H
#define SKELETON_H

#include "../../Helpers/Array.h"
#include "../../Math/Vector.h"
#include <memory>
#include <string>
#include <map>

namespace selene
{

        /**
         * \addtogroup Resources
         * \brief Resources (textures, meshes, animations, etc.) and resource management.
         * @{
         */

        /**
         * Represents skeleton. Skeleton is used in mesh animation, it contains bones hierarchy.
         * Skeleton can have one or more instances. Each instance can be in its own pose - bones
         * can have different transformations for each instance.
         */
        class Skeleton
        {
        public:
                /**
                 * Represents transform.
                 */
                class Transform
                {
                public:
                        Quaternion rotation;
                        Vector3d position;

                        /**
                         * \brief Creates identity transform.
                         */
                        void identity();

                        /**
                         * \brief Computes combined transform.
                         * \param[in] transform another transform which will be combined with current
                         * \return combined transform
                         */
                        Transform operator +(const Transform& transform);

                        /**
                         * \brief Computes inverted transform.
                         * \param[in] transform initial transform
                         * \return inverted transform
                         */
                        friend Transform operator -(const Transform& transform);

                        /**
                         * \brief Computes initial transform.
                         * \param[in] transform another transform which will be subtracted from current
                         * \return initial transform
                         */
                        Transform operator -(const Transform& transform);

                };

                /**
                 * Represents bone.
                 */
                class Bone
                {
                public:
                        Transform offsetTransform;
                        std::string name;
                        int32_t parent;

                };

                /**
                 * Represents bone transform.
                 */
                class BoneTransform
                {
                public:
                        Transform transform;
                        std::string boneName;

                };

                /**
                 * Represents skeleton instance. This instance can be animated. It also holds reference to the
                 * original skeleton.
                 */
                class Instance
                {
                public:
                        Instance();
                        ~Instance();

                        /**
                         * \brief Initializes skeleton instance with skeleton.
                         * \param[in] skeleton shared pointer to the skeleton
                         * \return true if instance has been successfully initialized
                         */
                        bool initialize(std::shared_ptr<Skeleton> skeleton);

                        /**
                         * \brief Destroys skeleton instance.
                         */
                        void destroy();

                        /**
                         * \brief Returns final bone transforms.
                         *
                         * Final bone transforms are combined bone transforms with bind pose transformation applied.
                         * \return const reference to the array of final bone transforms (which can be used
                         * in rendering)
                         */
                        const Array<Transform, uint16_t>& getFinalBoneTransforms() const;

                        /**
                         * \brief Returns combined bone transforms.
                         * \return const reference to the array of combined bone transforms (this bone transforms
                         * do not have bind pose transformation applied)
                         */
                        const Array<Transform, uint16_t>& getCombinedBoneTransforms() const;

                        /**
                         * \brief Sets initial skeleton pose.
                         */
                        void setInitialPose();

                        /**
                         * \brief Blends skeleton pose.
                         * \param[in] boneTransforms local bone transforms which will be blended with current
                         * \param[in] blendFactor blend factor (float in [0; 1] range)
                         */
                        void blendPose(const Array<BoneTransform, uint16_t>& boneTransforms,
                                       float blendFactor);

                        /**
                         * \brief Returns bone index.
                         * \param[in] boneName name of the bone
                         * \return index of the bone with given name (or -1 if bone could not be found)
                         */
                        int32_t getBoneIndex(const std::string& boneName) const;

                private:
                        Array<Transform, uint16_t> localBoneTransforms_;
                        mutable Array<Transform, uint16_t> combinedBoneTransforms_;
                        mutable Array<Transform, uint16_t> finalBoneTransforms_;
                        std::weak_ptr<Skeleton> skeleton_;
                        mutable bool isUpdated_;

                        /**
                         * \brief Computes final bone transforms.
                         */
                        void computeFinalBoneTransforms() const;

                };

                Skeleton();
                ~Skeleton();

                /**
                 * \brief Initializes skeleton.
                 * \return true if skeleton successfully initialized
                 */
                bool initialize();

                /**
                 * \brief Destroys skeleton.
                 */
                void destroy();

                /**
                 * \brief Returns bones.
                 * \return reference to the array of bones
                 */
                Array<Bone, uint16_t>& getBones();

                /**
                 * \brief Returns bones.
                 * \return const reference to the array of bones
                 */
                const Array<Bone, uint16_t>& getBones() const;

                /**
                 * \brief Returns bone index.
                 * \param[in] boneName name of the bone
                 * \return index of the bone with given name (or -1 if bone could not be found)
                 */
                int32_t getBoneIndex(const std::string& boneName) const;

                /**
                 * \brief Assignes skeleton.
                 * \param[in] skeleton another skeleton which will be assigned to current
                 * \return reference to the skeleton
                 */
                Skeleton& operator =(const Skeleton& skeleton);

        private:
                Array<Bone, uint16_t> bones_;
                Array<Transform, uint16_t> initialLocalBoneTransforms_;
                std::map<std::string, uint16_t> bonesMap_;

        };

        /**
         * @}
         */

}

#endif
