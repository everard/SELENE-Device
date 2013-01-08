// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef D3D9_TEXTURE_H
#define D3D9_TEXTURE_H

#include "../../../../../Engine/Core/Resources/Texture/Texture.h"
#include "../D3D9Device.h"

namespace selene
{

        // Represents D3D9 texture.
        class D3d9Texture: public Texture
        {
        public:
                D3d9Texture(const char* name = nullptr);
                ~D3d9Texture();

                // Retains D3D9 texture
                bool retain();

                // Discards D3D9 texture
                void discard();

        private:
                friend class D3d9Renderer;

                LPDIRECT3DTEXTURE9 d3dTexture_;

        };

}

#endif
