// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "DummyTextureFactory.h"
#include "Exporter.h"
#include <iostream>
#include <fstream>
#include <map>

namespace selene
{

        // Represents vertex with duplicates.
        class MultiVertex
        {
        public:
                // Represents face map.
                class FaceMap
                {
                public:
                        uint32_t faceNo, index;
                        uint32_t vertexIndex;

                };

                MultiVertex() {}
                ~MultiVertex() {}

                // Finds face map by key
                FaceMap* find(uint32_t key)
                {
                        auto it = duplicates_.find(key);
                        if(it == duplicates_.end())
                                return nullptr;

                        return &it->second;
                }

                // Inserts new duplicate
                void insert(uint32_t key, uint32_t faceNo, uint32_t index, uint32_t vertexIndex)
                {
                        FaceMap faceMap;

                        faceMap.vertexIndex = vertexIndex;
                        faceMap.faceNo = faceNo;
                        faceMap.index  = index;

                        duplicates_.insert(std::make_pair(key, faceMap));
                }

                // Updates faces with new indices
                void updateFaces(RawMeshData::Face* faces)
                {
                        for(auto it = duplicates_.begin(); it != duplicates_.end(); ++it)
                                faces[it->second.faceNo].indices[it->second.index] = it->second.vertexIndex;
                }

        private:
                std::map<uint32_t, FaceMap> duplicates_;

        };

        // Merges faces
        // INPUT:
        //       currentFaces - pointer to the array of current faces
        //       addedFaces   - array of faces, which are merged with current
        //       numVertices  - number of vertices
        //       numFaces     - number of faces
        // RETURNS: true on success, false otherwise.
        // OUTPUT: new faces (in currentFaces), new number of vertices (in numVertices)
        bool mergeFaces(RawMeshData::Face** currentFaces,
                        RawMeshData::Face* addedFaces,
                        uint32_t& numVertices,
                        uint32_t numFaces)
        {
                // validate
                if(currentFaces == nullptr)
                        return false;

                RawMeshData::Face* faces = *currentFaces;
                if(faces == nullptr || addedFaces == nullptr || numVertices == 0 || numFaces == 0)
                        return false;

                // create array of new faces
                RawMeshData::Face* newFaces = new(std::nothrow) RawMeshData::Face[numFaces];

                if(newFaces == nullptr)
                        return false;

                // create array of vertices with duplicates
                MultiVertex* vertices = new(std::nothrow) MultiVertex[numVertices];

                if(vertices == nullptr)
                {
                        SAFE_DELETE_ARRAY(newFaces);
                        return false;
                }

                // go through all faces and create duplicates
                uint32_t vertexIndex = 0;

                for(register uint32_t faceNo = 0; faceNo < numFaces; ++faceNo)
                {
                        for(register uint8_t i = 0; i < 3; ++i)
                        {
                                MultiVertex* multiVertex = &vertices[faces[faceNo].indices[i]];

                                uint32_t key = addedFaces[faceNo].indices[i];

                                MultiVertex::FaceMap* faceMap = multiVertex->find(key);

                                if(faceMap == nullptr)
                                {
                                        multiVertex->insert(key, faceNo, i, vertexIndex);
                                        ++vertexIndex;
                                }
                                else
                                        newFaces[faceNo].indices[i] = faceMap->vertexIndex;
                        }
                }

                // update faces
                for(register uint32_t i = 0; i < numVertices; ++i)
                        vertices[i].updateFaces(newFaces);

                // free memory
                SAFE_DELETE_ARRAY(vertices);
                SAFE_DELETE_ARRAY(faces);

                // set new faces as current
                *currentFaces = newFaces;

                // set new number of vertices
                numVertices = vertexIndex;

                return true;
        }

        Exporter::Exporter(RawMeshData* rawMeshData): mesh_(nullptr)
        {
                rawMeshData_ = rawMeshData;
                faces_ = nullptr;

                numVertices_  = 0;
                numFaces_     = 0;
        }
        Exporter::~Exporter()
        {
                clear();
        }

