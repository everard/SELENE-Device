// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "MeshManager.h"

namespace selene
{

        MeshManager::MeshManager()
        {
                textureManager_ = nullptr;
                textureFactory_ = nullptr;
                hasSkeleton_ = false;
        }
        MeshManager::~MeshManager() {}

        //--------------------------------------------------------------------------------------------------------------------------------------
        bool MeshManager::readMesh(std::istream& stream,
                                   Mesh::Data& meshData,
                                   ResourceManager* textureManager,
                                   ResourceFactory* textureFactory)
        {
                textureManager_ = textureManager;
                textureFactory_ = textureFactory;

                // read header
                if(!readHeader(stream))
                        return false;

                // read bounding box
                if(!readBoundingBox(stream, meshData.boundingBox))
                        return false;

                // read vertices and faces
                if(!readVerticesAndFaces(stream, meshData))
                        return false;

                // read subsets
                if(!readSubsets(stream, meshData))
                        return false;

                // read bones
                if(hasSkeleton_)
                {
                        meshData.skeleton.reset(new(std::nothrow) Skeleton);
                        if(!meshData.skeleton)
                                return false;

                        if(!readBones(stream, meshData.skeleton->getBones()))
                                return false;

                        return meshData.skeleton->initialize();
                }

                return true;
        }

        //--------------------------------------------------------------------------------------------------------------------------------------
        bool MeshManager::writeMesh(std::ostream& stream, const Mesh::Data& meshData)
        {
                // write header
                hasSkeleton_ = static_cast<bool>(meshData.skeleton) ? true : false;
                if(!writeHeader(stream))
                        return false;

                // write bounding box
                if(!writeBoundingBox(stream, meshData.boundingBox))
                        return false;

                // write vertices and faces
                if(!writeVerticesAndFaces(stream, meshData))
                        return false;

                // write subsets
                if(!writeSubsets(stream, meshData))
                        return false;

                // write bones
                if(hasSkeleton_)
                        return writeBones(stream, meshData.skeleton->getBones());

                return true;
        }

        //--------------------------------------------------------------------------------------------------------------------------------------
        bool MeshManager::readString(std::istream& stream, char* string)
        {
                if(!stream.good())
                        return false;

                uint16_t length = 0;

                // read length
                stream.read(reinterpret_cast<char*>(&length), sizeof(uint16_t));

                // read string
                if(length != 0 && length < MAX_STRING_LENGTH)
                {
                        stream.read(string, length);
                        string[length] = 0;
                        return true;
                }

                stream.seekg(length, std::ios_base::cur);
                string[0] = 0;
                return false;
        }

        //--------------------------------------------------------------------------------------------------------------------------------------
        bool MeshManager::writeString(std::ostream& stream, const char* string)
        {
                if(!stream.good())
                        return false;

                uint16_t length = 0;

                if(string == nullptr)
                {
                        // write string length and return false if string is not passed
                        stream.write(reinterpret_cast<char*>(&length), sizeof(uint16_t));
                        return false;
                }

                // get string length
                size_t stringLength = strlen(string);
                length = (stringLength >= MAX_STRING_LENGTH) ? (MAX_STRING_LENGTH - 1) :
                         (static_cast<uint16_t>(stringLength));

                // write string length
                stream.write(reinterpret_cast<char*>(&length), sizeof(uint16_t));
                if(length == 0)
                        return false;

                // write string
                stream.write(string, length);

                return true;
        }

        //--------------------------------------------------------------------------------------------------------------------------------------
        bool MeshManager::readMaterial(std::istream& stream, Material& material)
        {
                if(!stream.good())
                        return false;

                uint8_t shadingType = 0, flags = 0;

                // read shading type
                stream.read(reinterpret_cast<char*>(&shadingType), sizeof(uint8_t));
                material.setShadingType(shadingType);

                // read flags
                stream.read(reinterpret_cast<char*>(&flags), sizeof(uint8_t));
                material.setFlags(flags);

                // read colors and other parameters
                char fileName[MAX_STRING_LENGTH];
                Vector3d colors[NUM_OF_MATERIAL_COLOR_TYPES];
                float specularLevel, glossiness, opacity;

                stream.read(reinterpret_cast<char*>(colors), sizeof(colors));
                stream.read(reinterpret_cast<char*>(&specularLevel), sizeof(float));
                stream.read(reinterpret_cast<char*>(&glossiness), sizeof(float));
                stream.read(reinterpret_cast<char*>(&opacity), sizeof(float));

                for(uint8_t i = 0; i < NUM_OF_MATERIAL_COLOR_TYPES; ++i)
                        material.setColor(colors[i], i);

                material.setSpecularLevel(specularLevel);
                material.setGlossiness(glossiness);
                material.setOpacity(opacity);

                // read texture maps
                for(register uint8_t j = 0; j < NUM_OF_TEXTURE_MAP_TYPES; ++j)
                {
                        if(readString(stream, fileName) && textureManager_ != nullptr)
                        {
                                material.setTextureMap(textureManager_->requestResource<Texture>(fileName), j);

                                if(*material.getTextureMap(j) == nullptr && textureFactory_ != nullptr)
                                {
                                        if(textureManager_->createResource(fileName, *textureFactory_) == SUCCESS)
                                                material.setTextureMap(textureManager_->requestResource<Texture>(fileName), j);
                                }
                        }
                        else
                                material.setTextureMap(Resource::Instance<Texture>(), j);
                }

                return true;
        }

