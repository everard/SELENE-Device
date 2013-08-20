// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef EXPORTER_H
#define EXPORTER_H

#include "NVMeshMender.h"
#include "RawMesh.h"

namespace selene
{

        /**
         * \addtogroup Exporter
         * @{
         */

        /**
         * Represents exporter. Processes RawMeshData and saves the result as SLE model.
         * Uses NVMeshMender to compute tangent-bitangent-normal basis for vertices.
         */
        class Exporter
        {
        public:
                /**
                 * \brief Constructs exporter with given raw mesh.
                 * \param[in] rawMesh raw mesh
                 */
                Exporter(RawMesh& rawMesh);
                Exporter(const Exporter&) = delete;
                ~Exporter();
                Exporter& operator =(const Exporter&) = delete;

                /**
                 * \brief Exports mesh.
                 * \param[in] fileName name of the file which will hold exported mesh
                 * \return true if mesh has been successfully exported
                 */
                bool doExport(const char* fileName);

        private:
                /**
                 * Represents dummy mesh.
                 */
                class DummyMesh: public Mesh
                {
                public:
                        DummyMesh(const char* name);
                        DummyMesh(const DummyMesh&) = delete;
                        ~DummyMesh();
                        DummyMesh& operator =(const DummyMesh&) = delete;

                        // selene::Resource interface implementation
                        bool retain();
                        void discard();

                };

                ResourceManager resourceManager_;
                DummyMesh mesh_;

                RawMesh& rawMesh_;
                RawMesh::Face* faces_;

                std::vector<MeshMender::Vertex> meshMenderVertices_;
                std::vector<unsigned int> newToOldVertexMapping_;

                //Array<RawMeshData::SkinVertex, uint32_t> skinVertices_;
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
