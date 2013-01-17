// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GLES_RENDERER_H
#define GLES_RENDERER_H

#include "../../../../Engine/Rendering/Renderer.h"
#include "../../../../Engine/Core/Resources/Mesh/Mesh.h"
#include "../../../../Engine/Core/Status/Status.h"
#include "Helpers/GLESGUIRenderer.h"

#include <GLES2/gl2.h>
#include <EGL/egl.h>

namespace selene
{

        // Represents OpenGL ES renderer.
        class GlesRenderer: public Renderer, public Status
        {
        public:
                // Initializes renderer
                bool initialize(const Renderer::Parameters& parameters);

                // Destroys renderer
                void destroy();

                // Renders scene
                void render(const Camera& camera);

                // Checks OpenGL ES error
                static void checkGlesError(const char* operation);

        private:
                friend class AndroidApplication;

                enum
                {
                        MAX_NUM_OF_BONES = 50,
                        MAX_NUM_MATRICES = 3,

                        // Capabilities
                        NUM_OF_VERTEX_SHADER_UNIFORMS_REQUIRED = (MAX_NUM_MATRICES * 4 + 2 * MAX_NUM_OF_BONES),
                        NUM_OF_VERTEX_ATTRIBUTES_REQUIRED = 6,

                        // Rendering passes
                        RENDERING_PASS_POSITIONS = 0,
                        RENDERING_PASS_NORMALS,
                        RENDERING_PASS_SHADING,
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

                Parameters parameters_;

                EGLSurface surface_;
                EGLContext context_;
                EGLDisplay display_;

                GlesTexture dummyTexture_;
                GlesGlslProgram programs_[2];
                GlesGuiRenderer guiRenderer_;

                GlesRenderer();
                GlesRenderer(const GlesRenderer& renderer);
                ~GlesRenderer();

                // Writes log entry
                void writeLogEntry(const char* entry);

                // Sets texture
                void setTexture(const Resource::Instance<Texture>& texture, uint8_t sampler,
                                uint8_t dummyTextureIndex);

                // Sets texture
                void setTexture(Texture* texture, uint8_t sampler, uint8_t dummyTextureIndex);

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
                                  const std::vector<Renderer::Data::Instance>& instances,
                                  GLint worldViewProjectionMatrixLocation,
                                  GLint worldViewMatrixLocation,
                                  GLint normalsMatrixLocation,
                                  GLint bonePositionsLocation,
                                  GLint boneRotationsLocation,
                                  uint8_t meshRenderingUnit,
                                  uint8_t pass);

                // Initializes context and retains all resources
                bool retain();

                // Discards context and all resources
                void discard();

                // Retains all helpers
                bool retainHelpers();

                // Discards all helpers
                void discardHelpers();

                // Initializes helpers
                bool initializeHelpers();

                // Destroys helpers
                void destroyHelpers();

                // Initializes OpenGL ES context
                bool initializeGlesContext();

                // Destroys OpenGL ES context
                void destroyGlesContext();

        };

}

#endif
