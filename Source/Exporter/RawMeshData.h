// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef RAW_MESH_DATA_H
#define RAW_MESH_DATA_H

#include "../Engine/Framework.h"

namespace selene
{

        /**
         * Represents raw mesh data.
         */
        class RawMeshData: protected MeshManager
        {
        public:
                /**
                 * Represents skin vertex.
                 */
                class SkinVertex
                {
                public:
                        Vector4d weights;
                        Vector4d indices;

                };

                /**
                 * Represents face.
                 */
                class Face
                {
                public:
                        uint32_t indices[3];

                        Face();
                        ~Face();

                };

                /**
                 * Represents material.
                 */
                class Material
                {
                public:
                        std::string textureMapFileNames[NUM_OF_TEXTURE_MAP_TYPES];
                        Vector3d colors[NUM_OF_MATERIAL_COLOR_TYPES];
                        float specularLevel, glossiness, opacity;

                        uint32_t faceIndex;

                        uint8_t type[5];
                        bool isTwoSided;

                        Material();
                        ~Material();

                };

                RawMeshData();
                ~RawMeshData();

                /**
                 * \brief Reads raw mesh data from file.
                 * \param[in] fileName name of the file from which raw
                 * data will be read
                 * \return true if data has been successfully read
                 */
                bool read(const char* fileName);

        private:
                friend class Exporter;

                // Faces
                Array<Face, uint32_t> faces_;
                Array<Face, uint32_t> textureFaces_;

                // Positions
                Array<Vector3d, uint32_t> positions_;

                // Texture coordinates
                Array<Vector2d, uint32_t> textureCoordinates_;

                // Skin vertices
                Array<SkinVertex, uint32_t> skinVertices_;

                // Materials
                Array<RawMeshData::Material, uint32_t> materials_;

                // Bones
                Array<Skeleton::Bone, uint16_t> bones_;

                // Min and max bounds
                Vector3d minBound_, maxBound_;

                // Skeleton existance flag
                bool hasSkeleton_;

                /**
                 * \brief Reads material.
                 * \param[in] stream std::istream from which material will be read
                 * \param[out] material material
                 * \return true if material has been successfully read
                 */
                bool readMaterial(std::istream& stream, RawMeshData::Material& material);

        };

}

#endif
