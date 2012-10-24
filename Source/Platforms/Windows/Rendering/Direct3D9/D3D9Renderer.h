// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef D3D9_RENDERER_H
#define D3D9_RENDERER_H

#include "../../../../Engine/Rendering/Renderer.h"
#include "../../../../Engine/Core/Resources/Mesh/Mesh.h"
#include "../../../../Engine/Core/Status/Status.h"

#include "Helpers/D3D9ParticlesRenderer.h"
#include "Helpers/D3D9FullScreenQuad.h"
#include "Helpers/D3D9LightsGeometry.h"
#include "Helpers/D3D9SSAOGeometry.h"
#include "Helpers/D3D9GUIRenderer.h"

#include "D3D9Device.h"
#include "D3D9Shader.h"

namespace selene
{

        // Forward declaration of classes
        class D3d9StaticMesh;
        class D3d9SkinMesh;
        class Material;

        enum
        {
                // Rendering flags
                RENDERING_SSAO_ENABLED  = 0x01,
                RENDERING_BLOOM_ENABLED = 0x02,
                RENDERING_FULL_SCREEN_ENABLED = 0x04
        };

        // Represents D3D9 renderer.
        class D3d9Renderer: public Renderer, public Status
        {
        public:
                D3d9Renderer();
                ~D3d9Renderer();

                // Initializes renderer
                bool initialize(const Renderer::Parameters& parameters);

                // Destroys renderer
                void destroy();

                // Renders scene
                void render(const Camera& camera);

        private:
                enum
                {
                        // Vertex shaders
                        VERTEX_SHADER_POSITIONS_PASS = 0,
                        VERTEX_SHADER_SKIN_POSITIONS_PASS,
                        VERTEX_SHADER_NORMALS_PASS,
                        VERTEX_SHADER_SKIN_NORMALS_PASS,
                        VERTEX_SHADER_DIRECTIONAL_LIGHT_ACCUMULATION,
                        VERTEX_SHADER_POINT_LIGHT_ACCUMULATION,
                        VERTEX_SHADER_SPOT_LIGHT_ACCUMULATION,
                        VERTEX_SHADER_SPOT_LIGHT_SHADOW_PASS,
                        VERTEX_SHADER_SHADING_PASS,
                        VERTEX_SHADER_SKIN_SHADING_PASS,
                        VERTEX_SHADER_RESULT_PASS,
                        VERTEX_SHADER_SSAO_PASS,
                        VERTEX_SHADER_SSAO_BLUR_X_PASS,
                        VERTEX_SHADER_SSAO_BLUR_Y_PASS,
                        VERTEX_SHADER_EDGE_DETECTION,
                        VERTEX_SHADER_GUI_FRAMES_PASS,
                        VERTEX_SHADER_GUI_TEXT_PASS,
                        VERTEX_SHADER_GUI_CURSOR_PASS,
                        VERTEX_SHADER_BRIGHT_PASS,
                        VERTEX_SHADER_BLOOM_PASS,
                        VERTEX_SHADER_COMBINE_PASS,
                        VERTEX_SHADER_PARTICLES_PASS,
                        NUM_OF_VERTEX_SHADERS,

                        // Pixel shaders
                        PIXEL_SHADER_POSITIONS_PASS = 0,
                        PIXEL_SHADER_NORMALS_PASS,
                        PIXEL_SHADER_DIRECTIONAL_LIGHT_ACCUMULATION,
                        PIXEL_SHADER_POINT_LIGHT_ACCUMULATION,
                        PIXEL_SHADER_SPOT_LIGHT_ACCUMULATION,
                        PIXEL_SHADER_SPOT_LIGHT_ACCUMULATION_WITH_SHADOWS,
                        PIXEL_SHADER_SPOT_LIGHT_SHADOW_PASS,
                        PIXEL_SHADER_SHADING_PASS,
                        PIXEL_SHADER_SHADING_PASS_WITH_SSAO,
                        PIXEL_SHADER_RESULT_PASS,
                        PIXEL_SHADER_SSAO_PASS,
                        PIXEL_SHADER_SSAO_BLUR_X_PASS,
                        PIXEL_SHADER_SSAO_BLUR_Y_PASS,
                        PIXEL_SHADER_EDGE_DETECTION,
                        PIXEL_SHADER_GUI_FRAMES_PASS,
                        PIXEL_SHADER_GUI_TEXT_PASS,
                        PIXEL_SHADER_GUI_CURSOR_PASS,
                        PIXEL_SHADER_BRIGHT_PASS,
                        PIXEL_SHADER_BLOOM_X_PASS,
                        PIXEL_SHADER_BLOOM_Y_PASS,
                        PIXEL_SHADER_COMBINE_PASS,
                        PIXEL_SHADER_PARTICLES_PASS,
                        NUM_OF_PIXEL_SHADERS,

                        // Optional vertex shaders
                        OPTIONAL_VERTEX_SHADER_POSITIONS_AND_NORMALS_PASS = 0,
                        OPTIONAL_VERTEX_SHADER_SKIN_POSITIONS_AND_NORMALS_PASS,
                        NUM_OF_OPTIONAL_VERTEX_SHADERS,

                        // Optional pixel shaders
                        OPTIONAL_PIXEL_SHADER_POSITIONS_AND_NORMALS_PASS = 0,
                        NUM_OF_OPTIONAL_PIXEL_SHADERS,

                        // Dummy textures
                        DUMMY_TEXTURE_WHITE = 0,
                        DUMMY_TEXTURE_NORMAL_MAP,
                        NUM_OF_DUMMY_TEXTURES,

