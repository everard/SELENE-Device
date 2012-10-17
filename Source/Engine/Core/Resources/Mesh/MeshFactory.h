// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef MESH_FACTORY_H
#define MESH_FACTORY_H

#include "../ResourceFactory.h"
#include "MeshManager.h"

namespace selene
{

        /**
         * Represents mesh factory. This factory loads mesh data from file.
         */
        template <class T> class MeshFactory: public ResourceFactory
        {
        public:
                /**
                 * \brief Constructs mesh factory with given file manager.
                 * \param[in] fileManager file manager
                 */
                MeshFactory(FileManager* fileManager = nullptr): ResourceFactory(fileManager) {}
                ~MeshFactory() {}

                /**
                 * \brief Creates mesh.
                 *
                 * Mesh data is loaded from file.
                 * \param[in] name name of the mesh (and name of the file from which mesh is loaded)
                 * \return pointer to the created mesh, or nullptr if mesh could not be created
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

                        // create mesh
                        std::unique_ptr<T> resource(new(std::nothrow) T(name));

                        // validate
                        if(resource.get() == nullptr)
                                return nullptr;

                        // read mesh
                        MeshManager meshManager;
                        if(meshManager.readMesh(*stream, resource->getData(),
                                                resourceManager_, resourceFactory_))
                                return resource.release();

                        return nullptr;
                }

        };

}

#endif
