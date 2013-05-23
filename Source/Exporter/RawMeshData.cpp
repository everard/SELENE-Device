// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "RawMeshData.h"
#include <iostream>

namespace selene
{

        RawMeshData::Face::Face()
        {
                memset(indices, 0, sizeof(indices));
        }
        RawMeshData::Face::~Face() {}

        RawMeshData::Material::Material()
        {
                specularLevel = glossiness = opacity = 0.0f;
                faceIndex = 0;

                memset(type, 0, sizeof(type));
                isTwoSided = false;
        }
        RawMeshData::Material::~Material() {}

        RawMeshData::RawMeshData()
        {
                hasSkeleton_ = false;
        }
        RawMeshData::~RawMeshData() {}

        //--------------------------------------------------------------------------------------------------------
        bool RawMeshData::read(const char* fileName)
        {
                if(fileName == nullptr)
                        return false;

                std::cout << "Reading raw mesh data from file...";

                std::ifstream stream(fileName, std::ios::binary);
                if(!stream)
                {
                        std::cout << "FAILED\n";
                        return false;
                }
                std::cout << "\n   Reading mesh characteristics...";

                // read header
                char header[7];
                stream.read(header, sizeof(header));

                if(strncmp(header, "TEMPSLE", sizeof(header)))
                {
                        std::cout << "FAILED\n";
                        return false;
                }

                std::cout << "SUCCESS\n";

                // read vertices and faces
                uint32_t numVertices, numFaces, numTextureCoordinates;
                uint32_t numMaterials, numSkinVertices;
                uint16_t numBones;

                stream.read(reinterpret_cast<char*>(&numVertices), sizeof(uint32_t));
                stream.read(reinterpret_cast<char*>(&numFaces), sizeof(uint32_t));
                stream.read(reinterpret_cast<char*>(&numTextureCoordinates), sizeof(uint32_t));

                std::cout << "   Raw mesh has " << numVertices << " positions, " << numFaces << " faces, "
                          << numTextureCoordinates << " texture coordinates\n";

                std::cout << "   Allocating memory...";
                if(!faces_.create(numFaces) || !textureFaces_.create(numFaces) ||
                   !positions_.create(numVertices) ||  !textureCoordinates_.create(numTextureCoordinates))
                {
                        std::cout << "FAILED\n";
                        return false;
                }

                std::cout << "SUCCESS\n";

                // read vertex positions
                std::cout << "   Reading positions...";

                for(uint32_t i = 0; i < positions_.getSize(); ++i)
                        stream.read(reinterpret_cast<char*>(&positions_[i]), sizeof(Vector3d));

                std::cout << "SUCCESS\n";

                // compute min and max bounds
                minBound_ = maxBound_ = positions_[0];
                for(uint32_t i = 1; i < positions_.getSize(); ++i)
                {
                        const Vector3d& position = positions_[i];

                        if(position.x < minBound_.x)
                                minBound_.x = position.x;
                        if(position.y < minBound_.y)
                                minBound_.y = position.y;
                        if(position.z < minBound_.z)
                                minBound_.z = position.z;

                        if(position.x > maxBound_.x)
                                maxBound_.x = position.x;
                        if(position.y > maxBound_.y)
                                maxBound_.y = position.y;
                        if(position.z > maxBound_.z)
                                maxBound_.z = position.z;
                }

                // read texture coordinates
                std::cout << "   Reading texture coordinates...";

                for(uint32_t i = 0; i < textureCoordinates_.getSize(); ++i)
                        stream.read(reinterpret_cast<char*>(&textureCoordinates_[i]), sizeof(Vector2d));

                std::cout << "SUCCESS\n";

                // read faces
                std::cout << "   Reading faces...";
                for(uint32_t i = 0; i < faces_.getSize(); ++i)
                {
                        stream.read(reinterpret_cast<char*>(faces_[i].indices), sizeof(RawMeshData::Face));

                        if(faces_[i].indices[0] >= positions_.getSize() ||
                           faces_[i].indices[1] >= positions_.getSize() ||
                           faces_[i].indices[2] >= positions_.getSize())
                        {
                                std::cout << "FAILED (Wrong vertex index in face #" << i << ")\n";
                                return false;
                        }

                        stream.read(reinterpret_cast<char*>(textureFaces_[i].indices), sizeof(RawMeshData::Face));

                        if(textureFaces_[i].indices[0] >= textureCoordinates_.getSize() ||
                           textureFaces_[i].indices[1] >= textureCoordinates_.getSize() ||
                           textureFaces_[i].indices[2] >= textureCoordinates_.getSize())
                        {
                                std::cout << "FAILED (Wrong texture coordinate index in face #" << i << ")\n";
                                return false;
                        }
                }

                std::cout << "SUCCESS\n";

                // read materials
                std::cout << "   Reading materials...";
                stream.read(reinterpret_cast<char*>(&numMaterials), sizeof(uint32_t));

                if(!materials_.create(numMaterials))
                {
                        std::cout << "FAILED\n";
                        return false;
                }

                for(uint32_t i = 0; i < materials_.getSize(); ++i)
                {
                        if(!readMaterial(stream, materials_[i]))
                        {
                                std::cout << "FAILED\n";
                                return false;
                        }
                }

                std::cout << "SUCCESS\n";

                // read skin vertices
                std::cout << "   Reading skin vertices...";

                stream.read(reinterpret_cast<char*>(&numSkinVertices), sizeof(uint32_t));

                if(numSkinVertices > 0 && numSkinVertices == positions_.getSize())
                {
                        if(!skinVertices_.create(numSkinVertices))
                        {
                                std::cout << "FAILED\n";
                                return false;
                        }

                        for(uint32_t i = 0; i < skinVertices_.getSize(); ++i)
                        {
                                stream.read(reinterpret_cast<char*>(&skinVertices_[i].weights), sizeof(Vector4d));

                                int32_t indices[4];
                                stream.read(reinterpret_cast<char*>(indices), sizeof(indices));

                                skinVertices_[i].indices.define((float)indices[0],
                                                                (float)indices[1],
                                                                (float)indices[2],
                                                                (float)indices[3]);
                        }

                        stream.read(reinterpret_cast<char*>(&numBones), sizeof(uint16_t));

                        if(!bones_.create(numBones))
                        {
                                std::cout << "FAILED\n";
                                return false;
                        }

                        char boneName[MAX_STRING_LENGTH];

                        for(uint32_t i = 0; i < bones_.getSize(); ++i)
                        {
                                if(!readString(stream, boneName))
                                {
                                        std::cout << "FAILED\n";
                                        return false;
                                }

                                bones_[i].name = boneName;
                                stream.read(reinterpret_cast<char*>(&bones_[i].offsetTransform.rotation),
                                                                    sizeof(Quaternion));
                                stream.read(reinterpret_cast<char*>(&bones_[i].offsetTransform.position),
                                                                    sizeof(Vector3d));
                                stream.read(reinterpret_cast<char*>(&bones_[i].parent), sizeof(int32_t));
                                bones_[i].offsetTransform.rotation.normalize();
                        }

                        std::cout << "SUCCESS\n";
                        hasSkeleton_ = true;
                }
                else
                        std::cout << "SKIN NOT DETECTED\n";

                std::cout << "SUCCESS\n";
                return true;
        }