                        // Render targets
                        RENDER_TARGET_POSITIONS = 0,
                        RENDER_TARGET_NORMALS,
                        RENDER_TARGET_LIGHTS,
                        RENDER_TARGET_SSAO,
                        RENDER_TARGET_BLURRED_SSAO,
                        NUM_OF_RENDER_TARGETS,

                        // Half-size render targets
                        HALF_SIZE_RENDER_TARGET_BLOOM = 0,
                        HALF_SIZE_RENDER_TARGET_BLURRED_BLOOM,
                        NUM_OF_HALF_SIZE_RENDER_TARGETS,

                        // Rendering passes
                        RENDERING_PASS_POSITIONS = 0,
                        RENDERING_PASS_NORMALS,
                        RENDERING_PASS_SHADING,
                        RENDERING_PASS_POSITIONS_AND_NORMALS
                };

                LPDIRECT3DDEVICE9 d3dDevice_;

                D3d9VertexShader optionalVertexShaders_[NUM_OF_OPTIONAL_VERTEX_SHADERS];
                D3d9PixelShader  optionalPixelShaders_[NUM_OF_OPTIONAL_PIXEL_SHADERS];
                D3d9VertexShader vertexShaders_[NUM_OF_VERTEX_SHADERS];
                D3d9PixelShader  pixelShaders_[NUM_OF_PIXEL_SHADERS];
                D3d9ParticlesRenderer particlesRenderer_;
                D3d9FullScreenQuad fullScreenQuad_;
                D3d9LightsGeometry lightsGeometry_;
                D3d9SsaoGeometry ssaoGeometry_;
                D3d9GuiRenderer guiRenderer_;

                // Helper textures
                LPDIRECT3DTEXTURE9 d3dDummyTextures_[NUM_OF_DUMMY_TEXTURES];
                LPDIRECT3DTEXTURE9 d3dRandomTexture_;

                // Render targets
                LPDIRECT3DTEXTURE9 d3dRenderTargetTextures_[NUM_OF_RENDER_TARGETS];
                LPDIRECT3DSURFACE9 d3dRenderTargetSurfaces_[NUM_OF_RENDER_TARGETS];
                LPDIRECT3DTEXTURE9 d3dHalfSizeRenderTargetTextures_[NUM_OF_HALF_SIZE_RENDER_TARGETS];
                LPDIRECT3DSURFACE9 d3dHalfSizeRenderTargetSurfaces_[NUM_OF_HALF_SIZE_RENDER_TARGETS];
                LPDIRECT3DSURFACE9 d3dBackBufferSurface_, d3dDepthStencilSurface_;
                LPDIRECT3DSURFACE9 d3dShadowMapRenderTargetSurface_;
                LPDIRECT3DSURFACE9 d3dShadowMapDepthStencilSurface_;
                LPDIRECT3DTEXTURE9 d3dShadowMapTexture_;

                // Matrices and vectors
                Matrix viewProjectionMatrix_;
                Matrix projectionMatrix_;
                Matrix viewInvMatrix_;
                Matrix normalsMatrix_;
                Matrix viewMatrix_;

                Vector4d textureCoordinatesAdjustment_;
                Vector4d projectionParameters_;
                Vector4d shadowMapKernelSize_;
                Vector4d unprojectionVector_;
                Vector4d screenSize_;

                // Parameters
                Parameters parameters_;
                DWORD d3dMaxTextureAnisotropy_;
                bool isR32fRenderTargetFormatSupported_;
                bool isMultipleRenderTargetSupported_;
                bool isThirdShaderModelSupported_;

                // Writes log entry
                void writeLogEntry(const char* entry);

                // Sets texture
                void setTexture(const Resource::Instance<Texture>& texture, DWORD pass,
                                uint8_t dummyTextureIndex);

                // Sets texture
                void setTexture(Texture* texture, DWORD pass, uint8_t dummyTextureIndex);

                // Sets skeleton pose
                void setSkeletonPose(const Array<Skeleton::Transform, uint16_t>& boneTransforms);

                // Sets up material for the normals rendering pass
                void setupNormalsPass(const Material& material);

                // Sets up material for the shading pass
                void setupShadingPass(const Material& material);

                // Sets up state for the light accumulation pass
                void setupLightAccumulationPass();

                // Renders actors
                void renderActors(const Mesh::Subset& meshSubset,
                                  const std::vector<Actor*>& actors,
                                  uint8_t meshRenderingUnit,
                                  uint8_t pass);

                // Renders shadows
                void renderShadows(const Mesh::Subset& meshSubset,
                                   const std::vector<Actor*>& actors,
                                   const SpotLight& spotLight,
                                   uint8_t meshRenderingUnit);

                // Creates shadow map
                void createShadowMap(ActorsList& actorsList, const SpotLight& spotLight);

                // Renders lights
                void renderLights(uint8_t lightsListType = LIGHTS_LIST_WITHOUT_SHADOWS);

                // Renders positions and normals
                void renderPositionsAndNormals();

                // Accumulates lighting
                void accumulateLighting();

                // Blurs SSAO
                void blurSsao(const Vector4d& edgeDetectionParameters);

                // Renders SSAO
                void renderSsao();

                // Renders shading
                void shade();

                // Renders particles
                void renderParticles();

                // Blurs bloom
                void blurBloom(const Vector4d& kernelSize);

                // Renders bloom
                void renderBloom();

        };

}

#endif
