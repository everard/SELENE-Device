// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "D3D9Texture.h"
#include "../D3D9Renderer.h"

namespace selene
{

        D3d9Texture::D3d9Texture(const char* name): Texture(name), d3dTexture_(nullptr) {}
        D3d9Texture::~D3d9Texture()
        {
                SAFE_RELEASE(d3dTexture_);
        }

        //-------------------------------------------------------------------------------------------------
        bool D3d9Texture::retain()
        {
                LPDIRECT3DDEVICE9 d3dDevice = D3d9Renderer::getDevice();
                if(d3dDevice == nullptr)
                        return false;

                // destroy D3D9 texture data if any
                SAFE_RELEASE(d3dTexture_);

                // validate
                if(data_.pixels.isEmpty())
                        return false;

                // get texture format
                D3DFORMAT format = D3DFMT_R8G8B8;
                uint32_t blockSize = 16;
                switch(data_.format)
                {
                        // DXT1
                        case TEXTURE_FORMAT_DXT1:
                                format = D3DFMT_DXT1;
                                blockSize = 8;
                                break;

                        // DXT3
                        case TEXTURE_FORMAT_DXT3:
                                format = D3DFMT_DXT3;
                                break;

                        // DXT5
                        case TEXTURE_FORMAT_DXT5:
                                format = D3DFMT_DXT5;
                                break;

                        // not compressed
                        case TEXTURE_FORMAT_NOT_COMPRESSED:
                                if(data_.bpp < 3)
                                        return false;
                                else if(data_.bpp == 4)
                                        format = D3DFMT_A8R8G8B8;
                                break;

                        // unknown format
                        default:
                                return false;
                }

                // create texture
                if(FAILED(d3dDevice->CreateTexture(data_.width, data_.height, data_.numMipMaps, 0,
                                                   format, D3DPOOL_DEFAULT, &d3dTexture_, nullptr)))
                {
                        SAFE_RELEASE(d3dTexture_);
                        return false;
                }

                // create helper texture
                LPDIRECT3DTEXTURE9 d3dHelperTexture = nullptr;
                if(FAILED(d3dDevice->CreateTexture(data_.width, data_.height, data_.numMipMaps, 0,
                                                   format, D3DPOOL_SYSTEMMEM, &d3dHelperTexture, nullptr)))
                {
                        SAFE_RELEASE(d3dHelperTexture);
                        SAFE_RELEASE(d3dTexture_);
                        return false;
                }

                D3DLOCKED_RECT lockedRect;

                uint32_t width  = data_.width;
                uint32_t height = data_.height;
                uint32_t offset = 0;

                // fill texture
                for(register uint32_t i = 0; i < data_.numMipMaps; ++i)
                {
                        // lock mip map
                        if(FAILED(d3dHelperTexture->LockRect(i, &lockedRect, nullptr, 0)))
                        {
                                SAFE_RELEASE(d3dHelperTexture);
                                SAFE_RELEASE(d3dTexture_);
                                return false;
                        }

                        if(width == 0)
                                width = 1;

                        if(height == 0)
                                height = 1;

                        if(data_.format != TEXTURE_FORMAT_NOT_COMPRESSED)
                        {
                                // handle compressed texture
                                uint32_t size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
                                memcpy(lockedRect.pBits, &data_.pixels[offset], size);
                                offset += size;
                        }
                        else
                        {
                                // handle not compressed texture
                                uint32_t size   = width * data_.bpp;
                                uint8_t* pixels = (uint8_t*)lockedRect.pBits;

                                // validate
                                if(size > (uint32_t)lockedRect.Pitch)
                                {
                                        d3dHelperTexture->UnlockRect(i);
                                        SAFE_RELEASE(d3dHelperTexture);
                                        SAFE_RELEASE(d3dTexture_);
                                        return false;
                                }

                                // fill texture using pitch value
                                for(register uint32_t j = 0; j < height; ++j)
                                {
                                        memcpy(pixels, &data_.pixels[offset], size);
                                        pixels += lockedRect.Pitch;
                                        offset += size;
                                }
                        }

                        // compute width and height of the next mip map level
                        width  >>= 1;
                        height >>= 1;

                        d3dHelperTexture->UnlockRect(i);
                }

                // copy data from helper texture to original
                if(FAILED(d3dDevice->UpdateTexture(d3dHelperTexture, d3dTexture_)))
                {
                        SAFE_RELEASE(d3dHelperTexture);
                        SAFE_RELEASE(d3dTexture_);
                        return false;
                }

                SAFE_RELEASE(d3dHelperTexture);

                return true;
        }

        //-------------------------------------------------------------------------------------------------
        void D3d9Texture::discard()
        {
                SAFE_RELEASE(d3dTexture_);
        }

}
