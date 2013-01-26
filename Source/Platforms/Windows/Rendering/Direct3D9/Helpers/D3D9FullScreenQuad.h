// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef D3D9_FULL_SCREEN_QUAD_H
#define D3D9_FULL_SCREEN_QUAD_H

#include "../../../../../Engine/Core/Macros/Macros.h"
#include "D3D9Shader.h"

namespace selene
{

        /**
         * \addtogroup Windows
         * @{
         */

        /**
         * Represents D3D9 full-screen quad.
         */
        class D3d9FullScreenQuad
        {
        public:
                D3d9FullScreenQuad();
                ~D3d9FullScreenQuad();

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
                LPDIRECT3DVERTEXDECLARATION9 d3dVertexDeclaration_;
                LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffer_;
                LPDIRECT3DDEVICE9 d3dDevice_;

        };

        /**
         * @}
         */

}

#endif