        //--------------------------------------------------------------------------------------------------------------------------------------
        bool MeshManager::readHeader(std::istream& stream)
        {
                if(!stream.good())
                        return false;

                // read header
                char header[4];
                stream.read(header, sizeof(header));

                // validate header
                if(strncmp(header, "SLE", 3) != 0)
                        return false;

                hasSkeleton_ = (header[3] == '\0') ? false : true;

                return true;
        }

        //--------------------------------------------------------------------------------------------------------------------------------------
        bool MeshManager::readBoundingBox(std::istream& stream, Box& boundingBox)
        {
                if(!stream.good())
                        return false;

                // read bounding box
                Vector3d vertices[8];
                stream.read(reinterpret_cast<char*>(vertices), sizeof(vertices));
                boundingBox.define(vertices);

                // return true
                return true;
        }

        //--------------------------------------------------------------------------------------------------------------------------------------
        bool MeshManager::readVerticesAndFaces(std::istream& stream, Mesh::Data& meshData)
        {
                if(!stream.good())
                        return false;

                uint32_t numVertices = 0, numFaces = 0;
                uint8_t  faceStride = 0;

                // read number of vertices and faces
                stream.read(reinterpret_cast<char*>(&numVertices), sizeof(uint32_t));
                stream.read(reinterpret_cast<char*>(&numFaces),    sizeof(uint32_t));

                // read face stride
                stream.read(reinterpret_cast<char*>(&faceStride), sizeof(uint8_t));

                // validate face stride
                if(faceStride != 2 && faceStride != 4)
                        return false;

                // allocate memory for vertices and faces
                if(!meshData.vertices[Mesh::VERTEX_STREAM_POSITIONS].create(numVertices, sizeof(Vector3d)) ||
                   !meshData.vertices[Mesh::VERTEX_STREAM_TBN_BASES].create(numVertices, sizeof(Vector3d) + sizeof(Vector4d)) ||
                   !meshData.vertices[Mesh::VERTEX_STREAM_TEXTURE_COORDINATES].create(numVertices, sizeof(Vector2d)))
                        return false;

                if(hasSkeleton_)
                        if(!meshData.vertices[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS].create(numVertices, 2 * sizeof(Vector4d)))
                                return false;

                if(!meshData.faces.create(numFaces, faceStride, 3))
                        return false;

                // read vertices and faces
                for(uint8_t i = Mesh::VERTEX_STREAM_POSITIONS; i <= Mesh::VERTEX_STREAM_TEXTURE_COORDINATES; ++i)
                        stream.read(reinterpret_cast<char*>(&meshData.vertices[i][0]),
                                    meshData.vertices[i].getSize() * meshData.vertices[i].getStride());

                if(hasSkeleton_)
                        stream.read(reinterpret_cast<char*>(&meshData.vertices[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS][0]),
                                    meshData.vertices[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS].getSize() *
                                    meshData.vertices[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS].getStride());

                stream.read(reinterpret_cast<char*>(&meshData.faces[0]),
                            3 * meshData.faces.getSize() * meshData.faces.getStride());

                // validate faces
                if(meshData.faces.getStride() == 2)
                {
                        uint16_t* faces = reinterpret_cast<uint16_t*>(&meshData.faces[0]);
                        for(register uint32_t i = 0; i < 3 * meshData.faces.getSize(); ++i)
                        {
                                if(faces[i] >= numVertices)
                                        return false;
                        }
                }
                else
                {
                        uint32_t* faces = reinterpret_cast<uint32_t*>(&meshData.faces[0]);
                        for(register uint32_t i = 0; i < 3 * meshData.faces.getSize(); ++i)
                        {
                                if(faces[i] >= numVertices)
                                        return false;
                        }
                }

                return true;
        }

