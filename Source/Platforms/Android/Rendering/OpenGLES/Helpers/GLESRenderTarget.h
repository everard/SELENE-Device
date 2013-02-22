// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GLES_RENDER_TARGET_H
#define GLES_RENDER_TARGET_H

#include "../../../../../Engine/Core/Macros/Macros.h"

#include <GLES2/gl2.h>

namespace selene
{

        /**
         * \addtogroup Android
         * @{
         */

        /**
         * Represents OpenGL ES render target.
         */
        class GlesRenderTarget
        {
        public:
                GlesRenderTarget();
                ~GlesRenderTarget();

                /**
                 * \brief Initializes render target.
                 * \param[in] width width of the render target
                 * \param[in] height height of the render target
                 * \param[in] depthRenderbuffer depth renderbuffer
                 * \param[in] depthTexture depth texture
                 * \return true if render target has been successfully initialized
                 */
                bool initialize(uint32_t width, uint32_t height);

                /**
                 * \brief Destroys render target.
                 */
                void destroy();

                /**
                 * \brief Returns renderable texture.
                 * \return renderable texture
                 */
                GLuint getRenderableTexture() const;

        private:
                GLuint renderableTexture_;

        };

        /**
         * @}
         */

}

#endif
