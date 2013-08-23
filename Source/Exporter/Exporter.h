// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef EXPORTER_H
#define EXPORTER_H

#include "NVMeshMender.h"
#include "RawMesh.h"

#include <memory>
#include <map>

namespace selene
{

        /**
         * \addtogroup Exporter
         * @{
         */

        /**
         * Represents exporter. Processes RawMesh and saves the result in engine's mesh format.
         * Uses NVMeshMender to compute TBN basis for vertices.
         */
        class Exporter
        {
        public:
                Exporter();
                Exporter(const Exporter&) = delete;
                ~Exporter();
                Exporter& operator =(const Exporter&) = delete;

                /**
                 * \brief Processes mesh and writes result to the file.
                 * \param[in] rawMesh raw mesh, which shall be processed
                 * \param[in] fileName name of the file, which will hold exported mesh
                 * \return true on success
                 */
                bool processMesh(RawMesh& rawMesh, const char* fileName);

        private:
                /**
                 * Represents vertex with duplicates.
                 */
                typedef std::map<uint32_t, uint32_t> MultiVertex;

                RawMesh* rawMesh_;
                std::unique_ptr<Mesh::Data> meshData_;
                Array<RawMesh::Face, uint32_t> faces_;

                Array<Vector4d, uint32_t> boneIndices_;
                Array<Vector4d, uint32_t> boneWeights_;

                uint32_t numVertices_;

                std::vector<MeshMender::Vertex> vertices_;
                std::vector<unsigned int> newToOldVertexMapping_;

                /**
                 * \brief Merges two sets of faces into one.
                 * \param[in] faces0 the first set of faces
                 * \param[in] faces1 the second set of faces
                 * \param[out] result new set of faces, which is the result of merge operation
                 * \param[in] numVertices number of vertices
                 * \return new number of vertices on success, zero on failure
                 */
                uint32_t mergeFaces(const Array<RawMesh::Face, uint32_t>& faces0,
                                    const Array<RawMesh::Face, uint32_t>& faces1,
                                    Array<RawMesh::Face, uint32_t>& result,
                                    uint32_t numVertices);

                /**
                 * \brief Computes tangent space.
                 * \return true on success
                 */
                bool computeTangentSpace();

                /**
                 * \brief Prepares vertex streams.
                 * \return true on success
                 */
                bool prepareVertexStreams();

                /**
                 * \brief Prepares faces.
                 * \return true on success
                 */
                bool prepareFaces();

                /**
                 * \brief Prepares mesh subsets.
                 * \return true on success
                 */
                bool prepareSubsets();

        };

        /**
         * @}
         */

}

#endif