        //--------------------------------------------------------------------------------------------------------------------------------------
        bool MeshManager::readSubsets(std::istream& stream, Mesh::Data& meshData)
        {
                if(!stream.good())
                        return false;

                uint16_t numSubsets = 0;

                // read number of subsets
                stream.read(reinterpret_cast<char*>(&numSubsets), sizeof(uint16_t));

                // allocate memory for subsets
                if(!meshData.subsets.create(numSubsets))
                        return false;

                // read subsets
                for(register uint16_t i = 0; i < meshData.subsets.getSize(); ++i)
                {
                        // create material
                        meshData.subsets[i].material.reset(new(std::nothrow) Material);
                        if(!meshData.subsets[i].material)
                                return false;

                        // read material
                        if(!readMaterial(stream, *meshData.subsets[i].material))
                                return false;

                        // read subset data
                        stream.read(reinterpret_cast<char*>(&meshData.subsets[i].vertexIndex),
                                    4 * sizeof(uint32_t));

                        // validate
                        Mesh::Subset& subset = meshData.subsets[i];
                        if(static_cast<uint32_t>(subset.vertexIndex + subset.numVertices) <= subset.vertexIndex ||
                           static_cast<uint32_t>(subset.faceIndex + subset.numFaces) <= subset.faceIndex ||
                           (subset.vertexIndex + subset.numVertices) > meshData.vertices[Mesh::VERTEX_STREAM_POSITIONS].getSize() ||
                           (subset.faceIndex + subset.numFaces) > meshData.faces.getSize())
                                return false;
                }

                return true;
        }