        //---------------------------------------------------------
        bool Exporter::doExport(const char* fileName)
        {
                clear();

                std::cout << "Starting export...";

                if(rawMeshData_ == nullptr)
                {
                        std::cout << "FAILED" << std::endl;
                        return false;
                }

                std::cout << "SUCCESS" << std::endl;

                if(rawMeshData_->positions_.isEmpty() || rawMeshData_->faces_.isEmpty() ||
                   rawMeshData_->textureCoordinates_.isEmpty() || rawMeshData_->textureFaces_.isEmpty())
                {
                        std::cout << "   ERROR: mesh is broken" << std::endl;
                        return false;
                }

                Mesh::Data& meshData = mesh_.getData();

                numVertices_ = rawMeshData_->positions_.getSize();
                numFaces_    = rawMeshData_->faces_.getSize();

                // make bounding box
                Vector3d center = (rawMeshData_->minBound_ + rawMeshData_->maxBound_) * 0.5f;
                float width  = rawMeshData_->maxBound_.x - rawMeshData_->minBound_.x;
                float height = rawMeshData_->maxBound_.y - rawMeshData_->minBound_.y;
                float depth  = rawMeshData_->maxBound_.z - rawMeshData_->minBound_.z;

                meshData.boundingBox.define(center, width, height, depth);

                // read faces
                std::cout << "Reading faces from raw mesh data...";

                if(!readFaces())
                {
                        std::cout << "FAILED" << std::endl;
                        return false;
                }

                std::cout << "SUCCESS" << std::endl;

                // merge with texture faces
                std::cout << "Merging faces with texture faces...";

                if(!mergeFaces(&faces_, &rawMeshData_->textureFaces_[0], numVertices_, numFaces_))
                {
                        std::cout << "FAILED" << std::endl;
                        return false;
                }

                std::cout << "SUCCESS" << std::endl;

                // create skin vertices
                if(rawMeshData_->hasSkeleton_)
                {
                        std::cout << "Creating skin vertices...";

                        if(!skinVertices_.create(numVertices_))
                        {
                                std::cout << "FAILED" << std::endl;
                                return false;
                        }

                        for(uint32_t i = 0; i < numFaces_; ++i)
                        {
                                const RawMeshData::Face& rawMeshDataFace =
                                        rawMeshData_->faces_[i];

                                for(uint8_t j = 0; j < 3; ++j)
                                {
                                        skinVertices_[faces_[i].indices[j]] =
                                                rawMeshData_->skinVertices_[rawMeshDataFace.indices[j]];
                                }
                        }

                        std::cout << "SUCCESS" << std::endl;
                }

                // mend the mesh
                std::vector<unsigned int> meshMenderFaces;
                meshMenderFaces.reserve(3 * numFaces_);

                meshMenderVertices_.reserve(numVertices_);
                for(uint32_t i = 0; i < numVertices_; ++i)
                        meshMenderVertices_.push_back(MeshMender::Vertex());

                for(uint32_t i = 0; i < numFaces_; ++i)
                {
                        const RawMeshData::Face& rawMeshDataFace =
                                rawMeshData_->faces_[i];

                        const RawMeshData::Face& rawMeshDataTextureFace =
                                rawMeshData_->textureFaces_[i];

                        for(uint8_t j = 0; j < 3; ++j)
                        {
                                meshMenderFaces.push_back(faces_[i].indices[j]);

                                MeshMender::Vertex& vertex = meshMenderVertices_[faces_[i].indices[j]];

                                const Vector3d& position =
                                        rawMeshData_->positions_[rawMeshDataFace.indices[j]];

                                const Vector2d& textureCoordinates =
                                        rawMeshData_->textureCoordinates_[rawMeshDataTextureFace.indices[j]];

                                vertex.pos = Vector3d(position.x, position.y, position.z);
                                vertex.s = textureCoordinates.x;
                                vertex.t = textureCoordinates.y;
                        }
                }

                std::cout << "Mending the mesh...";

                MeshMender meshMender;

                if(!meshMender.Mend(meshMenderVertices_, meshMenderFaces,
                                    newToOldVertexMapping_, 0.1f, 1.0f, 1.0f))
                {
                        std::cout << "FAILED" << std::endl;
                        return false;
                }

                numVertices_ = meshMenderVertices_.size();

                for(uint32_t i = 0; i < numFaces_; ++i)
                {
                        for(uint8_t j = 0; j < 3; ++j)
                                faces_[i].indices[j] = meshMenderFaces[3 * i + j];
                }

                std::cout << "SUCCESS" << std::endl;

                // create vertices and faces
                std::cout << "Creating vertices and faces...";

                if(!createVerticesAndFaces(meshData))
                {
                        std::cout << "FAILED" << std::endl;
                        return false;
                }

                std::cout << "SUCCESS" << std::endl;

                // create subsets
                std::cout << "Creating subsets...";

                if(!createSubsets(meshData))
                {
                        std::cout << "FAILED" << std::endl;
                        return false;
                }

                std::cout << "SUCCESS" << std::endl;

                // create bones
                if(rawMeshData_->hasSkeleton_)
                {
                        std::cout << "Creating bones...";

                        meshData.skeleton.reset(new(std::nothrow) Skeleton);
                        if(!meshData.skeleton)
                        {
                                std::cout << "FAILED" << std::endl;
                                return false;
                        }

                        meshData.skeleton->getBones() = rawMeshData_->bones_;

                        std::cout << "SUCCESS" << std::endl;
                }

                // save mesh
                MeshManager meshManager;

                // open file
                std::cout << "Creating output file...";
                std::ofstream stream(fileName, std::ios::binary);

                if(!stream.good())
                {
                        std::cout << "FAILED" << std::endl;
                        return false;
                }

                std::cout << "SUCCESS" << std::endl;

                // write mesh to file
                std::cout << "Writing mesh data to file...";

                if(!meshManager.writeMesh(stream, meshData))
                {
                        std::cout << "FAILED" << std::endl;
                        return false;
                }

                std::cout << "SUCCESS" << std::endl;
                std::cout << "Exported mesh has " << meshData.vertices[Mesh::VERTEX_STREAM_POSITIONS].getSize() <<
                             " vertices and " << meshData.faces.getSize() << " faces." << std::endl;

                return true;
        }

