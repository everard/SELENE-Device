// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef D3D9_RENDERER_H
#define D3D9_RENDERER_H

#include "../../../../Engine/Rendering/Renderer.h"
#include "../../../../Engine/Core/Resources/Mesh/Mesh.h"
#include "../../../../Engine/Core/Status/Status.h"

#include "Helpers/D3D9RenderTargetContainer.h"
#include "Helpers/D3D9ParticlesRenderer.h"
#include "Helpers/D3D9LightingRenderer.h"
#include "Helpers/D3D9ActorsRenderer.h"
#include "Helpers/D3D9FullScreenQuad.h"
#include "Helpers/D3D9TextureHandler.h"
#include "Helpers/D3D9SSAOGeometry.h"
#include "Helpers/D3D9GUIRenderer.h"
#include "Helpers/D3D9Shader.h"

namespace selene
{

        // Forward declaration of classes
        class D3d9StaticMesh;
        class D3d9SkinMesh;
        class Material;

        // Represents D3D9 renderer.
        class D3d9Renderer: public Renderer, public Status
        {
        public:
                // Initializes renderer
                bool initialize(const Renderer::Parameters& parameters);

                // Destroys renderer
                void destroy();

                // Renders scene
                void render(const Camera& camera);

                // Returns device
                static LPDIRECT3DDEVICE9 getDevice();

        private:
                friend class WindowsApplication;

                enum
                {
                        VERTEX_SHADER_RESULT_PASS = 0,
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

                        PIXEL_SHADER_RESULT_PASS = 0,
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

                        OPTIONAL_VERTEX_SHADER_SSAO_PASS = 0,
                        NUM_OF_OPTIONAL_VERTEX_SHADERS,

                        OPTIONAL_PIXEL_SHADER_SSAO_PASS = 0,
                        NUM_OF_OPTIONAL_PIXEL_SHADERS
                };

                static LPDIRECT3DDEVICE9 d3dDevice_;
                LPDIRECT3D9 d3d_;

                D3d9VertexShader optionalVertexShaders_[NUM_OF_OPTIONAL_VERTEX_SHADERS];
                D3d9PixelShader  optionalPixelShaders_[NUM_OF_OPTIONAL_PIXEL_SHADERS];
                D3d9VertexShader vertexShaders_[NUM_OF_VERTEX_SHADERS];
                D3d9PixelShader  pixelShaders_[NUM_OF_PIXEL_SHADERS];

                D3d9RenderTargetContainer renderTargetContainer_;
                D3d9ParticlesRenderer particlesRenderer_;
                D3d9LightingRenderer lightingRenderer_;
                D3d9ActorsRenderer actorsRenderer_;
                D3d9TextureHandler textureHandler_;
                D3d9FullScreenQuad fullScreenQuad_;
                D3d9SsaoGeometry ssaoGeometry_;
                D3d9GuiRenderer guiRenderer_;

                LPDIRECT3DTEXTURE9 d3dRandomTexture_;

                D3DPRESENT_PARAMETERS d3dPresentParameters_;
                D3d9FrameParameters frameParameters_;
                Parameters parameters_;

                D3d9Capabilities capabilities_;
                bool isDeviceLost_;

                D3d9Renderer();
                D3d9Renderer(const D3d9Renderer& renderer);
                ~D3d9Renderer();
                D3d9Renderer& operator =(const D3d9Renderer&);

                // Initializes helpers
                bool initializeHelpers();

                // Destroys helpers
                void destroyHelpers();

                // Writes log entry
                void writeLogEntry(const char* entry);

                /*// Blurs SSAO
                void blurSsao(const Vector4d& edgeDetectionParameters, bool shouldUpscale = false);

                // Renders SSAO
                void renderSsao();

                // Renders particles
                void renderParticles(Renderer::Data::ParticleSystemNode& particleSystemNode);

                // Blurs bloom
                void blurBloom(const Vector4d& kernelSize);

                // Renders bloom
                void renderBloom();*/

        };

}

#endif
