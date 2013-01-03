// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef EXPORTER_H
#define EXPORTER_H

#include "NVMeshMender.h"
#include "RawMeshData.h"

namespace selene
{

        /**
         * \addtogroup Exporter
         * @{
         */

        /**
         * Represents exporter. Processes RawMeshData and saves the result as SLE model.
         * Uses nvMeshMender to compute tangent-bitangent-normal basis for vertices.
         */
        class Exporter
        {
        public:
                /**
                 * \brief Constructs exporter with given raw mesh data.
                 * \param[in] rawMeshData raw mesh data
                 */
                Exporter(RawMeshData* rawMeshData);
                ~Exporter();

                /**
                 * \brief Exports mesh.
                 * \param[in] fileName name of the file which will hold exported mesh
                 * \return true if mesh has been successfully exported
                 */
                bool doExport(const char* fileName);

        private:
                ResourceManager resourceManager_;
                Mesh mesh_;

                RawMeshData* rawMeshData_;
                RawMeshData::Face* faces_;

                std::vector<MeshMender::Vertex> meshMenderVertices_;
                std::vector<unsigned int> newToOldVertexMapping_;

                Array<RawMeshData::SkinVertex, uint32_t> skinVertices_;
                uint32_t numVertices_;
                uint32_t numFaces_;

                /**
                 * \brief Clears all temporary data.
                 */
                void clear();

                /**
                 * \brief Reads faces from raw mesh data.
                 * \return true if faces have been successfully read
                 */
                bool readFaces();

                /**
                 * \brief Creates vertices and faces.
                 * \param[out] meshData mesh data
                 * \return true if vertices and faces have been successfully created
                 */
                bool createVerticesAndFaces(Mesh::Data& meshData);

                /**
                 * \brief Creates subsets.
                 * \param[out] meshData mesh data
                 * \return true if subsets have been successfully created
                 */
                bool createSubsets(Mesh::Data& meshData);

        };

        /**
         * @}
         */

}

#endif
