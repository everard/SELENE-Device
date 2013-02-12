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
                destroy();
        }

        //-------------------------
        bool GlesTexture::retain()
        {
                destroy();

                LOGI("****************************** Retaining Texture '%s'", getName());

                // validate
                if(data_.pixels.isEmpty())
                        return false;

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
                                return false;
                }

                // generate texture
                glGenTextures(1, &texture_);
                if(texture_ == 0)
                        return false;

                CHECK_GLES_ERROR("glGenTextures");

                glActiveTexture(GL_TEXTURE0);
                CHECK_GLES_ERROR("glActiveTexture");

                glBindTexture(GL_TEXTURE_2D, texture_);
                CHECK_GLES_ERROR("glBindTexture");

                glTexImage2D(GL_TEXTURE_2D, 0, format, data_.width, data_.height, 0,
                             format, GL_UNSIGNED_BYTE, &data_.pixels[0]);
                CHECK_GLES_ERROR("glTexImage2D");

                glGenerateMipmap(GL_TEXTURE_2D);
                CHECK_GLES_ERROR("glGenerateMipmap");

                glBindTexture(GL_TEXTURE_2D, 0);
                CHECK_GLES_ERROR("glBindTexture");

                LOGI("****************************** Texture '%s' has been successfully retained", getName());

                return true;
        }

        //-------------------------
        void GlesTexture::discard()
        {
                destroy();
        }

        //-------------------------
        void GlesTexture::destroy()
        {
                if(texture_ != 0)
                {
                        glDeleteTextures(1, &texture_);
                        texture_ = 0;
                }
        }

}
