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

        /// Vertex element types and sizes
        enum
        {
                // Vertex element types
                VERTEX_ELEMENT_POSITION = 0,
                VERTEX_ELEMENT_TEXCOORD,
                VERTEX_ELEMENT_BINORMAL,
                VERTEX_ELEMENT_TANGENT,
                VERTEX_ELEMENT_NORMAL,

                // Vertex element sizes
                VERTEX_ELEMENT_SIZE_FLOAT1 = 4,
                VERTEX_ELEMENT_SIZE_FLOAT2 = 8,
                VERTEX_ELEMENT_SIZE_FLOAT3 = 12,
                VERTEX_ELEMENT_SIZE_FLOAT4 = 16
        };

        /**
         * Represents mesh.
         */
        class Mesh: public Resource
        {
        public:
                /**
                 * Represents mesh subset.
                 */
                class Subset
                {
                public:
                        // Index of the first vertex in subset
                        uint32_t vertexIndex;

                        // Number of vertices used in subset
                        uint32_t numVertices;

                        // Index of the first face in subset
                        uint32_t faceIndex;

                        // Number of faces in subset
                        uint32_t numFaces;

                        // Material
                        Material material;

                        Subset();
                        ~Subset();

                };

                /**
                 * Represents vertex element.
                 */
                class VertexElement
                {
                public:
                        // Type
                        uint8_t type;

                        // Size
                        uint8_t size;

                        // Offset
                        uint8_t offset;

                        // Index
                        uint8_t index;

                        /**
                         * \brief Constructs vertex element with given type, size, offset and index.
                         * \param[in] type_ type of the vertex element
                         * \param[in] size_ size of the vertex element
                         * \param[in] offset_ offset of the vertex element
                         * \param[in] index_ index of the vertex element
                         */
                        VertexElement(uint8_t type_ = VERTEX_ELEMENT_POSITION,
                                      uint8_t size_ = VERTEX_ELEMENT_SIZE_FLOAT1,
                                      uint8_t offset_ = 0,
                                      uint8_t index_  = 0);
                        ~VertexElement();

                };

                /**
                 * Represents mesh data container.
                 */
                class Data
                {
                public:
                        // Vertices
                        Array<uint8_t, uint32_t> vertices;

                        // Faces
                        Array<uint8_t, uint32_t> faces;

                        // Subsets
                        Array<Subset, uint16_t> subsets;

                        // Vertex elements
                        Array<VertexElement, uint8_t> vertexElements;

                        // Bounding box
                        Box boundingBox;

                        // Skeleton
                        Skeleton skeleton;

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
                // Data
                Data data_;

        };

}

#endif