        //--------------------------------------------------------------------------------------------------------------------------------------
        bool MeshManager::readBones(std::istream& stream, Array<Skeleton::Bone, uint16_t>& bones)
        {
                if(!stream.good())
                        return false;

                uint16_t numBones = 0;

                // read number of bones
                stream.read(reinterpret_cast<char*>(&numBones), sizeof(uint16_t));

                // allocate memory for bones
                if(!bones.create(numBones))
                        return false;

                char boneName[MAX_STRING_LENGTH];

                // read bones
                try
                {
                        for(register uint16_t i = 0; i < bones.getSize(); ++i)
                        {
                                Skeleton::Bone& bone = bones[i];

                                if(!readString(stream, boneName))
                                        return false;

                                bone.name = boneName;
                                stream.read(reinterpret_cast<char*>(&bone.offsetTransform.rotation), sizeof(Quaternion));
                                stream.read(reinterpret_cast<char*>(&bone.offsetTransform.position), sizeof(Vector3d));
                                stream.read(reinterpret_cast<char*>(&bone.parent), sizeof(int32_t));

                                // validate
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

        //--------------------------------------------------------------------------------------------------------------------------------------
        bool MeshManager::writeMaterial(std::ostream& stream, const Material& material)
        {
                if(!stream.good())
                        return false;

                // write shading type
                uint8_t shadingType = material.getShadingType();
                stream.write(reinterpret_cast<char*>(&shadingType), sizeof(uint8_t));

                // write flags
                uint8_t flags = material.getFlags();
                stream.write(reinterpret_cast<char*>(&flags), sizeof(uint8_t));

                // write colors and other parameters
                Vector3d colors[NUM_OF_MATERIAL_COLOR_TYPES];
                float specularLevel, glossiness, opacity;

                for(uint8_t i = 0; i < NUM_OF_MATERIAL_COLOR_TYPES; ++i)
                        colors[i] = material.getColor(i);

                specularLevel = material.getSpecularLevel();
                glossiness = material.getGlossiness();
                opacity = material.getOpacity();

                stream.write(reinterpret_cast<char*>(colors), sizeof(colors));
                stream.write(reinterpret_cast<char*>(&specularLevel), sizeof(float));
                stream.write(reinterpret_cast<char*>(&glossiness), sizeof(float));
                stream.write(reinterpret_cast<char*>(&opacity), sizeof(float));

                // write texture maps
                for(register uint8_t j = 0; j < NUM_OF_TEXTURE_MAP_TYPES; ++j)
                {
                        Texture* texture = *material.getTextureMap(j);
                        if(texture != nullptr)
                                writeString(stream, texture->getName());
                        else
                                writeString(stream, nullptr);
                }

                return true;
        }

        //--------------------------------------------------------------------------------------------------------------------------------------
        bool MeshManager::writeHeader(std::ostream& stream)
        {
                if(!stream.good())
                        return false;

                // write header
                stream.write("SLE", 3);

                if(hasSkeleton_)
                        stream.write("S", 1);
                else
                        stream.write("\0", 1);

                return true;
        }

        //--------------------------------------------------------------------------------------------------------------------------------------
        bool MeshManager::writeBoundingBox(std::ostream& stream, const Box& boundingBox)
        {
                if(!stream.good())
                        return false;

                // write bounding box
                stream.write(reinterpret_cast<const char*>(boundingBox.getVertices()),
                             8 * sizeof(Vector3d));

                return true;
        }

        //--------------------------------------------------------------------------------------------------------------------------------------
        bool MeshManager::writeVerticesAndFaces(std::ostream& stream, const Mesh::Data& meshData)
        {
                if(!stream.good())
                        return false;

                // validate
                for(uint8_t i = Mesh::VERTEX_STREAM_POSITIONS; i <= Mesh::VERTEX_STREAM_TEXTURE_COORDINATES; ++i)
                        if(meshData.vertices[i].isEmpty())
                                return false;

                if(hasSkeleton_ && meshData.vertices[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS].isEmpty())
                        return false;

                if(meshData.faces.isEmpty() || (meshData.faces.getStride() != 2 && meshData.faces.getStride() != 4))
                        return false;

                try
                {
                        // write number of vertices and faces
                        uint32_t numVertices = meshData.vertices[Mesh::VERTEX_STREAM_POSITIONS].getSize();
                        uint32_t numFaces    = meshData.faces.getSize();

                        stream.write(reinterpret_cast<char*>(&numVertices), sizeof(uint32_t));
                        stream.write(reinterpret_cast<char*>(&numFaces),    sizeof(uint32_t));

                        // write face stride
                        uint8_t faceStride = meshData.faces.getStride();
                        stream.write(reinterpret_cast<char*>(&faceStride), sizeof(uint8_t));

                        // write vertices and faces
                        for(uint8_t i = Mesh::VERTEX_STREAM_POSITIONS; i <= Mesh::VERTEX_STREAM_TEXTURE_COORDINATES; ++i)
                                stream.write(reinterpret_cast<const char*>(&meshData.vertices[i][0]),
                                             meshData.vertices[i].getSize() * meshData.vertices[i].getStride());

                        if(hasSkeleton_)
                                stream.write(reinterpret_cast<const char*>(&meshData.vertices[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS][0]),
                                             meshData.vertices[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS].getSize() *
                                             meshData.vertices[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS].getStride());

                        stream.write(reinterpret_cast<const char*>(&meshData.faces[0]),
                                     3 * meshData.faces.getSize() * meshData.faces.getStride());
                }
                catch(...)
                {
                        return false;
                }

                return true;
        }

        //--------------------------------------------------------------------------------------------------------------------------------------
        bool MeshManager::writeSubsets(std::ostream& stream, const Mesh::Data& meshData)
        {
                if(!stream.good())
                        return false;

                // validate
                if(meshData.subsets.isEmpty())
                        return false;

                try
                {
                        // write number of subsets
                        uint16_t numSubsets = meshData.subsets.getSize();
                        stream.write(reinterpret_cast<char*>(&numSubsets), sizeof(uint16_t));

                        // write subsets
                        for(register uint16_t i = 0; i < meshData.subsets.getSize(); ++i)
                        {
                                // check material
                                if(!meshData.subsets[i].material)
                                        return false;

                                // write material
                                if(!writeMaterial(stream, *meshData.subsets[i].material))
                                        return false;

                                // write subset data
                                stream.write(reinterpret_cast<const char*>(&meshData.subsets[i].vertexIndex),
                                             4 * sizeof(uint32_t));
                        }
                }
                catch(...)
                {
                        return false;
                }

                return true;
        }

        //--------------------------------------------------------------------------------------------------------------------------------------
        bool MeshManager::writeBones(std::ostream& stream, const Array<Skeleton::Bone, uint16_t>& bones)
        {
                if(!stream.good())
                        return false;

                // validate
                if(bones.isEmpty())
                        return false;

                try
                {
                        // write number of bones
                        uint16_t numBones = bones.getSize();
                        stream.write(reinterpret_cast<char*>(&numBones), sizeof(uint16_t));

                        // write bones
                        for(register uint16_t i = 0; i < bones.getSize(); ++i)
                        {
                                const Skeleton::Bone& bone = bones[i];

                                if(!writeString(stream, bone.name.c_str()))
                                        return false;

                                stream.write(reinterpret_cast<const char*>(&bone.offsetTransform.rotation),
                                             sizeof(Quaternion));
                                stream.write(reinterpret_cast<const char*>(&bone.offsetTransform.position),
                                             sizeof(Vector3d));
                                stream.write(reinterpret_cast<const char*>(&bone.parent), sizeof(int32_t));
                        }
                }
                catch(...)
                {
                        return false;
                }

                return true;
        }

}
