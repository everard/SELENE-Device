// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "MeshAnimationManager.h"

namespace selene
{

        MeshAnimationManager::MeshAnimationManager() {}
        MeshAnimationManager::~MeshAnimationManager() {}

        //-----------------------------------------------------------------------------------------------------
        bool MeshAnimationManager::readMeshAnimation(std::istream& stream,
                                                     MeshAnimation::Data& meshAnimationData)
        {
                // read header
                char header[3];
                stream.read(header, sizeof(header));

                // validate header
                if(strncmp(header, "SLA", 3) != 0)
                        return false;

                Array<MeshAnimation::Key, uint32_t>& meshAnimationKeys =
                        meshAnimationData.keys;

                // read number of mesh animation keys
                uint32_t numMeshAnimationKeys = 0;
                stream.read(reinterpret_cast<char*>(&numMeshAnimationKeys), sizeof(uint32_t));

                // allocate memory for mesh animation keys
                if(!meshAnimationKeys.create(numMeshAnimationKeys))
                        return false;

                char boneName[MAX_STRING_LENGTH];

                try
                {
                        for(uint32_t i = 0; i < meshAnimationKeys.getSize(); ++i)
                        {
                                MeshAnimation::Key& meshAnimationKey = meshAnimationKeys[i];

                                // read bone transforms
                                uint16_t numBoneTransforms = 0;
                                stream.read(reinterpret_cast<char*>(&numBoneTransforms), sizeof(uint16_t));

                                if(!meshAnimationKey.create(numBoneTransforms))
                                        return false;

                                for(uint16_t j = 0; j < meshAnimationKey.getSize(); ++j)
                                {
                                        if(!readString(stream, boneName))
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
