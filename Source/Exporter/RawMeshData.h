// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef RAW_MESH_DATA_H
#define RAW_MESH_DATA_H

#include "../Engine/Framework.h"

namespace selene
{

        /**
         * \addtogroup Exporter
         * @{
         */

        /**
         * Represents raw mesh data. The following list outlines its structure.
         * - Header, 7 bytes, must be "TEMPSLE";
         * - number of vertices (**nv**), uint32_t;
         * - number of faces (**nf**), uint32_t;
         * - number of texture coordinates (**ntc**), uint32_t;
         * - vertex positions, **nv** times Vector3d;
         * - texture coordinates, **ntc** times Vector2d;
         * - faces, **nf** times (3 * uint32_t);
         * - texture faces, **nf** times (3 * uint32_t);
         * - number of materials (**nm**), uint32_t;
         * - materials, **nm** times *material*;
         * - number of skin vertices (**nsv**), uint32_t;
         * - skin vertices, **nsv** times *skin vertex*;
         * - number of bones (**nb**), uint16_t;
         * - bones, **nb** times *bone*.
         *
         * The following list outlines **material** data structure.
         * - Index of the face, uint32_t;
         * - type, 5 bytes, must be "PHONG" or "BLINN";
         * - flag, which marks material as two-sided, uint8_t;
         * - colors, Vector3d for each material color;
         * - specular level, float;
         * - glossiness, float;
         * - opacity, float;
         * - texture names, *string* for each texture map.
         *
         * The following table outlines **skin vertex** data structure.
         * - Weights, Vector4d;
         * - bone indices, 4 times uint32_t.
         *
         * The following table outlines **bone** data structure.
         * - Name, *string*;
         * - rotation, Quaternion;
         * - position, Vector3d;
         * - index of the parent bone, int32_t.
         *
         * The following table outlines **string** data structure.
         * - Length (**len**), uint16_t;
         * - characters, **len** times char.
         */
        class RawMeshData
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

                        SkinVertex();
                        SkinVertex(const SkinVertex&) = default;
                        ~SkinVertex();
                        SkinVertex& operator =(const SkinVertex&) = default;

                };

                /**
                 * Represents face.
                 */
                class Face
                {
                public:
                        uint32_t indices[3];

                        Face();
                        Face(const Face&) = default;
                        ~Face();
                        Face& operator =(const Face&) = default;

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
                        Material(const Material&) = default;
                        ~Material();
                        Material& operator =(const Material&) = default;

                };

                RawMeshData();
                RawMeshData(const RawMeshData&) = delete;
                ~RawMeshData();
                RawMeshData& operator =(const RawMeshData&) = delete;

                /**
                 * \brief Reads raw mesh data from file.
                 * \param[in] fileName name of the file from which raw
                 * data will be read
                 * \return true if data has been successfully read
                 */
                bool read(const char* fileName);

        private:
                friend class Exporter;

                Array<Face, uint32_t> faces_;
                Array<Face, uint32_t> textureFaces_;

                Array<Vector3d, uint32_t> positions_;
                Array<Vector2d, uint32_t> textureCoordinates_;
                Array<SkinVertex, uint32_t> skinVertices_;

                Array<RawMeshData::Material, uint32_t> materials_;
                Array<Skeleton::Bone, uint16_t> bones_;
                Vector3d minBound_, maxBound_;
                bool hasSkeleton_;

                /**
                 * \brief Reads material.
                 * \param[in] stream std::istream from which material will be read
                 * \param[out] material material
                 * \return true if material has been successfully read
                 */
                bool readMaterial(std::istream& stream, RawMeshData::Material& material);

        };

        /**
         * @}
         */

}

#endif
