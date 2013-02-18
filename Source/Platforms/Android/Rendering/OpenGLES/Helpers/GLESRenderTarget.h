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
                 * \param[in] depthRenderBuffer ID of the depth renderbuffer (if set to zero, then
                 * new depth renderbuffer is created)
                 * \return true if render target has been successfully initialized
                 */
                bool initialize(uint32_t width, uint32_t height, GLuint depthRenderBuffer = 0);

                /**
                 * \brief Destroys render target.
                 */
                void destroy();

                /**
                 * \brief Sets render target.
                 */
                void set() const;

                /**
                 * \brief Creates renderbuffer with given dimentions and format.
                 * \param[in] width width of the render target
                 * \param[in] height height of the render target
                 * \param[in] format format of the renderbuffer
                 * \return renderbuffer ID
                 */
                static GLuint createRenderbuffer(uint32_t width, uint32_t height, GLenum format);

        private:
                friend class GlesTextureHandler;

                GLuint renderableTexture_;
                GLuint framebufferObject_;
                GLuint depthRenderBuffer_;

                bool isDepthRenderbufferShared_;

        };

        /**
         * @}
         */

}

#endif
