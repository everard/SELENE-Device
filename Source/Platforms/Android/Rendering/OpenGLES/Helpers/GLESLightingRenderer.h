// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GLES_LIGHTING_RENDERER_H
#define GLES_LIGHTING_RENDERER_H

#include "../../../../../Engine/Rendering/Renderer.h"
#include "../../../../../Engine/Scene/Nodes/Light.h"
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
        class GlesActorsRenderer;
        class GlesTextureHandler;

        /**
         * Represents lighting renderer. Renders lights and shadows.
         */
        class GlesLightingRenderer
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

                GlesLightingRenderer();
                GlesLightingRenderer(const GlesLightingRenderer&) = delete;
                ~GlesLightingRenderer();
                GlesLightingRenderer& operator =(const GlesLightingRenderer&) = delete;

                /**
                 * \brief Initializes lighting renderer.
                 * \param[in] renderTargetContainer render target container
                 * \param[in] frameParameters frame parameters
                 * \param[in] actorsRenderer actors renderer
                 * \param[in] textureHandler texture handler
                 * \return true if lighting renderer has been successfully initialized
                 */
                bool initialize(GlesRenderTargetContainer& renderTargetContainer,
                                GlesFrameParameters& frameParameters,
                                GlesActorsRenderer& actorsRenderer,
                                GlesTextureHandler& textureHandler);

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
                        GLSL_PROGRAM_DIRECTIONAL_LIGHT_ACCUMULATION = 0,
                        GLSL_PROGRAM_POINT_LIGHT_ACCUMULATION,
                        GLSL_PROGRAM_SPOT_LIGHT_ACCUMULATION,
                        GLSL_PROGRAM_SPOT_LIGHT_ACCUMULATION_WITH_SHADOWS,
                        GLSL_PROGRAM_SPOT_LIGHT_SHADOWS_PASS,
                        NUM_OF_GLSL_PROGRAMS,

                        LOCATION_ATTRIBUTE_POSITION = 0
                };

                /**
                 * Represents container of the GLSL program's locations of variables.
                 */
                class Variables
                {
                public:
                        GLint locationViewProjectionMatrix;
                        GLint locationNormalsMatrix;
                        GLint locationViewMatrix;

                        GLint locationLightTextureMatrix;
                        GLint locationLightViewMatrix;

                        GLint locationLightColors;
                        GLint locationLightPositions;
                        GLint locationLightDirections;

                        GLint locationShadowMapConversionParameters;
                        GLint locationTextureCoordinatesAdjustment;
                        GLint locationConversionParameters;
                        GLint locationUnprojectionVector;
                        GLint locationBias;

                        GLint locationDepthBuffer;
                        GLint locationNormalsBuffer;
                        GLint locationShadowsBuffer;
                        GLint locationShadowMap;

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

                GLuint vertexBuffers_[NUM_OF_LIGHT_TYPES];

                GlesRenderTargetContainer* renderTargetContainer_;
                GlesFrameParameters* frameParameters_;
                GlesActorsRenderer* actorsRenderer_;
                GlesTextureHandler* textureHandler_;

                /**
                 * \brief Renders lighting without shadows.
                 * \param[in] lightNode light node
                 * \param[in] lightUnit light unit, from which rendering will start
                 */
                void renderLightingWithoutShadows(Renderer::Data::LightNode& lightNode,
                                                  uint8_t lightUnit);

                /**
                 * \brief Prepares light accumulation.
                 * \param[in] variables container of the GLSL program's locations of variables
                 */
                void prepareLightAccumulation(const GlesLightingRenderer::Variables& variables);

                /**
                 * \brief Prepares shadows pass.
                 * \param[in] variables container of the GLSL program's locations of variables
                 */
                void prepareShadowsPass(const GlesLightingRenderer::Variables& variables);

                /**
                 * \brief Renders light geometry.
                 * \param[in] variables container of the GLSL program's locations of variables
                 * \param[in] type type of the light
                 * \param[in] numLights number of lights
                 * \param[in] positions light positions
                 * \param[in] directions light directions
                 * \param[in] colors light colors
                 */
                void renderLightGeometry(const GlesLightingRenderer::Variables& variables,
                                         uint8_t type, uint32_t numLights,
                                         Vector4d* colors,
                                         Vector4d* positions,
                                         Vector4d* directions);

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