        //--------------------------------------------------------------------------------------------------------
        bool RawMeshData::readMaterial(std::istream& stream, RawMeshData::Material& material)
        {
                // read face index
                stream.read(reinterpret_cast<char*>(&material.faceIndex), sizeof(uint32_t));

                if(material.faceIndex >= faces_.getSize())
                        return false;

                // read material type
                stream.read(reinterpret_cast<char*>(material.type), sizeof(material.type));

                if(strncmp(reinterpret_cast<char*>(material.type), "BLINN", 5) != 0 &&
                   strncmp(reinterpret_cast<char*>(material.type), "PHONG", 5) != 0)
                        return false;

                // read flag
                uint8_t isTwoSided;
                stream.read(reinterpret_cast<char*>(&isTwoSided), sizeof(uint8_t));
                material.isTwoSided = (isTwoSided != 0) ? true : false;

                // read colors
                stream.read(reinterpret_cast<char*>(material.colors), sizeof(material.colors));

                // read specular level, glossiness and opacity
                stream.read(reinterpret_cast<char*>(&material.specularLevel), sizeof(float));
                stream.read(reinterpret_cast<char*>(&material.glossiness), sizeof(float));
                stream.read(reinterpret_cast<char*>(&material.opacity), sizeof(float));

                // read texture map file names
                char textureMapFileName[MAX_STRING_LENGTH];

                for(uint32_t i = 0; i < NUM_OF_TEXTURE_MAP_TYPES; ++i)
                {
                        if(readString(stream, textureMapFileName))
                                material.textureMapFileNames[i] = textureMapFileName;
                }

                return true;
        }

}
