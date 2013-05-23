// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef D3D9_ACTORS_RENDERER_H
#define D3D9_ACTORS_RENDERER_H

#include "../../../../../Engine/Core/Resources/Mesh/Mesh.h"
#include "../../../../../Engine/Rendering/Renderer.h"
#include "D3D9Shader.h"

namespace selene
{

        /**
         * \addtogroup Windows
         * @{
         */

        // Forward declaration of classes
        class D3d9RenderTargetContainer;
        class D3d9FrameParameters;
        class D3d9TextureHandler;
        class D3d9Capabilities;

        /**
         * Represents actors renderer. Renders positions map, normals map, shadow map and shading.
         */
        class D3d9ActorsRenderer
        {
        public:
                D3d9ActorsRenderer();
                ~D3d9ActorsRenderer();

                /**
                 * \brief Initializes actors renderer.
                 * \param[in] renderTargetContainer render target container
                 * \param[in] frameParameters frame parameters
                 * \param[in] textureHandler texture handler
                 * \param[in] capabilities D3D capabilities
                 * \return true if actors renderer has been successfully initialized
                 */
                bool initialize(D3d9RenderTargetContainer& renderTargetContainer,
                                D3d9FrameParameters& frameParameters,
                                D3d9TextureHandler& textureHandler,
                                D3d9Capabilities& capabilities);

                /**
                 * \brief Destroys actors renderer.
                 */
                void destroy();

                /**
                 * \brief Renders positions and normals from given actor node.
                 * \param[in] actorNode actor node
                 */
                void renderPositionsAndNormals(Renderer::Data::ActorNode& actorNode);

                /**
                 * \brief Renders shadow map from given actor node with given projection parameters of the light.
                 * \param[in] actorNode actor node
                 * \param[in] projectionParameters light projection parameters
                 */
                void renderShadowMap(Renderer::Data::ActorNode& actorNode,
                                     const Vector4d& projectionParameters);

                /**
                 * \brief Renders shading.
                 * \param[in] actorNode actor node
                 * \param[in] isSsaoEnabled if set to true, then SSAO buffer will be used in shading
                 */
                void renderShading(Renderer::Data::ActorNode& actorNode, bool isSsaoEnabled);

        private:
                /// Helper constants
                enum
                {
                        RENDERING_PASS_POSITIONS = 0,
                        RENDERING_PASS_NORMALS,
                        RENDERING_PASS_SHADING,
                        RENDERING_PASS_POSITIONS_AND_NORMALS,
                        NUM_OF_RENDERING_PASSES,

                        VERTEX_SHADER_POSITIONS_PASS = 0,
                        VERTEX_SHADER_SKIN_POSITIONS_PASS,
                        VERTEX_SHADER_NORMALS_PASS,
                        VERTEX_SHADER_SKIN_NORMALS_PASS,
                        VERTEX_SHADER_SHADING_PASS,
                        VERTEX_SHADER_SKIN_SHADING_PASS,
                        NUM_OF_VERTEX_SHADERS,

                        PIXEL_SHADER_POSITIONS_PASS = 0,
                        PIXEL_SHADER_NORMALS_PASS,
                        PIXEL_SHADER_SHADING_PASS,
                        PIXEL_SHADER_SHADING_PASS_WITH_SSAO,
                        NUM_OF_PIXEL_SHADERS,

                        OPTIONAL_VERTEX_SHADER_POSITIONS_AND_NORMALS_PASS = 0,
                        OPTIONAL_VERTEX_SHADER_SKIN_POSITIONS_AND_NORMALS_PASS,
                        NUM_OF_OPTIONAL_VERTEX_SHADERS,

                        OPTIONAL_PIXEL_SHADER_POSITIONS_AND_NORMALS_PASS = 0,
                        NUM_OF_OPTIONAL_PIXEL_SHADERS,

                        MAX_VERTEX_STREAMS_PER_PASS = 3,
                        MAX_NUM_OF_BONES_IN_MODEL = 100,

