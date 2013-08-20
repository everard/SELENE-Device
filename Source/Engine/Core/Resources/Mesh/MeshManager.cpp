// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "MeshManager.h"
#include "../../Helpers/Utility.h"

namespace selene
{

        MeshManager::MeshManager(): textureManager_(nullptr), textureFactory_(nullptr)
        {
                const uint8_t vertexStreamStrides[Mesh::NUM_OF_VERTEX_STREAMS] =
                {
                        sizeof(Vector3d), sizeof(Vector3d) + sizeof(Vector4d),
                        sizeof(Vector2d), sizeof(Vector4d) + sizeof(Vector4d)
                };
                const bool vertexStreamPresenceFlags[Mesh::NUM_OF_VERTEX_STREAMS] = {true, true, true, false};

                for(uint8_t i = 0; i < Mesh::NUM_OF_VERTEX_STREAMS; ++i)
                {
                        vertexStreams_[i].stride    = vertexStreamStrides[i];
                        vertexStreams_[i].isPresent = vertexStreamPresenceFlags[i];
                }
        }
        MeshManager::~MeshManager() {}

        //--------------------------------------------------------------------------------------------------------
        bool MeshManager::readMesh(std::istream& stream,
                                   Mesh::Data& meshData,
                                   ResourceManager* textureManager,
                                   ResourceFactory* textureFactory)
        {
                textureManager_ = textureManager;
                textureFactory_ = textureFactory;

                if(!readHeader(stream, meshData))
                        return false;

                if(!readBoundingBox(stream, meshData.boundingBox))
                        return false;

                if(!readVerticesAndFaces(stream, meshData))
                        return false;

                if(!readSubsets(stream, meshData))
                        return false;

                if(static_cast<bool>(meshData.skeleton))
                {
                        if(!readBones(stream, meshData.skeleton->getBones()))
                                return false;

                        return meshData.skeleton->initialize();
                }

                return true;
        }

        //--------------------------------------------------------------------------------------------------------
        bool MeshManager::writeMesh(std::ostream& stream, const Mesh::Data& meshData)
        {
                if(!writeHeader(stream, meshData))
                        return false;

                if(!writeBoundingBox(stream, meshData.boundingBox))
                        return false;

                if(!writeVerticesAndFaces(stream, meshData))
                        return false;

                if(!writeSubsets(stream, meshData))
                        return false;

                if(static_cast<bool>(meshData.skeleton))
                        return writeBones(stream, meshData.skeleton->getBones());

                return true;
        }

        MeshManager::VertexStream::VertexStream(): stride(0), isPresent(false) {}
        MeshManager::VertexStream::~VertexStream() {}

        //--------------------------------------------------------------------------------------------------------
        bool MeshManager::readMaterial(std::istream& stream, Material& material)
        {
                if(!stream.good())
                        return false;

                uint8_t shadingType = 0, flags = 0;

                stream.read(reinterpret_cast<char*>(&shadingType), sizeof(uint8_t));
                material.setShadingType(shadingType);

                stream.read(reinterpret_cast<char*>(&flags), sizeof(uint8_t));
                material.setFlags(flags);

                Vector3d colors[NUM_OF_MATERIAL_COLOR_TYPES];
                float specularLevel, glossiness, opacity;

                stream.read(reinterpret_cast<char*>(colors),         sizeof(colors));
                stream.read(reinterpret_cast<char*>(&specularLevel), sizeof(float));
                stream.read(reinterpret_cast<char*>(&glossiness),    sizeof(float));
                stream.read(reinterpret_cast<char*>(&opacity),       sizeof(float));

                for(uint8_t i = 0; i < NUM_OF_MATERIAL_COLOR_TYPES; ++i)
                        material.setColor(colors[i], i);

                material.setSpecularLevel(specularLevel);
                material.setGlossiness(glossiness);
                material.setOpacity(opacity);

                char fileName[Utility::MAX_STRING_LENGTH];
                for(uint8_t j = 0; j < NUM_OF_TEXTURE_MAP_TYPES; ++j)
                {
                        if(Utility::readString(stream, fileName) && textureManager_ != nullptr)
                        {
                                material.setTextureMap(textureManager_->requestResource<Texture>(fileName), j);

                                if(*material.getTextureMap(j) == nullptr && textureFactory_ != nullptr)
                                {
                                        if(textureManager_->createResource(fileName, *textureFactory_) == SUCCESS)
                                        {
                                                auto textureMap =
                                                        textureManager_->requestResource<Texture>(fileName);
                                                material.setTextureMap(textureMap, j);
                                        }
                                }
                        }
                        else
                                material.setTextureMap(Resource::Instance<Texture>(), j);
                }

                return true;
        }

