// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "RawMesh.h"
#include <iostream>

namespace selene
{

        RawMesh::RawMesh():
                positions_(), boneWeights_(), boneIndices_(), textureCoordinates_(),
                faces_(), textureFaces_(), materials_(), bones_(), resourceManager_() {}
        RawMesh::~RawMesh() {}

        //-----------------------------------------------------------------------------------------------------------
        bool RawMesh::read(const char* fileName)
        {
                if(fileName == nullptr)
                        return false;

                std::ifstream stream(fileName, std::ios::binary);
                if(!stream)
                {
                        std::cout << "error: could not open file" << std::endl;
                        return false;
                }

                if(!readHeader(stream))
                        return false;

                if(!readVertices(stream))
                        return false;

                if(!readFaces(stream))
                        return false;

                if(!readMaterials(stream))
                        return false;

                if(!readBones(stream))
                        return false;

                if(stream.fail())
                {
                        std::cout << "error: broken file" << std::endl;
                        return false;
                }

                return true;
        }

        RawMesh::DummyTexture::DummyTexture(const char* name): Texture(name) {}

        //-----------------------------------------------------------------------------------------------------------
        bool RawMesh::DummyTexture::retain()
        {
                return true;
        }

        //-----------------------------------------------------------------------------------------------------------
        void RawMesh::DummyTexture::discard() {}

        //-----------------------------------------------------------------------------------------------------------
        bool RawMesh::readHeader(std::istream& stream)
        {
                if(!stream.good())
                {
                        std::cout << "error: broken file" << std::endl;
                        return false;
                }

                uint32_t numPositions, numTextureCoordinates, numFaces, numMaterials;
                uint16_t numBones;
                char header[4];

                stream.read(header, sizeof(header));

                if(std::memcmp(header, "SDIF", sizeof(header)) != 0)
                {
                        std::cout << "error: broken file" << std::endl;
                        return false;
                }

                stream.read(reinterpret_cast<char*>(&numPositions),          sizeof(uint32_t));
                stream.read(reinterpret_cast<char*>(&numTextureCoordinates), sizeof(uint32_t));

                stream.read(reinterpret_cast<char*>(&numFaces), sizeof(uint32_t));

                stream.read(reinterpret_cast<char*>(&numMaterials), sizeof(uint32_t));
                stream.read(reinterpret_cast<char*>(&numBones),     sizeof(uint16_t));

                bool isAllocationSuccessful = positions_.create(numPositions);
                isAllocationSuccessful = isAllocationSuccessful && textureCoordinates_.create(numTextureCoordinates);

                isAllocationSuccessful = isAllocationSuccessful && faces_.create(numFaces);
                isAllocationSuccessful = isAllocationSuccessful && textureFaces_.create(numFaces);

                isAllocationSuccessful = isAllocationSuccessful && materials_.create(numMaterials);

                if(numBones > 0)
                {
                        isAllocationSuccessful = isAllocationSuccessful && boneWeights_.create(numPositions);
                        isAllocationSuccessful = isAllocationSuccessful && boneIndices_.create(numPositions);

                        isAllocationSuccessful = isAllocationSuccessful && bones_.create(numBones);
                }

                if(!isAllocationSuccessful)
                {
                        std::cout << "error: not enough memory" << std::endl;
                        return false;
                }

                return true;
        }

        //-----------------------------------------------------------------------------------------------------------
        bool RawMesh::readVertices(std::istream& stream)
        {
                if(!stream.good())
                {
                        std::cout << "error: broken file" << std::endl;
                        return false;
                }

                stream.read(reinterpret_cast<char*>(&positions_[0]), positions_.getSize() * sizeof(Vector3d));

                if(!bones_.isEmpty())
                {
                        stream.read(reinterpret_cast<char*>(&boneWeights_[0]),
                                    boneWeights_.getSize() * sizeof(Vector4d));
                        stream.read(reinterpret_cast<char*>(&boneIndices_[0]),
                                    boneIndices_.getSize() * sizeof(Vector4d));
                }

                stream.read(reinterpret_cast<char*>(&textureCoordinates_[0]),
                            textureCoordinates_.getSize() * sizeof(Vector2d));

                bounds_[0] = bounds_[1] = positions_[0];
                for(uint32_t i = 1; i < positions_.getSize(); ++i)
                {
                        const Vector3d& position = positions_[i];

                        if(position.x < bounds_[0].x)
                                bounds_[0].x = position.x;
                        if(position.y < bounds_[0].y)
                                bounds_[0].y = position.y;
                        if(position.z < bounds_[0].z)
                                bounds_[0].z = position.z;

                        if(position.x > bounds_[1].x)
                                bounds_[1].x = position.x;
                        if(position.y > bounds_[1].y)
                                bounds_[1].y = position.y;
                        if(position.z > bounds_[1].z)
                                bounds_[1].z = position.z;
                }

                return true;
        }

