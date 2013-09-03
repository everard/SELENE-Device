// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Exporter.h"

#include <iostream>
#include <fstream>

namespace selene
{

        Exporter::Exporter():
                rawMesh_(nullptr), meshData_(), faces_(), boneIndices_(),
                boneWeights_(), numVertices_(0), vertices_(), newToOldVertexMapping_() {}
        Exporter::~Exporter() {}

        //-------------------------------------------------------------------------------------------------------
        bool Exporter::processMesh(RawMesh& rawMesh, const char* fileName)
        {
                rawMesh_ = &rawMesh;
                meshData_.reset(new(std::nothrow) Mesh::Data);
                if(!meshData_)
                {
                        std::cout << "error: not enough memory" << std::endl;
                        return false;
                }

                numVertices_ = rawMesh_->positions_.getSize();
                meshData_->boundingBox = rawMesh_->boundingBox_;

                boneIndices_.destroy();
                boneWeights_.destroy();

                vertices_.clear();
                newToOldVertexMapping_.clear();

                std::cout << "processing raw mesh..." << std::endl;
                faces_ = rawMesh_->faces_;
                if(faces_.getSize() != rawMesh_->faces_.getSize())
                {
                        std::cout << "error: not enough memory" << std::endl;
                        return false;
                }

                std::cout << "merging faces..." << std::endl;

                numVertices_ = mergeFaces(faces_, rawMesh_->normalFaces_, faces_, numVertices_);
                if(numVertices_ == 0)
                {
                        std::cout << "error: could not merge faces" << std::endl;
                        return false;
                }

                numVertices_ = mergeFaces(faces_, rawMesh_->textureFaces_, faces_, numVertices_);
                if(numVertices_ == 0)
                {
                        std::cout << "error: could not merge faces" << std::endl;
                        return false;
                }

                if(!rawMesh_->bones_.isEmpty())
                {
                        std::cout << "reading bone indices and weights..." << std::endl;

                        if(!boneIndices_.create(numVertices_) || !boneWeights_.create(numVertices_))
                        {
                                std::cout << "error: not enough memory" << std::endl;
                                return false;
                        }

                        uint32_t numFaces = faces_.getSize();
                        for(uint32_t i = 0; i < numFaces; ++i)
                        {
                                const RawMesh::Face& face = rawMesh_->faces_[i];

                                for(uint8_t j = 0; j < 3; ++j)
                                {
                                        uint32_t vertexIndex = faces_[i].indices[j];
                                        boneIndices_[vertexIndex] = rawMesh_->boneIndices_[face.indices[j]];
                                        boneWeights_[vertexIndex] = rawMesh_->boneWeights_[face.indices[j]];
                                }
                        }
                }

                std::cout << "computing tangent space..." << std::endl;
                if(!computeTangentSpace())
                {
                        std::cout << "error: broken mesh" << std::endl;
                        return false;
                }

                std::cout << "preparing vertex streams..." << std::endl;
                if(!prepareVertexStreams())
                {
                        std::cout << "error: not enough memory" << std::endl;
                        return false;
                }

                std::cout << "preparing faces..." << std::endl;
                if(!prepareFaces())
                {
                        std::cout << "error: not enough memory" << std::endl;
                        return false;
                }

                std::cout << "preparing subsets..." << std::endl;
                if(!prepareSubsets())
                {
                        std::cout << "error: not enough memory" << std::endl;
                        return false;
                }

                if(!rawMesh_->bones_.isEmpty())
                {
                        std::cout << "preparing skeleton...";
                        auto& skeleton = meshData_->skeleton;

                        skeleton.reset(new(std::nothrow) Skeleton);
                        if(!skeleton)
                        {
                                std::cout << "error: not enough memory" << std::endl;
                                return false;
                        }

                        skeleton->getBones() = rawMesh_->bones_;
                        if(skeleton->getBones().getSize() != rawMesh_->bones_.getSize())
                        {
                                std::cout << "error: not enough memory" << std::endl;
                                return false;
                        }
                }

                std::cout << "writing mesh to the file..." << std::endl;

                std::ofstream stream(fileName, std::ios_base::binary);
                MeshManager meshManager;

                if(!meshManager.writeMesh(stream, *meshData_))
                {
                        std::cout << "error: could not write mesh to the file" << std::endl;
                        return false;
                }

                std::cout << "mesh has " << numVertices_;
                std::cout << " vertices and " << faces_.getSize() << " faces" << std::endl;

                return true;
        }