        //--------------------------------------------------------------------------------------------------------
        bool MeshManager::readHeader(std::istream& stream, Mesh::Data& meshData)
        {
                if(!stream.good())
                        return false;

                char header[4];

                stream.read(header, sizeof(header));
                if(std::memcmp(header, "SDMF", 4) != 0)
                        return false;

                uint32_t numVertices = 0, numFaces = 0;
                uint16_t numSubsets  = 0, numBones = 0;
                uint8_t  faceStride  = 0;

                stream.read(reinterpret_cast<char*>(&numVertices), sizeof(uint32_t));
                stream.read(reinterpret_cast<char*>(&numFaces),    sizeof(uint32_t));

                stream.read(reinterpret_cast<char*>(&numSubsets), sizeof(uint16_t));
                stream.read(reinterpret_cast<char*>(&numBones),   sizeof(uint16_t));

                stream.read(reinterpret_cast<char*>(&faceStride), sizeof(uint8_t));

                if(faceStride != 2 && faceStride != 4)
                        return false;

                vertexStreams_[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS].isPresent = (numBones > 0);
                for(uint8_t i = 0; i < Mesh::NUM_OF_VERTEX_STREAMS; ++i)
                {
                        if(!vertexStreams_[i].isPresent)
                                continue;

                        if(!meshData.vertices[i].create(numVertices, vertexStreams_[i].stride))
                                return false;
                }

                if(!meshData.faces.create(numFaces, faceStride, 3))
                        return false;

                if(!meshData.subsets.create(numSubsets))
                        return false;

                if(numBones > 0)
                {
                        meshData.skeleton.reset(new(std::nothrow) Skeleton);
                        if(!meshData.skeleton)
                                return false;

                        auto& bones = meshData.skeleton->getBones();
                        if(!bones.create(numBones))
                                return false;
                }
                else
                        meshData.skeleton.reset();

                return true;
        }

        //--------------------------------------------------------------------------------------------------------
        bool MeshManager::readBoundingBox(std::istream& stream, Box& boundingBox)
        {
                if(!stream.good())
                        return false;

                Vector3d vertices[8];
                stream.read(reinterpret_cast<char*>(vertices), sizeof(vertices));
                boundingBox.define(vertices);

                return true;
        }

        //--------------------------------------------------------------------------------------------------------
        bool MeshManager::readVerticesAndFaces(std::istream& stream, Mesh::Data& meshData)
        {
                if(!stream.good())
                        return false;

                for(uint8_t i = 0; i < Mesh::NUM_OF_VERTEX_STREAMS; ++i)
                {
                        if(!vertexStreams_[i].isPresent)
                                continue;

                        auto& vertexStream = meshData.vertices[i];
                        stream.read(reinterpret_cast<char*>(&vertexStream[0]),
                                    vertexStream.getSize() * vertexStream.getStride());
                }

                stream.read(reinterpret_cast<char*>(&meshData.faces[0]),
                            3 * meshData.faces.getSize() * meshData.faces.getStride());

                auto numVertices = meshData.vertices[Mesh::VERTEX_STREAM_POSITIONS].getSize();
                if(meshData.faces.getStride() == 2)
                {
                        uint16_t* faces = reinterpret_cast<uint16_t*>(&meshData.faces[0]);
                        for(uint32_t i = 0; i < 3 * meshData.faces.getSize(); ++i)
                        {
                                if(faces[i] >= numVertices)
                                        return false;
                        }
                }
                else
                {
                        uint32_t* faces = reinterpret_cast<uint32_t*>(&meshData.faces[0]);
                        for(uint32_t i = 0; i < 3 * meshData.faces.getSize(); ++i)
                        {
                                if(faces[i] >= numVertices)
                                        return false;
                        }
                }

                return true;
        }