        //-----------------------------------------------------------------------------------------------------------
        bool RawMesh::readFaces(std::istream& stream)
        {
                if(!stream.good())
                {
                        std::cout << "error: broken file" << std::endl;
                        return false;
                }

                auto numFaces = faces_.getSize();
                auto numPositions = positions_.getSize();
                auto numTextureCoordinates = textureCoordinates_.getSize();

                stream.read(reinterpret_cast<char*>(faces_[0].indices), numFaces * sizeof(Face));
                stream.read(reinterpret_cast<char*>(textureFaces_[0].indices), numFaces * sizeof(Face));

                for(uint32_t i = 0; i < numFaces; ++i)
                {
                        uint32_t* face = faces_[i].indices;
                        if(face[2] >= numPositions || face[0] >= numPositions || face[1] >= numPositions)
                        {
                                std::cout << "error: face " << i << " contains bad reference" << std::endl;
                                return false;
                        }

                        face = textureFaces_[i].indices;
                        if(face[2] >= numTextureCoordinates ||
                           face[0] >= numTextureCoordinates ||
                           face[1] >= numTextureCoordinates)
                        {
                                std::cout << "error: texture face " << i << " contains bad reference" << std::endl;
                                return false;
                        }
                }

                return true;
        }

        //-----------------------------------------------------------------------------------------------------------
        bool RawMesh::readMaterials(std::istream& stream)
        {
                if(!stream.good())
                {
                        std::cout << "error: broken file" << std::endl;
                        return false;
                }

                for(uint32_t i = 0; i < materials_.getSize(); ++i)
                {
                        auto& material = materials_[i].first;
                        material.reset(new(std::nothrow) Material);
                        if(!material)
                        {
                                std::cout << "error: not enough memory" << std::endl;
                                return false;
                        }

                        stream.read(reinterpret_cast<char*>(&materials_[i].second), sizeof(uint32_t));

                        if(materials_[i].second >= faces_.getSize())
                        {
                                std::cout << "error: broken material data" << std::endl;
                                return false;
                        }

                        if(!readMaterial(stream, *material.get()))
                        {
                                std::cout << "error: could not read material " << i << std::endl;
                                return false;
                        }
                }

                return true;
        }

        //-----------------------------------------------------------------------------------------------------------
        bool RawMesh::readMaterial(std::istream& stream, Material& material)
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
                DummyResourceFactory<DummyTexture> textureFactory;
                for(uint8_t j = 0; j < NUM_OF_TEXTURE_MAP_TYPES; ++j)
                {
                        if(!Utility::readString(stream, fileName))
                                continue;

                        resourceManager_.createResource(fileName, textureFactory);
                        material.setTextureMap(resourceManager_.requestResource<Texture>(fileName), j);
                }

                return true;
        }

        //-----------------------------------------------------------------------------------------------------------
        bool RawMesh::readBones(std::istream& stream)
        {
                if(bones_.getSize() == 0)
                        return true;

                if(!stream.good())
                {
                        std::cout << "error: broken file" << std::endl;
                        return false;
                }

                char boneName[Utility::MAX_STRING_LENGTH];
                for(uint32_t i = 0; i < bones_.getSize(); ++i)
                {
                        if(!Utility::readString(stream, boneName))
                        {
                                std::cout << "error: could not read name of the bone " << i << std::endl;
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

                return true;
        }

}