        //-------------------------------------------------------------------------------------------------------
        uint32_t Exporter::mergeFaces(const Array<RawMesh::Face, uint32_t>& faces0,
                                      const Array<RawMesh::Face, uint32_t>& faces1,
                                      Array<RawMesh::Face, uint32_t>& result,
                                      uint32_t numVertices)
        {
                uint32_t numFaces = faces0.getSize();
                if(numFaces == 0 || faces1.getSize() != numFaces)
                        return 0;

                Array<MultiVertex, uint32_t> vertices;
                if(!vertices.create(numVertices))
                        return 0;

                Array<RawMesh::Face, uint32_t> newFaces;
                if(!newFaces.create(numFaces))
                        return 0;

                uint32_t newVertexIndex = 0;

                try
                {
                        for(uint32_t faceNo = 0; faceNo < numFaces; ++faceNo)
                        {
                                for(uint8_t i = 0; i < 3; ++i)
                                {
                                        MultiVertex& multiVertex = vertices[faces0[faceNo].indices[i]];
                                        uint32_t vertexIndex = faces1[faceNo].indices[i];

                                        auto duplicate = multiVertex.find(vertexIndex);

                                        if(duplicate == multiVertex.end())
                                        {
                                                multiVertex.insert(std::make_pair(vertexIndex, newVertexIndex));
                                                newFaces[faceNo].indices[i] = newVertexIndex++;
                                        }
                                        else
                                                newFaces[faceNo].indices[i] = duplicate->second;
                                }
                        }
                }
                catch(...)
                {
                        return 0;
                }

                result = newFaces;
                if(result.getSize() != newFaces.getSize())
                        return 0;

                return newVertexIndex;
        }

        //-------------------------------------------------------------------------------------------------------
        bool Exporter::computeTangentSpace()
        {
                uint32_t numFaces = faces_.getSize();

                std::vector<unsigned int> newFaces;
                newFaces.reserve(3 * numFaces);

                vertices_.reserve(numVertices_);
                for(uint32_t i = 0; i < numVertices_; ++i)
                        vertices_.push_back(MeshMender::Vertex());

                for(uint32_t i = 0; i < numFaces; ++i)
                {
                        const RawMesh::Face& face = rawMesh_->faces_[i];
                        const RawMesh::Face& normalFace  = rawMesh_->normalFaces_[i];
                        const RawMesh::Face& textureFace = rawMesh_->textureFaces_[i];

                        for(uint8_t j = 0; j < 3; ++j)
                        {
                                uint32_t vertexIndex = faces_[i].indices[j];

                                newFaces.push_back(vertexIndex);
                                MeshMender::Vertex& vertex = vertices_[vertexIndex];

                                const auto& position = rawMesh_->positions_[face.indices[j]];
                                const auto& normal = rawMesh_->normals_[normalFace.indices[j]];
                                const auto& textureCoordinates =
                                        rawMesh_->textureCoordinates_[textureFace.indices[j]];

                                vertex.pos = position;
                                vertex.normal = normal;
                                vertex.s = textureCoordinates.x;
                                vertex.t = textureCoordinates.y;
                        }
                }

                MeshMender meshMender;
                if(!meshMender.Mend(vertices_, newFaces, newToOldVertexMapping_,
                                    0.0f, 0.0f, 0.0f, 1.0f, MeshMender::DONT_CALCULATE_NORMALS))
                        return false;

                numVertices_ = vertices_.size();
                for(uint32_t i = 0; i < numFaces; ++i)
                {
                        for(uint8_t j = 0; j < 3; ++j)
                                faces_[i].indices[j] = newFaces[3 * i + j];
                }

                return true;
        }

        //-------------------------------------------------------------------------------------------------------
        bool Exporter::prepareVertexStreams()
        {
                auto& textureCoordinates = meshData_->vertices[Mesh::VERTEX_STREAM_TEXTURE_COORDINATES];
                auto& positions          = meshData_->vertices[Mesh::VERTEX_STREAM_POSITIONS];
                auto& tbnBases           = meshData_->vertices[Mesh::VERTEX_STREAM_TBN_BASES];

                auto& boneIndicesAndWeights = meshData_->vertices[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS];

                if(!textureCoordinates.create(numVertices_, sizeof(Vector2d)) ||
                   !positions.create(numVertices_, sizeof(Vector3d))          ||
                   !tbnBases.create(numVertices_, sizeof(Vector3d) + sizeof(Vector4d)))
                        return false;

                if(!rawMesh_->bones_.isEmpty())
                {
                        if(!boneIndicesAndWeights.create(numVertices_, 2 * sizeof(Vector4d)))
                                return false;
                }

                for(uint32_t i = 0; i < faces_.getSize(); ++i)
                {
                        for(uint8_t j = 0; j < 3; ++j)
                        {
                                uint32_t vertexIndex = faces_[i].indices[j];
                                const auto& vertex = vertices_[vertexIndex];

                                uint8_t* stream = &positions[vertexIndex * positions.getStride()];
                                *reinterpret_cast<Vector3d*>(stream) = vertex.pos;

                                stream = &textureCoordinates[vertexIndex * textureCoordinates.getStride()];
                                *reinterpret_cast<Vector2d*>(stream) = Vector2d(vertex.s, vertex.t);

                                stream = &tbnBases[vertexIndex * tbnBases.getStride()];
                                Vector4d& tangent = *(reinterpret_cast<Vector4d*>(stream + sizeof(Vector3d)));
                                Vector3d& normal  = *(reinterpret_cast<Vector3d*>(stream));

                                normal = vertex.normal;
                                normal.normalize();

                                // tangent and binormal

                                Vector3d t = vertex.tangent;
                                Vector3d b = vertex.binormal;

                                t = t - normal.dot(t) * normal;
                                t.normalize();

                                b = b - normal.dot(b) * normal - t.dot(b) * t / t.dot(t);
                                b.normalize();

                                Vector3d crossProduct = normal.cross(t);
                                if(crossProduct == b)
                                        tangent.define(t, 1.0f);
                                else if(crossProduct == -b)
                                        tangent.define(t, -1.0f);
                                else
                                {
                                        std::cout << "warning: vertex " << vertexIndex <<
                                                     " has bad TBN basis" << std::endl;
                                        tangent = Vector4d(t, -1.0f);
                                }

                                if(!rawMesh_->bones_.isEmpty())
                                {
                                        stream = &boneIndicesAndWeights[vertexIndex *
                                                                        boneIndicesAndWeights.getStride()];

                                        Vector4d& boneIndices = *(reinterpret_cast<Vector4d*>(stream));
                                        Vector4d& boneWeights = *(reinterpret_cast<Vector4d*>(stream +
                                                                                              sizeof(Vector4d)));

                                        uint32_t oldVertexIndex = newToOldVertexMapping_[vertexIndex];
                                        boneIndices = boneIndices_[oldVertexIndex];
                                        boneWeights = boneWeights_[oldVertexIndex];

                                        if(std::fabs(boneWeights.x + boneWeights.y +
                                                     boneWeights.z + boneWeights.w - 1.0f) > SELENE_EPSILON)
                                                std::cout << "warning: vertex " << vertexIndex <<
                                                             " has bad bone weights" << std::endl;
                                }
                        }
                }

                return true;
        }