        //--------------------------------------------------------------------------------------------------------
        bool MeshManager::readSubsets(std::istream& stream, Mesh::Data& meshData)
        {
                if(!stream.good())
                        return false;

                auto numVertices = meshData.vertices[Mesh::VERTEX_STREAM_POSITIONS].getSize();
                auto numFaces    = meshData.faces.getSize();

                for(uint16_t i = 0; i < meshData.subsets.getSize(); ++i)
                {
                        meshData.subsets[i].material.reset(new(std::nothrow) Material);
                        if(!meshData.subsets[i].material)
                                return false;

                        if(!readMaterial(stream, *meshData.subsets[i].material))
                                return false;

                        stream.read(reinterpret_cast<char*>(&meshData.subsets[i].vertexIndex),
                                    4 * sizeof(uint32_t));

                        Mesh::Subset& subset = meshData.subsets[i];

                        uint32_t nextVertexIndex = static_cast<uint32_t>(subset.vertexIndex + subset.numVertices);
                        uint32_t nextFaceIndex   = static_cast<uint32_t>(subset.faceIndex + subset.numFaces);

                        if(nextVertexIndex <= subset.vertexIndex || nextVertexIndex > numVertices ||
                           nextFaceIndex   <= subset.faceIndex   || nextFaceIndex   > numFaces)
                                return false;
                }

                return true;
        }

        //--------------------------------------------------------------------------------------------------------
        bool MeshManager::readBones(std::istream& stream, Array<Skeleton::Bone, uint16_t>& bones)
        {
                if(!stream.good())
                        return false;

                char boneName[Utility::MAX_STRING_LENGTH];

                try
                {
                        for(uint16_t i = 0; i < bones.getSize(); ++i)
                        {
                                Skeleton::Bone& bone = bones[i];

                                if(!Utility::readString(stream, boneName))
                                        return false;

                                bone.name = boneName;
                                stream.read(reinterpret_cast<char*>(&bone.offsetTransform.rotation),
                                            sizeof(Quaternion));
                                stream.read(reinterpret_cast<char*>(&bone.offsetTransform.position),
                                            sizeof(Vector3d));
                                stream.read(reinterpret_cast<char*>(&bone.parent), sizeof(int32_t));

                                if(bones[i].parent >= bones.getSize())
                                        return false;
                        }
                }
                catch(...)
                {
                        return false;
                }

                return true;
        }

        //--------------------------------------------------------------------------------------------------------
        bool MeshManager::writeMaterial(std::ostream& stream, const Material& material)
        {
                if(!stream.good())
                        return false;

                uint8_t shadingType = material.getShadingType();
                stream.write(reinterpret_cast<char*>(&shadingType), sizeof(uint8_t));

                uint8_t flags = material.getFlags();
                stream.write(reinterpret_cast<char*>(&flags), sizeof(uint8_t));

                Vector3d colors[NUM_OF_MATERIAL_COLOR_TYPES];
                float specularLevel, glossiness, opacity;

                for(uint8_t i = 0; i < NUM_OF_MATERIAL_COLOR_TYPES; ++i)
                        colors[i] = material.getColor(i);

                specularLevel = material.getSpecularLevel();
                glossiness = material.getGlossiness();
                opacity = material.getOpacity();

                stream.write(reinterpret_cast<char*>(colors),         sizeof(colors));
                stream.write(reinterpret_cast<char*>(&specularLevel), sizeof(float));
                stream.write(reinterpret_cast<char*>(&glossiness),    sizeof(float));
                stream.write(reinterpret_cast<char*>(&opacity),       sizeof(float));

                for(uint8_t j = 0; j < NUM_OF_TEXTURE_MAP_TYPES; ++j)
                {
                        Texture* texture = *material.getTextureMap(j);
                        if(texture != nullptr)
                                Utility::writeString(stream, texture->getName());
                        else
                                Utility::writeString(stream, nullptr);
                }

                return true;
        }

