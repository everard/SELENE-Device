// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

/*#ifndef D3D9_RENDER_TARGET_H
#define D3D9_RENDER_TARGET_H

#include "../../../../../Engine/Core/Macros/Macros.h"
#include <d3d9.h>

namespace selene
{*/

        /**
         * \addtogroup Windows
         * @{
         */

        /**
         * Represents D3D9 render target.
         */
        /*class D3d9RenderTarget
        {
        public:
                D3d9RenderTarget();
                ~D3d9RenderTarget();*/

                /**
                 * \brief Initializes render target.
                 * \param[in] width width of the render target
                 * \param[in] height height of the render target
                 * \param[in] format format of the render target
                 * \param[in] depthStencilSurfaceDescriptor descriptor of the depth-stencil surface
                 * (if set to nullptr, the depth-stencil surface will not be created)
                 * \return true if render target has been successfully initialized
                 */
                /*bool initialize(uint32_t width, uint32_t height, D3DFORMAT format,
                                D3DSURFACE_DESC* depthStencilSurfaceDescriptor = nullptr);*/

                /**
                 * \brief Initializes render target.
                 * \param[in] d3dTexture texture, which holds result of the rendering
                 * \param[in] d3dSurface rendering surface
                 * \param[in] d3dDepthStencilSurface depth-stencil surface
                 */
                /*void initialize(LPDIRECT3DTEXTURE9 d3dTexture, LPDIRECT3DSURFACE9 d3dSurface,
                                LPDIRECT3DSURFACE9 d3dDepthStencilSurface);*/

                /**
                 * \brief Destroys render target.
                 */
                //void destroy();

                /**
                 * \brief Returns D3D9 texture of the render target.
                 * \return D3D9 texture
                 */
                //LPDIRECT3DTEXTURE9 getTexture() const;

                /**
                 * \brief Returns D3D9 rendering surface.
                 * \return D3D9 surface
                 */
                //LPDIRECT3DSURFACE9 getSurface() const;

                /**
                 * \brief Returns D3D9 depth-stencil surface.
                 * \return D3D9 depth-stencil surface
                 */
                //LPDIRECT3DSURFACE9 getDepthStencilSurface() const;

        /*private:
                LPDIRECT3DTEXTURE9 d3dTexture_;
                LPDIRECT3DSURFACE9 d3dSurface_;
                LPDIRECT3DSURFACE9 d3dDepthStencilSurface_;

        };*/

        /**
         * @}
         */

/*}

#endif*/
