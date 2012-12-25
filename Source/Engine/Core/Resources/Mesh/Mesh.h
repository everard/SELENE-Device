// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef MESH_H
#define MESH_H

#include "../../Material/Material.h"
#include "../ResourceManager.h"
#include "../../Math/Box.h"
#include "Skeleton.h"

namespace selene
{

        /**
         * \addtogroup Resources
         * @{
         */

        /**
         * Represents mesh.
         * \see Mesh::Data for more info.
         */
        class Mesh: public Resource
        {
        public:
                /// Vertex streams
                enum
                {
                        VERTEX_STREAM_POSITIONS = 0,
                        VERTEX_STREAM_TBN_BASES,
                        VERTEX_STREAM_TEXTURE_COORDINATES,
                        VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS,
                        NUM_OF_VERTEX_STREAMS
                };

                /**
                 * Represents mesh subset.
                 */
                class Subset
                {
                public:
                        uint32_t vertexIndex;
                        uint32_t numVertices;
                        uint32_t faceIndex;
                        uint32_t numFaces;
                        std::shared_ptr<Material> material;

                        Subset();
                        ~Subset();

                };

                /**
                 * Represents mesh data container. Mesh data consists of vertices, faces, subsets,
                 * bounding box and skeleton. Vertices consist of streams, which hold specific data,
                 * such as, positions, tangent-bitangent-normal bases, texture coordinates, bone
                 * indices and weights. Subsets split mesh into submeshes with different materials.
                 */
                class Data
                {
                public:
                        Array<uint8_t, uint32_t> vertices[NUM_OF_VERTEX_STREAMS], faces;
                        Array<Subset, uint16_t> subsets;
                        Box boundingBox;
                        std::shared_ptr<Skeleton> skeleton;

                };

                /**
                 * \brief Constructs mesh with given name.
                 * \param[in] name name of the mesh
                 */
                Mesh(const char* name);
                ~Mesh();

                /**
                 * \brief Prepares mesh.
                 * \return true if mesh successfully prepared for rendering
                 */
                bool prepare();

                /**
                 * \brief Returns mesh data.
                 * \return reference to the mesh data
                 */
                Data& getData();

                /**
                 * \brief Returns true if mesh has skeleton.
                 * \return true if mesh has skeleton
                 */
                bool hasSkeleton() const;

        protected:
                Data data_;

        };

        /**
         * @}
         */

}

#endif