        //---------------------------------------------------------
        void Exporter::clear()
        {
                skinVertices_.destroy();
                newToOldVertexMapping_.clear();
                SAFE_DELETE_ARRAY(faces_);

                numVertices_  = 0;
                numFaces_     = 0;
        }

        //---------------------------------------------------------
        bool Exporter::readFaces()
        {
                faces_ = new(std::nothrow) RawMeshData::Face[rawMeshData_->faces_.getSize()];

                if(faces_ == nullptr)
                        return false;

                memcpy(&faces_[0], &rawMeshData_->faces_[0], rawMeshData_->faces_.getSize() *
                       sizeof(RawMeshData::Face));

                return true;
        }

        //---------------------------------------------------------
        bool Exporter::createVerticesAndFaces(Mesh::Data& meshData)
        {
                if(!meshData.vertices[Mesh::VERTEX_STREAM_POSITIONS].create(numVertices_, sizeof(Vector3d)) ||
                   !meshData.vertices[Mesh::VERTEX_STREAM_TBN_BASES].create(numVertices_, sizeof(Vector3d) + sizeof(Vector4d)) ||
                   !meshData.vertices[Mesh::VERTEX_STREAM_TEXTURE_COORDINATES].create(numVertices_, sizeof(Vector2d)))
                        return false;

                if(rawMeshData_->hasSkeleton_)
                        if(!meshData.vertices[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS].create(numVertices_, 2 * sizeof(Vector4d)))
                                return false;

                for(uint32_t i = 0; i < numFaces_; ++i)
                {
                        for(uint8_t j = 0; j < 3; ++j)
                        {
                                uint32_t vertexIndex = faces_[i].indices[j];
                                uint8_t* vertexPosition = &meshData.vertices[Mesh::VERTEX_STREAM_POSITIONS][vertexIndex *
                                        meshData.vertices[Mesh::VERTEX_STREAM_POSITIONS].getStride()];

                                const MeshMender::Vertex& menderVertex =
                                        meshMenderVertices_[faces_[i].indices[j]];

                                // position
                                Vector3d& position = *(reinterpret_cast<Vector3d*>(vertexPosition));

                                position.define(menderVertex.pos.x,
                                                menderVertex.pos.y,
                                                menderVertex.pos.z);

                                // normal
                                uint8_t* vertexTbnBasis = &meshData.vertices[Mesh::VERTEX_STREAM_TBN_BASES][vertexIndex *
                                        meshData.vertices[Mesh::VERTEX_STREAM_TBN_BASES].getStride()];
                                Vector3d& normal = *(reinterpret_cast<Vector3d*>(vertexTbnBasis));

                                normal.define(menderVertex.normal.x,
                                              menderVertex.normal.y,
                                              menderVertex.normal.z);
                                normal.normalize();

                                // tangent and binormal
                                Vector4d& tangent = *(reinterpret_cast<Vector4d*>(vertexTbnBasis + sizeof(Vector3d)));

                                Vector3d t = menderVertex.tangent;
                                Vector3d b = menderVertex.binormal;

                                t = t - normal.dot(t) * normal;
                                t.normalize();

                                b = b - normal.dot(b) * normal - t.dot(b) * t / t.dot(t);
                                b.normalize();

                                Vector3d crossProduct = normal * t;
                                if(crossProduct == b)
                                        tangent.define(t, 1.0f);
                                else if(crossProduct == -b)
                                        tangent.define(t, -1.0f);
                                else
                                {
                                        std::cout << "          Warning! V[" << faces_[i].indices[j] << "] has non ortho TBN basis" << std::endl;
                                        tangent = Vector4d(t, -1.0f);
                                }

                                // texture coordinates
                                uint8_t* vertexTextureCoordinates = &meshData.vertices[Mesh::VERTEX_STREAM_TEXTURE_COORDINATES][vertexIndex *
                                        meshData.vertices[Mesh::VERTEX_STREAM_TEXTURE_COORDINATES].getStride()];
                                Vector2d& textureCoordinates = *(reinterpret_cast<Vector2d*>(vertexTextureCoordinates));

                                textureCoordinates.define(menderVertex.s, menderVertex.t);

                                // fill weights and indices
                                if(rawMeshData_->hasSkeleton_)
                                {
                                        uint8_t* vertexIndicesAndWeights = &meshData.vertices[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS][vertexIndex *
                                                meshData.vertices[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS].getStride()];
                                        Vector4d& indices = *(reinterpret_cast<Vector4d*>(vertexIndicesAndWeights));
                                        Vector4d& weights = *(reinterpret_cast<Vector4d*>(vertexIndicesAndWeights + sizeof(Vector4d)));

                                        uint32_t oldVertexIndex = newToOldVertexMapping_[vertexIndex];
                                        indices = skinVertices_[oldVertexIndex].indices;
                                        weights = skinVertices_[oldVertexIndex].weights;

                                        if(fabs(weights.x + weights.y + weights.z + weights.w - 1.0f) > SELENE_EPSILON)
                                                std::cout << "          Warning! V[" << vertexIndex << "] has overall skin weights != 1.0" << std::endl;
                                }
                        }
                }

                // create faces
                uint8_t faceStride = 4;
                if(meshData.vertices[Mesh::VERTEX_STREAM_POSITIONS].getSize() <= 0xFFFF)
                        faceStride = 2;

                if(!meshData.faces.create(numFaces_, faceStride, 3))
                        return false;

                if(faceStride == 2)
                {
                        uint16_t* faces = reinterpret_cast<uint16_t*>(&meshData.faces[0]);

                        for(register uint32_t i = 0; i < meshData.faces.getSize(); ++i)
                        {
                                for(register uint8_t j = 0; j < 3; ++j)
                                        faces[3 * i + j] = static_cast<uint16_t>(faces_[i].indices[j]);
                        }
                }
                else
                {
                        uint32_t* faces = reinterpret_cast<uint32_t*>(&meshData.faces[0]);

                        for(register uint32_t i = 0; i < meshData.faces.getSize(); ++i)
                        {
                                for(register uint8_t j = 0; j < 3; ++j)
                                        faces[3 * i + j] = faces_[i].indices[j];
                        }
                }

                return true;
        }

