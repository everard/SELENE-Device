// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef MESH_ANIMATION_H
#define MESH_ANIMATION_H

#include "../Mesh/Skeleton.h"
#include "../Resource.h"

namespace selene
{

        /**
         * Represents mesh animation.
         */
        class MeshAnimation: public Resource
        {
        public:
                /**
                 * Represents mesh animation key.
                 */
                typedef Array<Skeleton::BoneTransform, uint16_t> Key;

                /**
                 * Represents mesh animation data container.
                 */
                class Data
                {
                public:
                        // Mesh animation keys
                        Array<Key, uint32_t> keys;

                        // Helper key (used in interpolation)
                        Key helperKey;

                        // Empty key
                        Key emptyKey;

                        // Animation length
                        float length, lengthInv;

                        Data();
                        ~Data();

                };

                /**
                 * \brief Constructs mesh animation with given name.
                 * \param[in] name name of the mesh animation
                 */
                MeshAnimation(const char* name);
                ~MeshAnimation();

                /**
                 * \brief Prepares mesh animation.
                 * \return true if mesh animation was successfully prepared for rendering
                 */
                bool prepare();

                /**
                 * \brief Returns mesh animation data.
                 * \return reference to the mesh animation data
                 */
                Data& getData();

                /**
                 * \brief Returns interpolated mesh animation key.
                 * \param[in] scalar interpolation amount (float in [0; 1] range, where
                 * zero stands for the first animation key and one - for the last)
                 * \return const reference to the interpolated mesh animation key
                 */
                const Key& getInterpolatedKey(float scalar);

                /**
                 * \brief Returns mesh animation key.
                 * \param[in] index index of the mesh animation key
                 * \return const reference to the mesh animation key
                 */
                const Key& getKey(uint32_t index);

                /**
                 * \brief Returns number of mesh animation keys.
                 * \return number of mesh animation keys
                 */
                uint32_t getNumKeys();

        protected:
                // Data
                Data data_;

        };

}

#endif
