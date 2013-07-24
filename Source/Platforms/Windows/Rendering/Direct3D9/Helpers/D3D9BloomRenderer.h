// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef D3D9_BLOOM_RENDERER_H
#define D3D9_BLOOM_RENDERER_H

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
        class D3d9FullScreenQuad;
        class D3d9TextureHandler;
        class D3d9Capabilities;

        /**
         * Represents bloom renderer.
         */
        class D3d9BloomRenderer
        {
        public:
                D3d9BloomRenderer();
                D3d9BloomRenderer(const D3d9BloomRenderer&) = delete;
                ~D3d9BloomRenderer();
                D3d9BloomRenderer& operator =(const D3d9BloomRenderer&) = delete;

                /**
                 * \brief Initializes bloom renderer.
                 * \param[in] renderTargetContainer render target container
                 * \param[in] frameParameters frame parameters
                 * \param[in] fullScreenQuad full screen quad
                 * \param[in] textureHandler texture handler
                 * \param[in] capabilities D3D capabilities
                 * \return true if bloom renderer has been successfully initialized
                 */
                bool initialize(D3d9RenderTargetContainer& renderTargetContainer,
                                D3d9FrameParameters& frameParameters,
                                D3d9FullScreenQuad& fullScreenQuad,
                                D3d9TextureHandler& textureHandler,
                                D3d9Capabilities& capabilities);

                /**
                 * \brief Destroys bloom renderer.
                 */
                void destroy();

                /**
                 * \brief Renders bloom.
                 */
                void renderBloom();

        private:
                /// Helper constants
                enum
                {
                        VERTEX_SHADER_BRIGHT_PASS = 0,
                        VERTEX_SHADER_BLOOM_PASS,
                        VERTEX_SHADER_COMBINE_PASS,
                        NUM_OF_VERTEX_SHADERS,

                        PIXEL_SHADER_BRIGHT_PASS = 0,
                        PIXEL_SHADER_BLOOM_PASS,
                        PIXEL_SHADER_COMBINE_PASS,
                        NUM_OF_PIXEL_SHADERS,

                        NUM_OF_BLUR_OFFSETS = 9,

                        LOCATION_IMAGE_SCALE = 0,
                        LOCATION_TEXTURE_COORDINATES_ADJUSTMENT = 0,
                        LOCATION_BLOOM_PARAMETERS = 1,
                        LOCATION_BLUR_OFFSETS = 2,

                        LOCATION_INPUT_IMAGE_BRIGHT_PASS  = 0,
                        LOCATION_INPUT_IMAGE_BLOOM_PASS   = 0,
                        LOCATION_BLOOM_IMAGE_COMBINE_PASS = 0,
                        LOCATION_ORIGINAL_IMAGE_COMBINE_PASS = 1
                };

                D3d9VertexShader vertexShaders_[NUM_OF_VERTEX_SHADERS];
                D3d9PixelShader  pixelShaders_[NUM_OF_PIXEL_SHADERS];

                LPDIRECT3DDEVICE9 d3dDevice_;

                D3d9RenderTargetContainer* renderTargetContainer_;
                D3d9FrameParameters* frameParameters_;
                D3d9FullScreenQuad* fullScreenQuad_;
                D3d9TextureHandler* textureHandler_;
                D3d9Capabilities* capabilities_;

        };

        /**
         * @}
         */

}

#endif
