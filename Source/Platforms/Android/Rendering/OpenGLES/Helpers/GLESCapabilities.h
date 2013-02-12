// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GLES_CAPABILITIES_H
#define GLES_CAPABILITIES_H

#include "../../../../../Engine/Rendering/Renderer.h"

#include <GLES2/gl2.h>
#include <EGL/egl.h>

namespace selene
{

        /**
         * \addtogroup Android
         * @{
         */

        /**
         * Represents OpenGL ES capabilities.
         */
        class GlesCapabilities
        {
        public:
                /// Helper constants
                enum
                {
                        MAX_NUM_OF_BONES = 50,
                        MAX_NUM_MATRICES = 3
                };

                GlesCapabilities();
                ~GlesCapabilities();

                /**
                 * \brief Checks capabilities and creates OpenGL ES context.
                 * \param[in] parameters renderer parameters
                 * \return true if current hardware meets minimum requirements and OpenGL ES context has
                 * been successfully created
                 */
                bool createCompatibleContext(const Renderer::Parameters& parameters);

                /**
                 * \brief Destroys context.
                 */
                void destroyContext();

                /**
                 * \brief Returns EGL surface.
                 * \return surface
                 */
                EGLSurface getSurface();

                /**
                 * \brief Returns OpenGL ES context.
                 * \return context
                 */
                EGLContext getContext();

                /**
                 * \brief Returns EGL display.
                 * \return display
                 */
                EGLDisplay getDisplay();

        private:
                EGLSurface surface_;
                EGLContext context_;
                EGLDisplay display_;

        };

        /**
         * @}
         */

}

#endif
