// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef MESH_ANIMATION_MANAGER_H
#define MESH_ANIMATION_MANAGER_H

#include "../Mesh/MeshManager.h"
#include "MeshAnimation.h"

namespace selene
{

        /**
         * Represents mesh animation manager.
         */
        class MeshAnimationManager: protected MeshManager
        {
        public:
                MeshAnimationManager();
                ~MeshAnimationManager();

                /**
                 * \brief Reads mesh animation.
                 * \param[in] stream std::istream from which mesh animation data is read
                 * \param[out] meshAnimationData mesh animation data
                 * \return true if mesh animation was successfully read
                 */
                bool readMeshAnimation(std::istream& stream,
                                       MeshAnimation::Data& meshAnimationData);

        };

}

#endif