        //--------------------------------------------------------------------------------------------------------
        bool MeshManager::writeHeader(std::ostream& stream, const Mesh::Data& meshData)
        {
                if(!stream.good())
                        return false;

                stream.write("SDMF", 4);

                vertexStreams_[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS].isPresent =
                        static_cast<bool>(meshData.skeleton);
                for(uint8_t i = 0; i < Mesh::NUM_OF_VERTEX_STREAMS; ++i)
                {
                        if(!vertexStreams_[i].isPresent)
                                continue;

                        if(meshData.vertices[i].isEmpty())
                                return false;
                }

                uint8_t faceStride = meshData.faces.getStride();
                if(meshData.faces.isEmpty() || (faceStride != 2 && faceStride != 4))
                        return false;

                uint32_t numVertices = meshData.vertices[Mesh::VERTEX_STREAM_POSITIONS].getSize();
                uint32_t numFaces    = meshData.faces.getSize();

                if(meshData.subsets.isEmpty())
                        return false;

                uint16_t numSubsets = meshData.subsets.getSize(), numBones = 0;

                if(static_cast<bool>(meshData.skeleton))
                {
                        const auto& bones = meshData.skeleton->getBones();
                        if(bones.isEmpty())
                                return false;

                        numBones = bones.getSize();
                }

                stream.write(reinterpret_cast<char*>(&numVertices), sizeof(uint32_t));
                stream.write(reinterpret_cast<char*>(&numFaces),    sizeof(uint32_t));

                stream.write(reinterpret_cast<char*>(&numSubsets), sizeof(uint16_t));
                stream.write(reinterpret_cast<char*>(&numBones),   sizeof(uint16_t));

                stream.write(reinterpret_cast<char*>(&faceStride), sizeof(uint8_t));

                return true;
        }

        //--------------------------------------------------------------------------------------------------------
        bool MeshManager::writeBoundingBox(std::ostream& stream, const Box& boundingBox)
        {
                if(!stream.good())
                        return false;

                stream.write(reinterpret_cast<const char*>(boundingBox.getVertices()), 8 * sizeof(Vector3d));

                return true;
        }

        //--------------------------------------------------------------------------------------------------------
        bool MeshManager::writeVerticesAndFaces(std::ostream& stream, const Mesh::Data& meshData)
        {
                if(!stream.good())
                        return false;

                for(uint8_t i = 0; i < Mesh::NUM_OF_VERTEX_STREAMS; ++i)
                {
                        if(!vertexStreams_[i].isPresent)
                                continue;

                        auto& vertexStream = meshData.vertices[i];
                        stream.write(reinterpret_cast<const char*>(&vertexStream[0]),
                                     vertexStream.getSize() * vertexStream.getStride());
                }

                stream.write(reinterpret_cast<const char*>(&meshData.faces[0]),
                             3 * meshData.faces.getSize() * meshData.faces.getStride());

                return true;
        }

        //--------------------------------------------------------------------------------------------------------
        bool MeshManager::writeSubsets(std::ostream& stream, const Mesh::Data& meshData)
        {
                if(!stream.good())
                        return false;

                for(uint16_t i = 0; i < meshData.subsets.getSize(); ++i)
                {
                        if(!meshData.subsets[i].material)
                                return false;

                        if(!writeMaterial(stream, *meshData.subsets[i].material))
                                return false;

                        stream.write(reinterpret_cast<const char*>(&meshData.subsets[i].vertexIndex),
                                     4 * sizeof(uint32_t));
                }

                return true;
        }

        //--------------------------------------------------------------------------------------------------------
        bool MeshManager::writeBones(std::ostream& stream, const Array<Skeleton::Bone, uint16_t>& bones)
        {
                if(!stream.good())
                        return false;

                for(uint16_t i = 0; i < bones.getSize(); ++i)
                {
                        const Skeleton::Bone& bone = bones[i];

                        if(!Utility::writeString(stream, bone.name.c_str()))
                                return false;

                        stream.write(reinterpret_cast<const char*>(&bone.offsetTransform.rotation),
                                     sizeof(Quaternion));
                        stream.write(reinterpret_cast<const char*>(&bone.offsetTransform.position),
                                     sizeof(Vector3d));
                        stream.write(reinterpret_cast<const char*>(&bone.parent), sizeof(int32_t));
                }

                return true;
        }

}
