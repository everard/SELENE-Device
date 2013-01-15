// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLESTexture.h"

#include "../../../Platform.h"
#include "../GLESRenderer.h"

namespace selene
{

        GlesTexture::GlesTexture(const char* name): Texture(name)
        {
                texture_ = 0;
        }
        GlesTexture::~GlesTexture()
        {
                if(texture_ != 0)
                        glDeleteTextures(1, &texture_);
        }

        //-------------------------
        bool GlesTexture::retain()
        {
                // validate
                if(data_.pixels.isEmpty())
                {
                        LOGI("--------------------- No texture data");
                        return false;
                }

                // get texture format
                GLint format = GL_RGB;
                switch(data_.format)
                {
                        // not compressed
                        case TEXTURE_FORMAT_NOT_COMPRESSED:
                                if(data_.bpp < 3)
                                        return false;
                                else if(data_.bpp == 4)
                                        format = GL_RGBA;
                                break;

                        // unknown format
                        default:
                                LOGI("--------------------- Texture format is not supported");
                                return false;
                }

                // generate texture
                glGenTextures(1, &texture_);
                if(texture_ == 0)
                        return false;

                GlesRenderer::checkGlesError("glGenTextures");

                glActiveTexture(GL_TEXTURE0);
                GlesRenderer::checkGlesError("glActiveTexture");

                glBindTexture(GL_TEXTURE_2D, texture_);
                GlesRenderer::checkGlesError("glBindTexture");

                glTexImage2D(GL_TEXTURE_2D, 0, format, data_.width, data_.height, 0,
                             format, GL_UNSIGNED_BYTE, &data_.pixels[0]);
                GlesRenderer::checkGlesError("glTexImage2D");

                glGenerateMipmap(GL_TEXTURE_2D);
                GlesRenderer::checkGlesError("glGenerateMipmap");

                glBindTexture(GL_TEXTURE_2D, 0);
                GlesRenderer::checkGlesError("glBindTexture");

                return true;
        }

        //-------------------------
        void GlesTexture::discard()
        {
                if(texture_ != 0)
                {
                        glDeleteTextures(1, &texture_);
                        texture_ = 0;
                }
        }

}
