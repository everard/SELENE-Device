// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "MeshAnimation.h"

namespace selene
{

        MeshAnimation::Data::Data()
        {
                length = lengthInv = 1.0f;
        }
        MeshAnimation::Data::~Data() {}

        MeshAnimation::MeshAnimation(const char* name): Resource(name) {}
        MeshAnimation::~MeshAnimation() {}

        //-----------------------------------------------------------------------
        bool MeshAnimation::prepare()
        {
                return true;
        }

        //-----------------------------------------------------------------------
        MeshAnimation::Data& MeshAnimation::getData()
        {
                return data_;
        }

        //-----------------------------------------------------------------------
        const MeshAnimation::Key& MeshAnimation::getInterpolatedKey(float scalar)
        {
                if(getNumKeys() == 0)
                        return data_.emptyKey;

                if(getNumKeys() == 1)
                        return data_.keys[0];

                if(scalar < 0.0f)
                        scalar = 0.0f;

                float intPart;
                if(scalar >= 1.0f)
                        scalar = std::modf(scalar, &intPart);

                uint32_t frame0 = static_cast<uint32_t>(data_.length * scalar);
                if(frame0 >= getNumKeys())
                        frame0 = 0;

                uint32_t frame1 = frame0 + 1;
                if(frame1 >= getNumKeys())
                        frame1 = 0;

                float start = static_cast<float>(frame0) * data_.lengthInv;
                scalar = (scalar - start) * data_.length;

                const Key& key0 = getKey(frame0);
                const Key& key1 = getKey(frame1);
                Key& key = data_.helperKey;

                if(key0.getSize() == key1.getSize() && key.getSize() == key0.getSize())
                {
                        for(uint32_t i = 0; i < key.getSize(); ++i)
                        {
                                key[i].transform.rotation =
                                        key0[i].transform.rotation.lerp(key1[i].transform.rotation, scalar);
                                key[i].transform.position =
                                        key0[i].transform.position.lerp(key1[i].transform.position, scalar);
                        }
                }

                return data_.helperKey;
        }

        //-----------------------------------------------------------------------
        const MeshAnimation::Key& MeshAnimation::getKey(uint32_t index)
        {
                if(index >= getNumKeys())
                        return data_.emptyKey;

                return data_.keys[index];
        }

        //-----------------------------------------------------------------------
        uint32_t MeshAnimation::getNumKeys()
        {
                return data_.keys.getSize();
        }

}
