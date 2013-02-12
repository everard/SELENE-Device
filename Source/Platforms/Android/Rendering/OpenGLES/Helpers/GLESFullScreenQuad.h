// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GLES_FULL_SCREEN_QUAD_H
#define GLES_FULL_SCREEN_QUAD_H

#include "../../../../../Engine/Core/Macros/Macros.h"
#include "GLESGLSLProgram.h"

namespace selene
{

        /**
         * \addtogroup Android
         * @{
         */

        /**
         * Represents OpenGL ES full-screen quad.
         */
        class GlesFullScreenQuad
        {
        public:
                GlesFullScreenQuad();
                ~GlesFullScreenQuad();

                /**
                 * \brief Initializes full-screen quad.
                 * \return true if full-screen quad has been successfully initialized
                 */
                bool initialize();

                /**
                 * \brief Destroys full-screen quad.
                 */
                void destroy();

                /**
                 * \brief Renders full-screen quad.
                 */
                void render();

        private:
                GLuint vertexBuffer_;

        };

        /**
         * @}
         */

}

#endif