        //-------------------------------------------------------------------------------------------------------
        bool Exporter::prepareFaces()
        {
                uint32_t numFaces = faces_.getSize();

                uint8_t faceStride = 4;
                if(numVertices_ <= 0xFFFF)
                        faceStride = 2;

                if(!meshData_->faces.create(numFaces, faceStride, 3))
                        return false;

                if(faceStride == 2)
                {
                        uint16_t* faces = reinterpret_cast<uint16_t*>(&meshData_->faces[0]);

                        for(uint32_t i = 0; i < numFaces; ++i)
                        {
                                for(uint8_t j = 0; j < 3; ++j)
                                        faces[3 * i + j] = static_cast<uint16_t>(faces_[i].indices[j]);
                        }
                }
                else
                {
                        uint32_t* faces = reinterpret_cast<uint32_t*>(&meshData_->faces[0]);

                        for(uint32_t i = 0; i < numFaces; ++i)
                        {
                                for(uint8_t j = 0; j < 3; ++j)
                                        faces[3 * i + j] = faces_[i].indices[j];
                        }
                }

                return true;
        }

        //-------------------------------------------------------------------------------------------------------
        bool Exporter::prepareSubsets()
        {
                auto& subsets = meshData_->subsets;

                if(!subsets.create(rawMesh_->materials_.getSize()))
                        return false;

                for(uint16_t i = 0; i < subsets.getSize(); ++i)
                {
                        Mesh::Subset& subset = subsets[i];

                        subset.material = rawMesh_->materials_[i].first;
                        if(!subset.material)
                                return false;

                        subset.faceIndex = rawMesh_->materials_[i].second;
                }

                uint32_t faceIndex = subsets[0].faceIndex;
                for(uint16_t i = 1; i < subsets.getSize(); ++i)
                {
                        if(faceIndex >= subsets[i].faceIndex)
                                return false;

                        subsets[i - 1].numFaces = subsets[i].faceIndex - faceIndex;
                        faceIndex = subsets[i].faceIndex;
                }
                subsets[subsets.getSize() - 1].numFaces = meshData_->faces.getSize() - faceIndex;

                for(uint16_t i = 0; i < subsets.getSize(); ++i)
                {
                        uint32_t firstFaceIndex = subsets[i].faceIndex;
                        uint32_t lastFaceIndex  = firstFaceIndex + subsets[i].numFaces;

                        uint32_t minVertexIndex = faces_[firstFaceIndex].indices[0];
                        uint32_t maxVertexIndex = faces_[firstFaceIndex].indices[0];

                        for(uint32_t j = firstFaceIndex; j < lastFaceIndex; ++j)
                        {
                                for(uint8_t k = 0; k < 3; ++k)
                                {
                                        uint32_t vertexIndex = faces_[j].indices[k];
                                        if(minVertexIndex > vertexIndex)
                                                minVertexIndex = vertexIndex;

                                        if(maxVertexIndex < vertexIndex)
                                                maxVertexIndex = vertexIndex;
                                }
                        }

                        subsets[i].vertexIndex = minVertexIndex;
                        subsets[i].numVertices = maxVertexIndex - minVertexIndex + 1;
                }

                return true;
        }

}
