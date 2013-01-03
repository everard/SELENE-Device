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
         * Represents raw mesh data. The following table outlines its structure.
         * Field                                   | Size                                | Value
         * --------------------------------------- | ----------------------------------- | --------------------------------
         * Header                                  | 7 bytes                             | "TEMPSLE"
         * number of vertices - **nv**             | 4 bytes                             | unsigned integer
         * number of faces - **nf**                | 4 bytes                             | unsigned integer
         * number of texture coordinates - **ntc** | 4 bytes                             | unsigned integer
         * vertex positions                        | **nv** x sizeof(Vector3d)           | **nv** times Vector3d
         * texture coordinates                     | **ntc** x sizeof(Vector2d)          | **ntc** times Vector2d
         * faces                                   | **nf** x (12 bytes)                 | **nf** times 3 unsigned integers
         * texture faces                           | **nf** x (12 bytes)                 | **nf** times 3 unsigned integers
         * number of materials - **nm**            | 4 bytes                             | unsigned integer
         * materials                               | **nm** x *material* (see below)     | **nm** times *material*
         * number of skin vertices - **nsv**       | 4 bytes                             | unsigned integer
         * skin vertices                           | **nsv** x *skin vertex* (see below) | **nsv** times *skin vertex*
         * number of bones - **nb**                | 2 bytes                             | unsigned int
         * bones                                   | **nb** x *bone* (see below)         | **nb** times *bone*
         *
         * The following table outlines **material** data structure.
         * Field                                   | Size                                                    | Value
         * --------------------------------------- | ------------------------------------------------------- | --------------------------------
         * face index                              | 4 bytes                                                 | unsigned integer
         * type                                    | 5 bytes                                                 | must be "PHONG" or "BLINN"
         * flag, which marks material as two-sided | 1 byte                                                  | unsigned integer
         * colors                                  | selene::NUM_OF_MATERIAL_COLOR_TYPES x sizeof(Vector3d)  | Vector3d for each material color
         * specular level                          | 4 bytes                                                 | float
         * glossiness                              | 4 bytes                                                 | float
         * opacity                                 | 4 bytes                                                 | float
         * texture names                           | selene::NUM_OF_TEXTURE_MAP_TYPES x *string* (see below) | *string* for each texture map
         *
         * The following table outlines **skin vertex** data structure.
         * Field        | Size             | Value
         * ------------ | ---------------- | ------------------------
         * weights      | sizeof(Vector4d) | Vector4d
         * bone indices | 4 x (4 bytes)    | 4 times unsigned integer
         *
         * The following table outlines **bone** data structure.
         * Field                    | Size                 | Value
         * ------------------------ | -------------------- | --------------
         * name                     | *string* (see below) | *string*
         * rotation                 | sizeof(Quaternion)   | Quaternion
         * position                 | sizeof(Vector3d)     | Vector3d
         * index of the parent bone | 4 bytes              | signed integer
         *
         * The following table outlines **string** data structure.
         * Field            | Size          | Value
         * ---------------- | ------------- | ------------------
         * length - **len** | 2 bytes       | unsigned integer
         * characters       | **len** bytes | **len** characters
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
