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
                RENDER_TARGET_NORMALS = 0,
                RENDER_TARGET_LIGHT_BUFFER,
                RENDER_TARGET_HELPER_0,
                RENDER_TARGET_HELPER_1,
                RENDER_TARGET_RESULT,
                NUM_OF_RENDER_TARGETS
        };

        /// Depth buffer types
        enum
        {
                DEPTH_BUFFER_NORMALS_PASS = 0,
                DEPTH_BUFFER_SHADING_PASS = 1,
                DEPTH_BUFFER_SHADOW_MAP = DEPTH_BUFFER_SHADING_PASS,
                NUM_OF_DEPTH_BUFFERS
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
                 * \return true if render target container has been successfully initialized
                 */
                bool initialize(GlesFrameParameters& frameParameters,
                                Renderer::Parameters& parameters);

                /**
                 * \brief Destroys render target container.
                 */
                void destroy();

                /**
                 * \brief Sets render target.
                 * \param[in] type type of the render target
                 * \return true if render target has been successfully set
                 */
                bool setRenderTarget(uint8_t type) const;

                /**
                 * \brief Returns render target.
                 * \param[in] type type of the render target
                 * \return const reference to the render target
                 */
                const GlesRenderTarget& getRenderTarget(uint8_t type) const;

                /**
                 * \brief Sets back buffer.
                 */
                void setBackBuffer() const;

                /**
                 * \brief Returns depth buffer
                 * \param[in] type type of the depth buffer
                 * \return depth buffer
                 */
                GLuint getDepthBuffer(uint8_t type) const;

                /**
                 * \brief Creates depth buffer with given dimensions.
                 * \param[in] width width of the buffer
                 * \param[in] height height of the buffer
                 * \return renderable texture
                 */
                static GLuint createDepthBuffer(uint32_t width, uint32_t height);

                /**
                 * \brief Sets shadow map.
                 * \return true if shadow map has been successfully set
                 */
                //const GlesRenderTarget& getShadowMap() const;

        private:
                GlesRenderTarget renderTargets_[NUM_OF_RENDER_TARGETS];
                GlesRenderTarget dummyRenderTarget_;

                GLuint frameBuffer_;
                GLuint depthBuffers_[NUM_OF_DEPTH_BUFFERS];

                mutable bool isFrameBufferBound_;

                /**
                 * \brief Returns true if framebuffer is complete.
                 * \return true if framebuffer is complete
                 */
                bool isFramebufferComplete() const;

        };

        /**
         * @}
         */

}

#endif
