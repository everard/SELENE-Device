// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GLES_RENDER_TARGET_CONTAINER_H
#define GLES_RENDER_TARGET_CONTAINER_H

#include "../../../../../Engine/Rendering/Renderer.h"
#include "GLESRenderTarget.h"
#include "GLESGLSLProgram.h"

namespace selene
{

        /**
         * \addtogroup Android
         * @{
         */

        // Forward declaration of classes
        class GlesFrameParameters;
        class GlesCapabilities;

        /// Render target types
        enum RENDER_TARGET
        {
                RENDER_TARGET_POSITIONS = 0,
                RENDER_TARGET_NORMALS,
                RENDER_TARGET_LIGHT_BUFFER,
                RENDER_TARGET_HELPER_0,
                RENDER_TARGET_HELPER_1,
                RENDER_TARGET_RESULT,
                NUM_OF_RENDER_TARGETS
        };

        /**
         * Represents render target container.
         */
        class GlesRenderTargetContainer
        {
        public:
                GlesRenderTargetContainer();
                ~GlesRenderTargetContainer();

                /**
                 * \brief Initializes render target container.
                 * \param[in] frameParameters frame parameters
                 * \param[in] parameters rendering parameters
                 * \param[in] capabilities OpenGL ES 2.0 capabilities
                 * \return true if render target container has been successfully initialized
                 */
                bool initialize(GlesFrameParameters& frameParameters,
                                Renderer::Parameters& parameters,
                                GlesCapabilities& capabilities);

                /**
                 * \brief Destroys render target container.
                 */
                void destroy();

                /**
                 * \brief Returns render target.
                 * \param[in] type type of the render target
                 * \return reference to the render target
                 */
                const GlesRenderTarget& getRenderTarget(uint8_t type) const;

                /**
                 * \brief Sets back buffer.
                 */
                void setBackBuffer() const;

                /**
                 * \brief Returns shadow map.
                 * \return reference to the shadow map
                 */
                //const GlesRenderTarget& getShadowMap() const;

        private:
                GlesRenderTarget renderTargets_[NUM_OF_RENDER_TARGETS];
                GlesRenderTarget dummyRenderTarget_;

                GLuint depthRenderbuffer_;

        };

        /**
         * @}
         */

}

#endif