        //---------------------------------------------------------
        bool Exporter::createSubsets(Mesh::Data& meshData)
        {
                if(!meshData.subsets.create(static_cast<uint16_t>(rawMeshData_->materials_.getSize())))
                        return false;

                // fill subsets
                DummyTextureFactory textureFactory;

                for(uint32_t i = 0; i < meshData.subsets.getSize(); ++i)
                {
                        Mesh::Subset& subset = meshData.subsets[i];
                        subset.material.reset(new(std::nothrow) Material);
                        if(!subset.material)
                                return false;

                        RawMeshData::Material& rawMaterial = rawMeshData_->materials_[i];

                        subset.faceIndex = rawMaterial.faceIndex;

                        // set blinn material
                        if(strncmp(reinterpret_cast<char*>(rawMaterial.type), "BLINN", 5) == 0)
                                subset.material->setShadingType(MATERIAL_SHADING_BLINN);
                        else if(strncmp(reinterpret_cast<char*>(rawMaterial.type), "PHONG", 5) == 0)
                                subset.material->setShadingType(MATERIAL_SHADING_PHONG);

                        Material& material = *subset.material;

                        if(rawMaterial.isTwoSided)
                                material.setFlags(MATERIAL_TWO_SIDED);

                        for(uint8_t j = 0; j < NUM_OF_MATERIAL_COLOR_TYPES; ++j)
                                material.setColor(rawMaterial.colors[j], j);

                        material.setSpecularLevel(rawMaterial.specularLevel);

                        if(rawMaterial.glossiness < 1.0f)
                                rawMaterial.glossiness = 1.0f;

                        material.setGlossiness(rawMaterial.glossiness);

                        material.setOpacity(rawMaterial.opacity);

                        for(uint8_t j = 0; j < NUM_OF_TEXTURE_MAP_TYPES; ++j)
                        {
                                if(rawMaterial.textureMapFileNames[j].length() > 0)
                                {
                                        const char* textureMapFileName = rawMaterial.textureMapFileNames[j].c_str();
                                        resourceManager_.createResource(textureMapFileName,
                                                                        textureFactory);

                                        material.setTextureMap(resourceManager_.requestResource<Texture>(textureMapFileName), j);
                                }
                        }
                }

                // validate
                uint32_t faceIndex = meshData.subsets[0].faceIndex;

                for(uint32_t i = 1; i < meshData.subsets.getSize(); ++i)
                {
                        if(faceIndex >= meshData.subsets[i].faceIndex)
                                return false;

                        meshData.subsets[i - 1].numFaces = meshData.subsets[i].faceIndex - faceIndex;
                        faceIndex = meshData.subsets[i].faceIndex;
                }

                meshData.subsets[meshData.subsets.getSize() - 1].numFaces = meshData.faces.getSize() - faceIndex;

                // compute min and max vertices for each subset
                for(uint32_t i = 0; i < meshData.subsets.getSize(); ++i)
                {
                        uint32_t firstFaceIndex = meshData.subsets[i].faceIndex;
                        uint32_t lastFaceIndex  = firstFaceIndex + meshData.subsets[i].numFaces;

                        uint32_t minVertexIndex = faces_[firstFaceIndex].indices[0];
                        uint32_t maxVertexIndex = faces_[firstFaceIndex].indices[0];

                        for(uint32_t j = firstFaceIndex; j < lastFaceIndex; ++j)
                        {
                                for(uint8_t k = 0; k < 3; ++k)
                                {
                                        if(minVertexIndex > faces_[j].indices[k])
                                                minVertexIndex = faces_[j].indices[k];

                                        if(maxVertexIndex < faces_[j].indices[k])
                                                maxVertexIndex = faces_[j].indices[k];
                                }
                        }

                        meshData.subsets[i].vertexIndex = minVertexIndex;
                        meshData.subsets[i].numVertices = maxVertexIndex - minVertexIndex + 1;
                }

                return true;
        }

}
