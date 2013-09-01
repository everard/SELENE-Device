// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "MeshAnimationManager.h"
#include "../../Helpers/Utility.h"

namespace selene
{

        MeshAnimationManager::MeshAnimationManager() {}
        MeshAnimationManager::~MeshAnimationManager() {}

        //-----------------------------------------------------------------------------------------------------
        bool MeshAnimationManager::readMeshAnimation(std::istream& stream,
                                                     MeshAnimation::Data& meshAnimationData)
        {
                char header[4];
                stream.read(header, sizeof(header));

                if(std::memcmp(header, "SDAF", 4) != 0)
                        return false;

                Array<MeshAnimation::Key, uint32_t>& meshAnimationKeys = meshAnimationData.keys;

                uint32_t numMeshAnimationKeys = 0;
                stream.read(reinterpret_cast<char*>(&numMeshAnimationKeys), sizeof(uint32_t));

                if(!meshAnimationKeys.create(numMeshAnimationKeys))
                        return false;

                char boneName[Utility::MAX_STRING_LENGTH];

                try
                {
                        for(uint32_t i = 0; i < meshAnimationKeys.getSize(); ++i)
                        {
                                MeshAnimation::Key& meshAnimationKey = meshAnimationKeys[i];

                                uint16_t numBoneTransforms = 0;
                                stream.read(reinterpret_cast<char*>(&numBoneTransforms), sizeof(uint16_t));

                                if(!meshAnimationKey.create(numBoneTransforms))
                                        return false;

                                for(uint16_t j = 0; j < meshAnimationKey.getSize(); ++j)
                                {
                                        if(!Utility::readString(stream, boneName))
                                                return false;

                                        Skeleton::BoneTransform& boneTransform = meshAnimationKey[j];
                                        boneTransform.boneName = boneName;

                                        stream.read(reinterpret_cast<char*>(&boneTransform.transform.rotation),
                                                    sizeof(Quaternion));
                                        stream.read(reinterpret_cast<char*>(&boneTransform.transform.position),
                                                    sizeof(Vector3d));
                                }
                        }
                }
                catch(...)
                {
                        return false;
                }

                meshAnimationData.length = static_cast<float>(meshAnimationKeys.getSize());
                meshAnimationData.lengthInv = 1.0f / meshAnimationData.length;
                meshAnimationData.helperKey = meshAnimationKeys[0];

                return true;
        }

}
