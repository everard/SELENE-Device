// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GLES_ACTORS_RENDERER_H
#define GLES_ACTORS_RENDERER_H

#include "../../../../../Engine/Core/Resources/Mesh/Mesh.h"
#include "../../../../../Engine/Rendering/Renderer.h"
#include "GLESGLSLProgram.h"

namespace selene
{

        /**
         * \addtogroup Android
         * @{
         */

        // Forward declaration of classes
        class GlesRenderTargetContainer;
        class GlesFrameParameters;
        class GlesTextureHandler;

        /**
         * Represents actors renderer. Renders positions map, normals map, shadow map and shading.
         */
        class GlesActorsRenderer
        {
        public:
                GlesActorsRenderer();
                GlesActorsRenderer(const GlesActorsRenderer&) = delete;
                ~GlesActorsRenderer();
                GlesActorsRenderer& operator =(const GlesActorsRenderer&) = delete;

                /**
                 * \brief Initializes actors renderer.
                 * \param[in] renderTargetContainer render target container
                 * \param[in] frameParameters frame parameters
                 * \param[in] textureHandler texture handler
                 * \return true if actors renderer has been successfully initialized
                 */
                bool initialize(GlesRenderTargetContainer& renderTargetContainer,
                                GlesFrameParameters& frameParameters,
                                GlesTextureHandler& textureHandler);

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
                 * \brief Renders shadow map from given actor node.
                 * \param[in] actorNode actor node
                 */
                void renderShadowMap(Renderer::Data::ActorNode& actorNode);

                /**
                 * \brief Renders shading from given actor node.
                 * \param[in] actorNode actor node
                 */
                void renderShading(Renderer::Data::ActorNode& actorNode);

        private:
                /// Helper constants
                enum
                {
                        RENDERING_PASS_NORMALS = 0,
                        RENDERING_PASS_SHADING,
                        RENDERING_PASS_SHADOWS,
                        NUM_OF_RENDERING_PASSES,

                        GLSL_PROGRAM_NORMALS_PASS = 0,
                        GLSL_PROGRAM_SKIN_NORMALS_PASS,
                        GLSL_PROGRAM_SHADING_PASS,
                        GLSL_PROGRAM_SKIN_SHADING_PASS,
                        GLSL_PROGRAM_SHADOWS_PASS,
                        GLSL_PROGRAM_SKIN_SHADOWS_PASS,
                        NUM_OF_GLSL_PROGRAMS,

                        MAX_NUM_OF_VERTEX_ATTRIBUTES_PER_PASS = 4,
                        MAX_NUM_OF_BONES_IN_MODEL = 50,

                        LOCATION_ATTRIBUTE_POSITION = 0,
                        LOCATION_ATTRIBUTE_NORMAL,
                        LOCATION_ATTRIBUTE_TANGENT,
                        LOCATION_ATTRIBUTE_TEXTURE_COORDINATES,
                        LOCATION_ATTRIBUTE_BONE_INDICES,
                        LOCATION_ATTRIBUTE_BONE_WEIGHTS
                };

                /**
                 * Represents container of the GLSL program's locations of variables.
                 */
                class Variables
                {
                public:
                        GLint locationWorldViewProjectionMatrix;
                        GLint locationNormalsMatrix;
                        GLint locationBoneRotations;
                        GLint locationBonePositions;

                        GLint locationTextureCoordinatesAdjustment;
                        GLint locationSpecularParameters;

                        GLint locationAmbientMap;
                        GLint locationDiffuseMap;
                        GLint locationSpecularMap;
                        GLint locationNormalMap;

                        GLint locationLightBuffer;

                        GLint locationAmbientColor;
                        GLint locationDiffuseColor;
                        GLint locationSpecularColor;

                        Variables();
                        Variables(const Variables&) = default;
                        ~Variables();
                        Variables& operator =(const Variables&) = default;

                        /**
                         * \brief Obtains locations of variables.
                         * \param[in] program GLSL program, which is source of the locations
                         */
                        void obtainLocations(GlesGlslProgram& program);

                };

                GlesGlslProgram programs_[NUM_OF_GLSL_PROGRAMS];
                Variables variables_[NUM_OF_GLSL_PROGRAMS];

                GlesRenderTargetContainer* renderTargetContainer_;
                GlesFrameParameters* frameParameters_;
                GlesTextureHandler* textureHandler_;

                /**
                 * \brief Sets material.
                 * \param[in] material material, which will be set
                 * \param[in] pass rendering pass
                 * \param[in] variables container of the variables' locations
                 */
                void setMaterial(const Material& material, uint8_t pass,
                                 const GlesActorsRenderer::Variables& variables);

                /**
                 * \brief Sets skeleton pose.
                 * \param[in] boneTransforms bone transforms in world space
                 * \param[in] variables container of the variables' locations
                 */
                void setSkeletonPose(const Array<Skeleton::Transform, uint16_t>& boneTransforms,
                                     const GlesActorsRenderer::Variables& variables);

                /**
                 * \brief Renders actors from given node.
                 * \param[in] actorNode actor node
                 * \param[in] pass rendering pass
                 */
                void renderActors(Renderer::Data::ActorNode& actorNode, uint8_t pass);

                /**
                 * \brief Renders meshes from given node.
                 * \param[in] variables container of the variables' locations
                 * \param[in] meshNode mesh node
                 * \param[in] meshRenderingUnit mesh rendering unit
                 * \param[in] pass rendering pass
                 */
                void renderMeshes(const GlesActorsRenderer::Variables& variables,
                                  Renderer::Data::MeshNode& meshNode,
                                  uint8_t meshRenderingUnit,
                                  uint8_t pass);

                /**
                 * \brief Renders instances of the given mesh subset.
                 * \param[in] renderingList rendering list, which contains view-projection
                 * transforms of the given mesh subset
                 * \param[in] variables container of the variables' locations
                 * \param[in] meshSubset subset of the mesh
                 * \param[in] meshRenderingUnit mesh rendering unit
                 * \param[in] pass rendering pass
                 */
                void renderMeshSubsetInstances(const Renderer::Data::List<Actor::Instance>& renderingList,
                                               const GlesActorsRenderer::Variables& variables,
                                               const Mesh::Subset& meshSubset,
                                               uint8_t meshRenderingUnit,
                                               uint8_t pass);

        };

        /**
         * @}
         */

}

#endif
