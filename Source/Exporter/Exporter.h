// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef EXPORTER_H
#define EXPORTER_H

#include "NVMeshMender.h"
#include "RawMeshData.h"

namespace selene
{

        /**
         * Represents exporter.
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
                 * \return true if mesh was successfully exported
                 */
                bool doExport(const char* fileName);

        private:
                // Resource manager
                ResourceManager resourceManager_;

                // Mesh
                Mesh mesh_;

                // Raw mesh data
                RawMeshData* rawMeshData_;

                // Temporary faces
                RawMeshData::Face* faces_;

                // MeshMender vertices
                std::vector<MeshMender::Vertex> meshMenderVertices_;

                // MeshMender vertices mapping
                std::vector<unsigned int> newToOldVertexMapping_;

                // Skin vertices
                Array<RawMeshData::SkinVertex, uint32_t> skinVertices_;

                // Number of vertices
                uint32_t numVertices_;

                // Number of faces
                uint32_t numFaces_;

                // Vertex stride
                uint8_t vertexStride_;

                /**
                 * \brief Clears all temporary data.
                 */
                void clear();

                /**
                 * \brief Reads faces from raw mesh data.
                 * \return true if faces were successfully read
                 */
                bool readFaces();

                /**
                 * \brief Creates vertex elements.
                 * \param[out] meshData mesh data
                 * \return true if vertex elements were successfully created
                 */
                bool createVertexElements(Mesh::Data& meshData);

                /**
                 * \brief Creates vertices and faces.
                 * \param[out] meshData mesh data
                 * \return true if vertices and faces were successfully created
                 */
                bool createVerticesAndFaces(Mesh::Data& meshData);

                /**
                 * \brief Creates subsets.
                 * \param[out] meshData mesh data
                 * \return true if subsets were successfully created
                 */
                bool createSubsets(Mesh::Data& meshData);

        };

}

#endif
