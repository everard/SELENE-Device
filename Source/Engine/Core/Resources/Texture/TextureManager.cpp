// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "TextureManager.h"

namespace selene
{

        TextureManager::TextureManager():
                totalSize_(0), compressionFactor_(0), ddsHeader_(), isDxt_(false) {}
        TextureManager::~TextureManager() {}

        //-----------------------------------------------------------------------------------------------------
        bool TextureManager::readTexture(std::istream& stream, Texture::Data& textureData)
        {
                // read header
                if(!readHeader(stream, textureData))
                        return false;

                // get texture properties
                if(!getProperties(textureData))
                        return false;

                // compute texture total size
                if(!computeTotalSize(textureData))
                        return false;

                // read pixels
                return readPixels(stream, textureData);
        }

        //-----------------------------------------------------------------------------------------------------
        bool TextureManager::readHeader(std::istream& stream, Texture::Data& textureData)
        {
                // read header
                char magic[4];
                stream.read(magic, sizeof(magic));

                // validate header
                if(strncmp(magic, "DDS ", 4) != 0)
                        return false;

                // read DDS header
                stream.read(reinterpret_cast<char*>(&ddsHeader_), sizeof(DdsHeader));

                // check common flags
                if((!IS_SET(ddsHeader_.caps.caps1, DDSCAPS_TEXTURE) && !IS_SET(ddsHeader_.flags, DDSD_CAPS)) ||
                   !IS_SET(ddsHeader_.flags, DDSD_PIXELFORMAT) ||
                   !IS_SET(ddsHeader_.flags, DDSD_WIDTH) ||
                   !IS_SET(ddsHeader_.flags, DDSD_HEIGHT))
                        return false;

                // get texture format
                isDxt_ = false;
                if(IS_SET(ddsHeader_.pixelFormat.flags, DDPF_FOURCC))
                {
                        switch(ddsHeader_.pixelFormat.fourCc)
                        {
                                case FOURCC_DXT1:
                                        textureData.format = TEXTURE_FORMAT_DXT1;
                                        break;

                                case FOURCC_DXT3:
                                        textureData.format = TEXTURE_FORMAT_DXT3;
                                        break;

                                case FOURCC_DXT5:
                                        textureData.format = TEXTURE_FORMAT_DXT5;
                                        break;

                                default:
                                        return false;
                        }

                        // determine compression factor
                        compressionFactor_ = 4;
                        if(textureData.format == TEXTURE_FORMAT_DXT1)
                                compressionFactor_ = 2;

                        isDxt_ = true;
                }
                else
                        textureData.format = TEXTURE_FORMAT_NOT_COMPRESSED;

                // validate
                if((isDxt_ && !IS_SET(ddsHeader_.flags, DDSD_LINEARSIZE)) ||
                   (ddsHeader_.pitchOrLinearSize == 0))
                        return false;

                return true;
        }

        //-----------------------------------------------------------------------------------------------------
        bool TextureManager::getProperties(Texture::Data& textureData)
        {
                // get number of mip maps
                textureData.numMipMaps = ddsHeader_.mipMapCount == 0 ? 1 : ddsHeader_.mipMapCount;

                // check flags and caps for mip maps
                if(!IS_SET(ddsHeader_.caps.caps1, DDSCAPS_COMPLEX) &&
                   !IS_SET(ddsHeader_.caps.caps1, DDSCAPS_MIPMAP) &&
                   !IS_SET(ddsHeader_.flags, DDSD_MIPMAPCOUNT) &&
                   ddsHeader_.mipMapCount > 1)
                        return false;

                // get width and height
                textureData.width  = ddsHeader_.width;
                textureData.height = ddsHeader_.height;

                // get BPP
                textureData.bpp = 0;
                if(!isDxt_)
                        textureData.bpp = static_cast<uint8_t>(ddsHeader_.pixelFormat.rgbBitCount >> 3);

                // validate
                if((textureData.width == 0 || textureData.height == 0) ||
                   (!isDxt_ && (textureData.bpp < 3)))
                        return false;

                return true;
        }

        //-----------------------------------------------------------------------------------------------------
        bool TextureManager::computeTotalSize(Texture::Data& textureData)
        {
                if(isDxt_)
                {
                        // compute DXT texture total size
                        if(textureData.numMipMaps > 1)
                                totalSize_ = ddsHeader_.pitchOrLinearSize * compressionFactor_;
                        else
                                totalSize_ = ddsHeader_.pitchOrLinearSize;
                }
                else
                {
                        // compute regular texture total size
                        uint32_t mipWidth  = textureData.width;
                        uint32_t mipHeight = textureData.height;

                        totalSize_ = 0;

                        for(register uint32_t i = 0; i < textureData.numMipMaps; ++i)
                        {
                                totalSize_ += mipWidth * mipHeight * textureData.bpp;

                                mipWidth  = mipWidth  >> 1;
                                mipHeight = mipHeight >> 1;

                                if(mipWidth == 0)
                                        mipWidth = 1;

                                if(mipHeight == 0)
                                        mipHeight = 1;
                        }
                }

                // allocate memory for texture
                if(!textureData.pixels.create(totalSize_))
                        return false;

                return true;
        }

        //-----------------------------------------------------------------------------------------------------
        bool TextureManager::readPixels(std::istream& stream, Texture::Data& textureData)
        {
                if(isDxt_)
                {
                        // read DXT texture data
                        stream.read(reinterpret_cast<char*>(&textureData.pixels[0]),
                                    textureData.pixels.getSize());
                }
                else
                {
                        // compute pitch
                        uint32_t pitch = textureData.width * textureData.bpp;
                        if((pitch & 0x03) != 0)
                                pitch += 4 - (pitch & 0x03);

                        Array<uint8_t, uint32_t> line;
                        if(!line.create(pitch))
                                return false;

                        // read regular texture data
                        uint32_t mipWidth  = textureData.width;
                        uint32_t mipHeight = textureData.height;

                        uint8_t* pixels = &textureData.pixels[0];

                        for(register uint32_t i = 0; i < textureData.numMipMaps; ++i)
                        {
                                uint32_t lineSize = mipWidth * textureData.bpp;

                                // validate
                                if(lineSize > pitch)
                                        return false;

                                // read mip map data
                                for(register uint32_t j = 0; j < mipHeight; ++j)
                                {
                                        stream.read(reinterpret_cast<char*>(&line[0]), pitch);
                                        memcpy(pixels, &line[0], lineSize);

                                        pixels += lineSize;
                                }

                                // compute new width and height
                                mipWidth  = mipWidth  >> 1;
                                mipHeight = mipHeight >> 1;

                                // compute new pitch
                                pitch = pitch >> 1;
                                if((pitch & 0x03) != 0)
                                        pitch += 4 - (pitch & 0x03);
                        }
                }

                return true;
        }

}
