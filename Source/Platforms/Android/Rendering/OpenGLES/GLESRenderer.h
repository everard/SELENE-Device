// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GLES_RENDERER_H
#define GLES_RENDERER_H

#include "../../../../Engine/Rendering/Renderer.h"
#include "../../../../Engine/Core/Resources/Mesh/Mesh.h"
#include "../../../../Engine/Core/Status/Status.h"

#include "Helpers/GLESFrameParameters.h"
#include "Helpers/GLESActorsRenderer.h"
#include "Helpers/GLESTextureHandler.h"
#include "Helpers/GLESCapabilities.h"
#include "Helpers/GLESGUIRenderer.h"

namespace selene
{

        /**
         * \addtogroup Android
         * @{
         */

        /**
         * Represents OpenGL ES renderer.
         */
        class GlesRenderer: public Renderer, public Status
        {
        public:
                // Renderer interface implementation
                bool initialize(const Renderer::Parameters& parameters);
                void destroy();
                void render(const Camera& camera);

        private:
                friend class AndroidApplication;

                /// Helper constants
                enum
                {
                        MAX_NUM_OF_BONES = 50,
                        MAX_NUM_MATRICES = 3,

                        NUM_OF_VERTEX_SHADER_UNIFORMS_REQUIRED = (MAX_NUM_MATRICES * 4 + 2 * MAX_NUM_OF_BONES),
                        NUM_OF_VERTEX_ATTRIBUTES_REQUIRED = 6,

                        RENDERING_PASS_POSITIONS = 0,
                        RENDERING_PASS_NORMALS,
                        RENDERING_PASS_SHADING
                };

                Matrix viewProjectionMatrix_;
                Matrix projectionMatrix_;
                Matrix viewInvMatrix_;
                Matrix normalsMatrix_;
                Matrix viewMatrix_;

                Vector4d projectionParameters_;
                Vector4d shadowMapKernelSize_;
                Vector4d unprojectionVector_;
                Vector4d screenSize_;

                GlesActorsRenderer actorsRenderer_;
                GlesTextureHandler textureHandler_;
                GlesGuiRenderer guiRenderer_;

                GlesFrameParameters frameParameters_;
                GlesCapabilities capabilities_;
                Parameters parameters_;

                GlesRenderer();
                GlesRenderer(const GlesRenderer& renderer);
                ~GlesRenderer();

                /**
                 * \brief Writes log entry.
                 * \param[in] entry string, which should be written to the log
                 */
                void writeLogEntry(const char* entry);

                // Sets skeleton pose
                void setSkeletonPose(const Array<Skeleton::Transform, uint16_t>& boneTransforms,
                                     GLint bonePositionsLocation, GLint boneRotationsLocation);

                // Renders actors
                void renderActors(Renderer::Data::ActorNode& actorNode,
                                  GlesGlslProgram* programs,
                                  uint8_t* vertexStreamIndices,
                                  uint8_t numVertexStreams,
                                  uint8_t pass);

                // Renders actors
                void renderActors(const Mesh::Subset& meshSubset,
                                  const Renderer::Data::List<Actor::Instance>& renderingList,
                                  GLint worldViewProjectionMatrixLocation,
                                  GLint worldViewMatrixLocation,
                                  GLint normalsMatrixLocation,
                                  GLint bonePositionsLocation,
                                  GLint boneRotationsLocation,
                                  uint8_t meshRenderingUnit,
                                  uint8_t pass);

                /**
                 * \brief Initializes context and retains all resources.
                 * \return true if context has been successfully initialized and
                 * all resources have been retained
                 */
                bool retain();

                /**
                 * \brief Discards context and all resources.
                 */
                void discard();

        };

        /**
         * @}
         */

}

#endif
