// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef D3D9_SSAO_RENDERER_H
#define D3D9_SSAO_RENDERER_H

#include "../../../../../Engine/Core/Macros/Macros.h"
#include "D3D9Shader.h"

namespace selene
{

        /**
         * \addtogroup Windows
         * @{
         */

        // Forward declaration of classes
        class D3d9RenderTargetContainer;
        class D3d9FullScreenQuad;
        class D3d9TextureHandler;

        /**
         * Represents D3D9 SSAO renderer.
         */
        class D3d9SsaoRenderer
        {
        public:
                /**
                 * \brief Initializes SSAO renderer.
                 * \return true if SSAO renderer has been successfully initialized
                 */
                bool initialize();

                /**
                 * \brief Destroys SSAO renderer.
                 */
                void destroy();

                /**
                 * \brief Renders SSAO.
                 */
                void renderSsao();

        private:
                friend class D3d9Renderer;

                /// Helper constants
                enum
                {
                        VERTEX_SHADER_SSAO_PASS = 0,
                        VERTEX_SHADER_SSAO_BLUR_X_PASS,
                        VERTEX_SHADER_SSAO_BLUR_Y_PASS,
                        NUM_OF_VERTEX_SHADERS,

                        PIXEL_SHADER_SSAO_PASS = 0,
                        PIXEL_SHADER_SSAO_BLUR_X_PASS,
                        PIXEL_SHADER_SSAO_BLUR_Y_PASS,
                        NUM_OF_PIXEL_SHADERS,

                        OPTIONAL_VERTEX_SHADER_SSAO_PASS = 0,
                        NUM_OF_OPTIONAL_VERTEX_SHADERS,

                        OPTIONAL_PIXEL_SHADER_SSAO_PASS = 0,
                        NUM_OF_OPTIONAL_PIXEL_SHADERS,

                        LOCATION_UNPROJECTION_VECTOR = 0,
                        LOCATION_PROJECTION_PARAMETERS = 1,
                        LOCATION_EDGE_DETECTION_PARAMETERS = 2,
                        LOCATION_SSAO_PARAMETERS = 2,
                        LOCATION_SCREEN_SIZE_SSAO_PASS = 0,
                        LOCATION_SCREEN_SIZE_BLUR_PASS = 3,
                        LOCATION_TEXTURE_COORDINATES_ADJUSTMENT = 4
                };

                D3d9VertexShader optionalVertexShaders_[NUM_OF_OPTIONAL_VERTEX_SHADERS];
                D3d9VertexShader vertexShaders_[NUM_OF_VERTEX_SHADERS];

                D3d9PixelShader optionalPixelShaders_[NUM_OF_OPTIONAL_PIXEL_SHADERS];
                D3d9PixelShader pixelShaders_[NUM_OF_PIXEL_SHADERS];

                LPDIRECT3DVERTEXDECLARATION9 d3dVertexDeclaration_;
                LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffer_;
                LPDIRECT3DTEXTURE9 d3dRandomTexture_;
                LPDIRECT3DDEVICE9 d3dDevice_;

                const D3d9RenderTargetContainer& renderTargetContainer_;
                const D3d9FrameParameters& frameParameters_;
                const D3d9Capabilities& capabilities_;
                D3d9FullScreenQuad& fullScreenQuad_;
                D3d9TextureHandler& textureHandler_;

                D3d9SsaoRenderer(const D3d9RenderTargetContainer& renderTargetContainer,
                                 const D3d9FrameParameters& frameParameters,
                                 const D3d9Capabilities& capabilities,
                                 D3d9FullScreenQuad& fullScreenQuad,
                                 D3d9TextureHandler& textureHandler);
                D3d9SsaoRenderer(const D3d9SsaoRenderer& ssaoRenderer);
                ~D3d9SsaoRenderer();
                D3d9SsaoRenderer& operator =(const D3d9SsaoRenderer&);

                /**
                 * \brief Blurs SSAO.
                 * \param[in] edgeDetectionParameters edge detection parameters
                 * \param[in] shouldUpscale flag, which shows the need to upscale SSAO buffer
                 */
                void blurSsao(const Vector4d& edgeDetectionParameters,
                              bool shouldUpscale = false);

        };

        /**
         * @}
         */

}

#endif