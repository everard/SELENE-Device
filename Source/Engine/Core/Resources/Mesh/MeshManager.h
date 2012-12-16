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
         * Represents mesh manager. Reads/writes meshes from/to std::istream/std::ostream.
         */
        class MeshManager
        {
        public:
                MeshManager();
                ~MeshManager();

                /**
                 * \brief Reads mesh.
                 * \param[in] stream std::istream from which mesh data is read
                 * \param[out] meshData mesh data
                 * \param[in] textureManager texture manager
                 * \param[in] textureFactory texture factory
                 * \return true if mesh was successfully read
                 */
                bool readMesh(std::istream& stream,
                              Mesh::Data& meshData,
                              ResourceManager* textureManager,
                              ResourceFactory* textureFactory);

                /**
                 * \brief Writes mesh.
                 * \param[in] stream std::ostream to which mesh data is written
                 * \param[in] meshData mesh data
                 * \return true if mesh was successfully written
                 */
                bool writeMesh(std::ostream& stream, const Mesh::Data& meshData);

        protected:
                enum
                {
                        MAX_STRING_LENGTH = 1024
                };

                /**
                 * \brief Reads string.
                 * \param[in] stream std::istream from which string is read
                 * \param[out] string c-string
                 * \return true if string was successfully read
                 */
                bool readString(std::istream& stream, char* string);

                /**
                 * \brief Writes string.
                 * \param[in] stream std::ostream to which string is written
                 * \param[in] string c-string
                 * \return true if string was successfully written
                 */
                bool writeString(std::ostream& stream, const char* string);

        private:
                ResourceManager* textureManager_;
                ResourceFactory* textureFactory_;
                bool hasSkeleton_;

                /**
                 * \brief Reads material.
                 * \param[in] stream std::istream from which material is read
                 * \param[out] material material
                 * \return true if material was successfully read
                 */
                bool readMaterial(std::istream& stream, Material& material);

                /**
                 * \brief Reads header.
                 * \param[in] stream std::istream from which header is read
                 * \return true if header was successfully read
                 */
                bool readHeader(std::istream& stream);

                /**
                 * \brief Reads bounding box.
                 * \param[in] stream std::istream from which bounding box is read
                 * \param[out] boundingBox bounding box
                 * \return true if bounding box was successfully read
                 */
                bool readBoundingBox(std::istream& stream, Box& boundingBox);

                /**
                 * \brief Reads vertices and faces.
                 * \param[in] stream std::istream from which vertices and faces are read
                 * \param[out] meshData mesh data
                 * \return true if vertices and faces were successfully read
                 */
                bool readVerticesAndFaces(std::istream& stream, Mesh::Data& meshData);

                /**
                 * \brief Reads subsets.
                 * \param[in] stream std::istream from which subsets are read
                 * \param[out] meshData mesh data
                 * \return true if subsets were successfully read
                 */
                bool readSubsets(std::istream& stream, Mesh::Data& meshData);

                /**
                 * \brief Reads bones.
                 * \param[in] stream std::istream from which bones are read
                 * \param[out] bones array of bones
                 * \return true if bones were successfully read
                 */
                bool readBones(std::istream& stream, Array<Skeleton::Bone, uint16_t>& bones);

                /**
                 * \brief Writes material.
                 * \param[in] stream std::ostream to which material is written
                 * \param[in] material material
                 * \return true if material was successfully written
                 */
                bool writeMaterial(std::ostream& stream, const Material& material);

                /**
                 * \brief Writes header.
                 * \param[in] stream std::ostream to which header is written
                 * \return true if header was successfully written
                 */
                bool writeHeader(std::ostream& stream);

                /**
                 * \brief Writes bounding box.
                 * \param[in] stream std::ostream to which bounding box is written
                 * \param[in] boundingBox bounding box
                 * \return true if bounding box was successfully written
                 */
                bool writeBoundingBox(std::ostream& stream, const Box& boundingBox);

                /**
                 * \brief Writes vertices and faces.
                 * \param[in] stream std::ostream to which vertices and faces are written
                 * \param[in] meshData mesh data
                 * \return true if vertices and faces were successfully written
                 */
                bool writeVerticesAndFaces(std::ostream& stream, const Mesh::Data& meshData);

                /**
                 * \brief Writes subsets.
                 * \param[in] stream std::ostream to which subsets are written
                 * \param[in] meshData mesh data
                 * \return true if subsets were successfully written
                 */
                bool writeSubsets(std::ostream& stream, const Mesh::Data& meshData);

                /**
                 * \brief Writes bones.
                 * \param[in] stream std::ostream to which bones are written
                 * \param[in] bones array of bones
                 * \return true if bones were successfully written
                 */
                bool writeBones(std::ostream& stream, const Array<Skeleton::Bone, uint16_t>& bones);

        };

        /**
         * @}
         */

}

#endif
