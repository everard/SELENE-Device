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

        /**
         * Represents render target container.
         */
        class D3d9RenderTargetContainer
        {
        public:
                enum
                {
                        RENDER_TARGET_POSITIONS = 0,
                        RENDER_TARGET_NORMALS,
                        RENDER_TARGET_LIGHT_BUFFER,
                        RENDER_TARGET_SSAO_BUFFER,
                        RENDER_TARGET_BLURRED_SSAO_BUFFER,
                        RENDER_TARGET_RESULT,
                        RENDER_TARGET_HALF_SIZE_SSAO_BUFFER,
                        RENDER_TARGET_HALF_SIZE_BLOOM,
                        RENDER_TARGET_HALF_SIZE_BLURRED_BLOOM,
                        NUM_OF_RENDER_TARGETS
                };

                /**
                 * \brief Initializes render target container.
                 * \param[in] parameters rendering parameters
                 * \param[in] capabilities capabilities
                 * \return true if render target container has been successfully initialized
                 */
                bool initialize();

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
                friend class D3d9Renderer;

                D3d9RenderTarget renderTargets_[NUM_OF_RENDER_TARGETS];
                D3d9RenderTarget dummyRenderTarget_;
                D3d9RenderTarget backBuffer_;
                D3d9RenderTarget shadowMap_;

                const Renderer::Parameters& parameters_;
                const D3d9Capabilities& capabilities_;
                D3d9FrameParameters& frameParameters_;

                D3d9RenderTargetContainer(const Renderer::Parameters& parameters,
                                          const D3d9Capabilities& capabilities,
                                          D3d9FrameParameters& frameParameters);
                D3d9RenderTargetContainer(const D3d9RenderTargetContainer& renderTargetContainer);
                ~D3d9RenderTargetContainer();
                D3d9RenderTargetContainer& operator =(const D3d9RenderTargetContainer&);

        };

        /**
         * @}
         */

}

#endif
