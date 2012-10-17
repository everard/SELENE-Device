// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef MESH_ANIMATION_FACTORY_H
#define MESH_ANIMATION_FACTORY_H

#include "MeshAnimationManager.h"
#include "../ResourceFactory.h"

namespace selene
{

        /**
         * Represents mesh animation factory.
         */
        template <class T> class MeshAnimationFactory: public ResourceFactory
        {
        public:
                /**
                 * \brief Constructs mesh animation factory with given file manager.
                 * \param[in] fileManager file manager
                 */
                MeshAnimationFactory(FileManager* fileManager = nullptr): ResourceFactory(fileManager) {}
                ~MeshAnimationFactory() {}

                /**
                 * \brief Creates mesh animation.
                 *
                 * Mesh animation data is loaded from file.
                 * \param[in] name name of the mesh animation (and name of the file from which
                 * mesh animation is loaded)
                 * \return pointer to the created mesh animation, or nullptr if mesh animation
                 * could not be created
                 */
                Resource* createResource(const char* name)
                {
                        // validate
                        if(name == nullptr || fileManager_ == nullptr)
                                return nullptr;

                        // open file
                        std::unique_ptr<std::istream> stream(fileManager_->open(name));
                        if(stream.get() == nullptr)
                                return nullptr;

                        // create mesh animation
                        std::unique_ptr<T> resource(new(std::nothrow) T(name));

                        // validate
                        if(resource.get() == nullptr)
                                return nullptr;

                        // read mesh animation
                        MeshAnimationManager meshAnimationManager;
                        if(meshAnimationManager.readMeshAnimation(*stream, resource->getData()))
                                return resource.release();

                        return nullptr;
                }

        };

}

#endif
