// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GLES_BLOOM_RENDERER_H
#define GLES_BLOOM_RENDERER_H

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
        class GlesFullScreenQuad;
        class GlesTextureHandler;

        /**
         * Represents bloom renderer.
         */
        class GlesBloomRenderer
        {
        public:
                GlesBloomRenderer();
                ~GlesBloomRenderer();

                /**
                 * \brief Initializes bloom renderer.
                 * \param[in] renderTargetContainer render target container
                 * \param[in] frameParameters frame parameters
                 * \param[in] fullScreenQuad full screen quad
                 * \param[in] textureHandler texture handler
                 * \return true if bloom renderer has been successfully initialized
                 */
                bool initialize(GlesRenderTargetContainer& renderTargetContainer,
                                GlesFrameParameters& frameParameters,
                                GlesFullScreenQuad& fullScreenQuad,
                                GlesTextureHandler& textureHandler);

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
                        GLSL_PROGRAM_BRIGHT_PASS = 0,
                        GLSL_PROGRAM_BLOOM_PASS,
                        GLSL_PROGRAM_COMBINE_PASS,
                        NUM_OF_GLSL_PROGRAMS,

                        NUM_OF_BLUR_OFFSETS = 9,

                        LOCATION_ATTRIBUTE_POSITION = 0
                };

                /**
                 * Represents container of the GLSL program's locations of variables.
                 */
                class Variables
                {
                public:
                        GLint locationInputImage;
                        GLint locationCombinedImage;

                        GLint locationTextureCoordinatesAdjustment;
                        GLint locationBloomParameters;
                        GLint locationBlurOffsets;
                        GLint locationImageScale;

                        Variables();
                        ~Variables();

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
                GlesFullScreenQuad* fullScreenQuad_;
                GlesTextureHandler* textureHandler_;

        };

        /**
         * @}
         */

}

#endif
