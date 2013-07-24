// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef D3D9_RENDER_TARGET_CONTAINER_H
#define D3D9_RENDER_TARGET_CONTAINER_H

#include "../../../../../Engine/Rendering/Renderer.h"
#include "D3D9RenderTarget.h"
#include "D3D9Shader.h"

namespace selene
{

        /**
         * \addtogroup Windows
         * @{
         */

        // Forward declaration of classes
        class D3d9FrameParameters;
        class D3d9Capabilities;

        /// Render target types
        enum RENDER_TARGET
        {
                RENDER_TARGET_POSITIONS = 0,
                RENDER_TARGET_NORMALS,
                RENDER_TARGET_LIGHT_BUFFER,
                RENDER_TARGET_HELPER_0,
                RENDER_TARGET_HELPER_1,
                RENDER_TARGET_RESULT,
                RENDER_TARGET_HALF_SIZE_HELPER,
                NUM_OF_RENDER_TARGETS
        };

        /**
         * Represents render target container.
         */
        class D3d9RenderTargetContainer
        {
        public:
                D3d9RenderTargetContainer();
                D3d9RenderTargetContainer(const D3d9RenderTargetContainer&) = delete;
                ~D3d9RenderTargetContainer();
                D3d9RenderTargetContainer& operator =(const D3d9RenderTargetContainer&) = delete;

                /**
                 * \brief Initializes render target container.
                 * \param[in] frameParameters frame parameters
                 * \param[in] parameters rendering parameters
                 * \param[in] capabilities D3D capabilities
                 * \return true if render target container has been successfully initialized
                 */
                bool initialize(D3d9FrameParameters& frameParameters,
                                Renderer::Parameters& parameters,
                                D3d9Capabilities& capabilities);

                /**
                 * \brief Destroys render target container.
                 */
                void destroy();

                /**
                 * \brief Returns render target.
                 * \param[in] type type of the render target
                 * \return reference to the render target
                 */
                const D3d9RenderTarget& getRenderTarget(uint8_t type) const;

                /**
                 * \brief Returns back buffer.
                 * \return reference to the back buffer
                 */
                const D3d9RenderTarget& getBackBuffer() const;

                /**
                 * \brief Returns shadow map.
                 * \return reference to the shadow map
                 */
                const D3d9RenderTarget& getShadowMap() const;

        private:
                D3d9RenderTarget renderTargets_[NUM_OF_RENDER_TARGETS];
                D3d9RenderTarget dummyRenderTarget_;
                D3d9RenderTarget backBuffer_;
                D3d9RenderTarget shadowMap_;

        };

        /**
         * @}
         */

}

#endif
