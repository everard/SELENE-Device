// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef RAW_MESH_H
#define RAW_MESH_H

#include "../Engine/Framework.h"

#include <utility>
#include <memory>

namespace selene
{

        /**
         * \addtogroup Exporter
         * \brief Application, which exports geometry to the engine's mesh format.
         * @{
         */

        /**
         * Represents raw mesh.
         */
        class RawMesh
        {
        public:
                /**
                 * Represents face.
                 */
                class Face
                {
                public:
                        uint32_t indices[3];

                };

                RawMesh();
                RawMesh(const RawMesh&) = delete;
                ~RawMesh();
                RawMesh& operator =(const RawMesh&) = delete;

                /**
                 * \brief Reads raw mesh from file.
                 * \param[in] fileName name of the file, from which mesh shall be read
                 * \return true if mesh has been successfully read
                 */
                bool read(const char* fileName);

        private:
                friend class Exporter;

                /**
                 * Represents dummy texture.
                 */
                class DummyTexture: public Texture
                {
                public:
                        /**
                         * \brief Constructs dummy texture with given name.
                         * \param[in] name name of the texture
                         */
                        DummyTexture(const char* name);
                        DummyTexture(const DummyTexture&) = delete;
                        ~DummyTexture() = default;
                        DummyTexture& operator =(const DummyTexture&) = delete;

                        // Texture interface implementation
                        bool retain();
                        void discard();

                };

                /**
                 * Represents dummy resource factory.
                 */
                template <class T> class DummyResourceFactory: public ResourceFactory
                {
                public:
                        DummyResourceFactory() = default;
                        DummyResourceFactory(const DummyResourceFactory&) = default;
                        ~DummyResourceFactory() = default;
                        DummyResourceFactory& operator =(const DummyResourceFactory&) = default;

                        /**
                         * \brief Creates dummy resource with given name.
                         * \param[in] name name of the resource
                         * \return pointer to the created resource, or nullptr if resource
                         * could not be created
                         */
                        Resource* createResource(const char* name)
                        {
                                if(name == nullptr)
                                        return nullptr;

                                return new(std::nothrow) T(name);
                        }

                };

                Array<Vector3d, uint32_t> positions_, normals_;
                Array<Vector2d, uint32_t> textureCoordinates_;
                Array<Vector4d, uint32_t> boneWeights_;
                Array<Vector4d, uint32_t> boneIndices_;

                Array<Face, uint32_t> faces_, normalFaces_, textureFaces_;

                Array<std::pair<std::shared_ptr<Material>, uint32_t>, uint16_t> materials_;
                Array<Skeleton::Bone, uint16_t> bones_;
                Box boundingBox_;

                ResourceManager resourceManager_;

                /**
                 * \brief Reads header.
                 * \param[in] stream std::istream, from which data shall be read
                 * \return true on success
                 */
                bool readHeader(std::istream& stream);

                /**
                 * \brief Reads vertices.
                 * \param[in] stream std::istream, from which data shall be read
                 * \return true on success
                 */
                bool readVertices(std::istream& stream);

                /**
                 * \brief Reads faces.
                 * \param[in] stream std::istream, from which data shall be read
                 * \return true on success
                 */
                bool readFaces(std::istream& stream);

                /**
                 * \brief Reads materials.
                 * \param[in] stream std::istream, from which data shall be read
                 * \return true on success
                 */
                bool readMaterials(std::istream& stream);

                /**
                 * \brief Reads material.
                 * \param[in] stream std::istream, from which data shall be read
                 * \param[out] material material
                 * \return true on success
                 */
                bool readMaterial(std::istream& stream, Material& material);

                /**
                 * \brief Reads bones.
                 * \param[in] stream std::istream, from which data shall be read
                 * \return true on success
                 */
                bool readBones(std::istream& stream);

        };

        /**
         * @}
         */

}

#endif
