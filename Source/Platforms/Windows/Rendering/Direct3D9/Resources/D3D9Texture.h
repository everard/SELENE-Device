// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef D3D9_TEXTURE_H
#define D3D9_TEXTURE_H

#include "../../../../../Engine/Core/Resources/Texture/Texture.h"
#include <d3d9.h>

namespace selene
{

        /**
         * \addtogroup Windows
         * @{
         */

        /**
         * Represents D3D9 texture.
         */
        class D3d9Texture: public Texture
        {
        public:
                /**
                 * \brief Constructs D3D9 texture with given name.
                 * \param[in] name name of the D3D9 texture
                 */
                D3d9Texture(const char* name = nullptr);
                D3d9Texture(const D3d9Texture&) = delete;
                ~D3d9Texture();
                D3d9Texture& operator =(const D3d9Texture&) = delete;

                // Texture interface implementation
                bool retain();
                void discard();

        private:
                friend class D3d9TextureHandler;

                LPDIRECT3DTEXTURE9 d3dTexture_;

        };

        /**
         * @}
         */

}

#endif
