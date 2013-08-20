// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef MESH_MANAGER_H
#define MESH_MANAGER_H

#include "Mesh.h"

namespace selene
{

        /**
         * \addtogroup Resources
         * @{
         */

        /**
         * Represents mesh manager. Reads/writes meshes from/to istream/ostream.
         */
        class MeshManager
        {
        public:
                MeshManager();
                MeshManager(const MeshManager&) = default;
                ~MeshManager();
                MeshManager& operator =(const MeshManager&) = default;

                /**
                 * \brief Reads mesh.
                 * \param[in] stream std::istream from which mesh data is read
                 * \param[out] meshData mesh data
                 * \param[in] textureManager texture manager
                 * \param[in] textureFactory texture factory
                 * \return true on success
                 */
                bool readMesh(std::istream& stream,
                              Mesh::Data& meshData,
                              ResourceManager* textureManager,
                              ResourceFactory* textureFactory);

                /**
                 * \brief Writes mesh.
                 * \param[in] stream std::ostream to which mesh data is written
                 * \param[in] meshData mesh data
                 * \return true on success
                 */
                bool writeMesh(std::ostream& stream, const Mesh::Data& meshData);

        private:
                /**
                 * Represents vertex stream. Holds information about vertex stride and
                 * shows whether or not current stream is present in the mesh.
                 */
                class VertexStream
                {
                public:
                        uint8_t stride;
                        bool isPresent;

                        VertexStream();
                        VertexStream(const VertexStream&) = default;
                        ~VertexStream();
                        VertexStream& operator =(const VertexStream&) = default;

                };

                VertexStream vertexStreams_[Mesh::NUM_OF_VERTEX_STREAMS];
                ResourceManager* textureManager_;
                ResourceFactory* textureFactory_;

                /**
                 * \brief Reads material.
                 * \param[in] stream std::istream from which material is read
                 * \param[out] material material
                 * \return true on success
                 */
                bool readMaterial(std::istream& stream, Material& material);

                /**
                 * \brief Reads header.
                 * \param[in] stream std::istream from which header is read
                 * \param[out] meshData mesh data
                 * \return true on success
                 */
                bool readHeader(std::istream& stream, Mesh::Data& meshData);

                /**
                 * \brief Reads bounding box.
                 * \param[in] stream std::istream from which bounding box is read
                 * \param[out] boundingBox bounding box
                 * \return true on success
                 */
                bool readBoundingBox(std::istream& stream, Box& boundingBox);

                /**
                 * \brief Reads vertices and faces.
                 * \param[in] stream std::istream from which vertices and faces are read
                 * \param[out] meshData mesh data
                 * \return true on success
                 */
                bool readVerticesAndFaces(std::istream& stream, Mesh::Data& meshData);

                /**
                 * \brief Reads subsets.
                 * \param[in] stream std::istream from which subsets are read
                 * \param[out] meshData mesh data
                 * \return true on success
                 */
                bool readSubsets(std::istream& stream, Mesh::Data& meshData);

                /**
                 * \brief Reads bones.
                 * \param[in] stream std::istream from which bones are read
                 * \param[out] bones array of bones
                 * \return true on success
                 */
                bool readBones(std::istream& stream, Array<Skeleton::Bone, uint16_t>& bones);

                /**
                 * \brief Writes material.
                 * \param[in] stream std::ostream to which material is written
                 * \param[in] material material
                 * \return true on success
                 */
                bool writeMaterial(std::ostream& stream, const Material& material);

                /**
                 * \brief Writes header.
                 * \param[in] stream std::ostream to which header is written
                 * \param[in] meshData mesh data
                 * \return true on success
                 */
                bool writeHeader(std::ostream& stream, const Mesh::Data& meshData);

                /**
                 * \brief Writes bounding box.
                 * \param[in] stream std::ostream to which bounding box is written
                 * \param[in] boundingBox bounding box
                 * \return true on success
                 */
                bool writeBoundingBox(std::ostream& stream, const Box& boundingBox);

                /**
                 * \brief Writes vertices and faces.
                 * \param[in] stream std::ostream to which vertices and faces are written
                 * \param[in] meshData mesh data
                 * \return true on success
                 */
                bool writeVerticesAndFaces(std::ostream& stream, const Mesh::Data& meshData);

                /**
                 * \brief Writes subsets.
                 * \param[in] stream std::ostream to which subsets are written
                 * \param[in] meshData mesh data
                 * \return true on success
                 */
                bool writeSubsets(std::ostream& stream, const Mesh::Data& meshData);

                /**
                 * \brief Writes bones.
                 * \param[in] stream std::ostream to which bones are written
                 * \param[in] bones array of bones
                 * \return true on success
                 */
                bool writeBones(std::ostream& stream, const Array<Skeleton::Bone, uint16_t>& bones);

        };

        /**
         * @}
         */

}

#endif
