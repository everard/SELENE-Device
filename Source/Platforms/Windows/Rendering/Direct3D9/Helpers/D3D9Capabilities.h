// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef D3D9_CAPABILITIES_H
#define D3D9_CAPABILITIES_H

#include "../../../../../Engine/Rendering/Renderer.h"
#include <d3d9.h>

namespace selene
{

        /**
         * \addtogroup Windows
         * @{
         */

        /**
         * Represents D3D9 capabilities.
         */
        class D3d9Capabilities
        {
        public:
                D3d9Capabilities();
                D3d9Capabilities(const D3d9Capabilities&) = delete;
                ~D3d9Capabilities();
                D3d9Capabilities& operator =(const D3d9Capabilities&) = delete;

                /**
                 * \brief Checks capabilities and creates device.
                 * \param[in] d3d D3D interface
                 * \param[in] parameters renderer parameters
                 * \param[out] d3dPresentParameters D3D present parameters
                 * \param[out] d3dDevice D3D device
                 * \return true if current hardware meets minimum requirements and device has
                 * been successfully created
                 */
                bool createCompatibleDevice(LPDIRECT3D9 d3d, const Renderer::Parameters& parameters,
                                            D3DPRESENT_PARAMETERS& d3dPresentParameters,
                                            LPDIRECT3DDEVICE9& d3dDevice);

                /**
                 * \brief Returns true if R32F render target format is supported.
                 * \return true if R32F render target format is supported
                 */
                bool isR32fRenderTargetFormatSupported() const;

                /**
                 * \brief Returns true if multiple render target is supported.
                 * \return true if multiple render target is supported
                 */
                bool isMultipleRenderTargetSupported() const;

                /**
                 * \brief Returns true if shader model 3.0 is supported.
                 * \return true if shader model 3.0 is supported
                 */
                bool isThirdShaderModelSupported() const;

                /**
                 * \brief Returns maximum texture anisotropy.
                 * \return maximum texture anisotropy
                 */
                DWORD getMaxTextureAnisotropy() const;

                /**
                 * \brief Returns maximum texture size.
                 * \return maximum texture size
                 */
                DWORD getMaxTextureSize() const;

        private:
                bool isR32fRenderTargetFormatSupported_;
                bool isMultipleRenderTargetSupported_;
                bool isThirdShaderModelSupported_;
                DWORD maxTextureAnisotropy_;
                DWORD maxTextureSize_;

        };

        /**
         * @}
         */

}

#endif