                        LOCATION_WORLD_VIEW_PROJECTION_MATRIX = 0,
                        LOCATION_WORLD_VIEW_MATRIX = 4,
                        LOCATION_NORMALS_MATRIX    = 8,
                        LOCATION_BONE_ROTATIONS    = 12,
                        LOCATION_BONE_POSITIONS    = LOCATION_BONE_ROTATIONS + MAX_NUM_OF_BONES_IN_MODEL,

                        LOCATION_TEXTURE_COORDINATES_ADJUSTMENT = 4,
                        LOCATION_PROJECTION_PARAMETERS = 0,

                        LOCATION_NORMAL_MAP_NORMALS_PASS   = 0,
                        LOCATION_SPECULAR_MAP_NORMALS_PASS = 1,
                        LOCATION_SPECULAR_PARAMETERS_NORMALS_PASS = 1,

                        LOCATION_AMBIENT_MAP_SHADING_PASS  = 0,
                        LOCATION_DIFFUSE_MAP_SHADING_PASS  = 1,
                        LOCATION_SPECULAR_MAP_SHADING_PASS = 2,
                        LOCATION_LIGHT_BUFFER_SHADING_PASS = 3,
                        LOCATION_SSAO_BUFFER_SHADING_PASS  = 4,

                        LOCATION_AMBIENT_COLOR_SHADING_PASS = 0,
                        LOCATION_DIFFUSE_COLOR_SHADING_PASS = 1,
                        LOCATION_SPECULAR_COLOR_SHADING_PASS = 2,
                        LOCATION_SPECULAR_PARAMETERS_SHADING_PASS = 3
                };

                D3d9VertexShader optionalVertexShaders_[NUM_OF_OPTIONAL_VERTEX_SHADERS];
                D3d9VertexShader vertexShaders_[NUM_OF_VERTEX_SHADERS];

                D3d9PixelShader optionalPixelShaders_[NUM_OF_OPTIONAL_PIXEL_SHADERS];
                D3d9PixelShader pixelShaders_[NUM_OF_PIXEL_SHADERS];

                LPDIRECT3DVERTEXDECLARATION9 d3dMeshVertexDeclaration_;
                LPDIRECT3DDEVICE9 d3dDevice_;

                D3d9RenderTargetContainer* renderTargetContainer_;
                D3d9FrameParameters* frameParameters_;
                D3d9TextureHandler* textureHandler_;
                D3d9Capabilities* capabilities_;

                /**
                 * \brief Sets material.
                 * \param[in] material material, which will be set
                 * \param[in] pass rendering pass
                 */
                void setMaterial(const Material& material, uint8_t pass);

                /**
                 * \brief Sets skeleton pose.
                 * \param[in] boneTransforms bone transforms in world space
                 */
                void setSkeletonPose(const Array<Skeleton::Transform, uint16_t>& boneTransforms);

                /**
                 * \brief Renders actors from given node.
                 * \param[in] actorNode actor node
                 * \param[in] pass rendering pass
                 */
                void renderActors(Renderer::Data::ActorNode& actorNode, uint8_t pass);

                /**
                 * \brief Renders meshes from given node.
                 * \param[in] meshNode mesh node
                 * \param[in] meshRenderingUnit mesh rendering unit
                 * \param[in] pass rendering pass
                 */
                void renderMeshes(Renderer::Data::MeshNode& meshNode,
                                  uint8_t meshRenderingUnit,
                                  uint8_t pass);

                /**
                 * \brief Renders instances of the given mesh subset.
                 * \param[in] renderingList rendering list, which contains view-projection
                 * transforms of the given mesh subset
                 * \param[in] meshSubset subset of the mesh
                 * \param[in] meshRenderingUnit mesh rendering unit
                 * \param[in] pass rendering pass
                 */
                void renderMeshSubsetInstances(const Renderer::Data::List<Actor::Instance>& renderingList,
                                               const Mesh::Subset& meshSubset,
                                               uint8_t meshRenderingUnit,
                                               uint8_t pass);

        };

        /**
         * @}
         */

}

#endif
