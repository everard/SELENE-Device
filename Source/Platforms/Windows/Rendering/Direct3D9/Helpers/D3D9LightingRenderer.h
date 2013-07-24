// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef D3D9_LIGHTING_RENDERER_H
#define D3D9_LIGHTING_RENDERER_H

#include "../../../../../Engine/Rendering/Renderer.h"
#include "../../../../../Engine/Scene/Nodes/Light.h"
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
        class D3d9ActorsRenderer;
        class D3d9TextureHandler;
        class D3d9Capabilities;

        /**
         * Represents lighting renderer. Renders lights and shadows.
         */
        class D3d9LightingRenderer
        {
        public:
                /// Light types and batch size
                enum
                {
                        LIGHT_DIRECTIONAL = 0,
                        LIGHT_POINT,
                        LIGHT_SPOT,
                        NUM_OF_LIGHT_TYPES,

                        BATCH_SIZE = 30
                };

                D3d9LightingRenderer();
                D3d9LightingRenderer(const D3d9LightingRenderer&) = delete;
                ~D3d9LightingRenderer();
                D3d9LightingRenderer& operator =(const D3d9LightingRenderer&) = delete;

                /**
                 * \brief Initializes lighting renderer.
                 * \param[in] renderTargetContainer render target container
                 * \param[in] frameParameters frame parameters
                 * \param[in] actorsRenderer actors renderer
                 * \param[in] textureHandler texture handler
                 * \param[in] capabilities D3D capabilities
                 * \return true if lighting renderer has been successfully initialized
                 */
                bool initialize(D3d9RenderTargetContainer& renderTargetContainer,
                                D3d9FrameParameters& frameParameters,
                                D3d9ActorsRenderer& actorsRenderer,
                                D3d9TextureHandler& textureHandler,
                                D3d9Capabilities& capabilities);

                /**
                 * \brief Destroys lighting renderer.
                 */
                void destroy();

                /**
                 * \brief Renders lighting.
                 * \param[in] lightNode light node
                 */
                void renderLighting(Renderer::Data::LightNode& lightNode);

        private:
                /// Helper constants
                enum
                {
                        VERTEX_SHADER_DIRECTIONAL_LIGHT_ACCUMULATION = 0,
                        VERTEX_SHADER_POINT_LIGHT_ACCUMULATION,
                        VERTEX_SHADER_SPOT_LIGHT_ACCUMULATION,
                        VERTEX_SHADER_SPOT_LIGHT_SHADOW_PASS,
                        NUM_OF_VERTEX_SHADERS,

                        PIXEL_SHADER_DIRECTIONAL_LIGHT_ACCUMULATION = 0,
                        PIXEL_SHADER_POINT_LIGHT_ACCUMULATION,
                        PIXEL_SHADER_SPOT_LIGHT_ACCUMULATION,
                        PIXEL_SHADER_SPOT_LIGHT_ACCUMULATION_WITH_SHADOWS,
                        PIXEL_SHADER_SPOT_LIGHT_SHADOW_PASS,
                        NUM_OF_PIXEL_SHADERS,

                        LOCATION_DIRECTIONAL_LIGHT_DIRECTION = 12,
                        LOCATION_DIRECTIONAL_LIGHT_COLOR = LOCATION_DIRECTIONAL_LIGHT_DIRECTION + BATCH_SIZE,

                        LOCATION_POINT_LIGHT_POSITION = 12,
                        LOCATION_POINT_LIGHT_COLOR = LOCATION_POINT_LIGHT_POSITION + BATCH_SIZE,

                        LOCATION_SPOT_LIGHT_POSITION  = 12,
                        LOCATION_SPOT_LIGHT_DIRECTION = LOCATION_SPOT_LIGHT_POSITION + BATCH_SIZE,
                        LOCATION_SPOT_LIGHT_COLOR = LOCATION_SPOT_LIGHT_DIRECTION + BATCH_SIZE,

                        LOCATION_VIEW_PROJECTION_MATRIX = 0,
                        LOCATION_NORMALS_MATRIX = 4,
                        LOCATION_VIEW_MATRIX = 8,

                        LOCATION_LIGHT_POSITION = 12,
                        LOCATION_LIGHT_DIRECTION = 13,

                        LOCATION_TEXTURE_COORDINATES_ADJUSTMENT = 0,
                        LOCATION_UNPROJECTION_VECTOR = 1,
                        LOCATION_PROJECTION_PARAMETERS = 2,

                        LOCATION_LIGHT_PROJECTION_PARAMETERS = 3,
                        LOCATION_LIGHT_VIEW_MATRIX = 4,
                        LOCATION_LIGHT_TEXTURE_MATRIX = 8,
                        LOCATION_SHADOW_MAP_BIAS = 12,
                        LOCATION_SHADOW_MAP_KERNEL_SIZE = 13,

                        LOCATION_POSITIONS_MAP = 0,
                        LOCATION_NORMALS_MAP = 1,
                        LOCATION_SHADOW_MAP = 1
                };

                D3d9VertexShader vertexShaders_[NUM_OF_VERTEX_SHADERS];
                D3d9PixelShader pixelShaders_[NUM_OF_PIXEL_SHADERS];

                LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffers_[NUM_OF_LIGHT_TYPES];
                LPDIRECT3DVERTEXDECLARATION9 d3dVertexDeclaration_;
                LPDIRECT3DDEVICE9 d3dDevice_;

                D3d9RenderTargetContainer* renderTargetContainer_;
                D3d9FrameParameters* frameParameters_;
                D3d9ActorsRenderer* actorsRenderer_;
                D3d9TextureHandler* textureHandler_;
                D3d9Capabilities* capabilities_;

                /**
                 * \brief Renders lighting without shadows.
                 * \param[in] lightNode light node
                 * \param[in] lightUnit light unit, from which rendering will start
                 */
                void renderLightingWithoutShadows(Renderer::Data::LightNode& lightNode,
                                                  uint8_t lightUnit);

                /**
                 * \brief Prepares light accumulation.
                 */
                void prepareLightAccumulation();

                /**
                 * \brief Renders light geometry.
                 * \param[in] type type of the light
                 * \param[in] numLights number of lights
                 * \param[in] positions light positions
                 * \param[in] directions light directions
                 * \param[in] colors light colors
                 */
                void renderLightGeometry(uint8_t type, uint32_t numLights,
                                         Vector4d* positions,
                                         Vector4d* directions,
                                         Vector4d* colors);

                /**
                 * \brief Renders shadow map.
                 * \param[in] actorNode actor node, which contains shadow casters
                 * \param[in] spotLight spot light
                 */
                void renderShadowMap(Renderer::Data::ActorNode& actorNode, const SpotLight& spotLight);

        };

        /**
         * @}
         */

}

#endif
