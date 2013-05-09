// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLESRenderTarget.h"
#include "../GLESRenderer.h"

#include "../../../Platform.h"

namespace selene
{

        GlesRenderTarget::GlesRenderTarget()
        {
                renderableTexture_ = 0;
        }
        GlesRenderTarget::~GlesRenderTarget()
        {
                destroy();
        }

        //----------------------------------------------------------------------------------------------------
        bool GlesRenderTarget::initialize(uint32_t width, uint32_t height)
        {
                destroy();

                // create renderable textures
                glGenTextures(1, &renderableTexture_);
                CHECK_GLES_ERROR("GlesRenderTarget::initialize: glGenTextures");

                glBindTexture(GL_TEXTURE_2D, renderableTexture_);
                CHECK_GLES_ERROR("GlesRenderTarget::initialize: glBindTexture");

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                CHECK_GLES_ERROR("GlesRenderTarget::initialize: glTexImage2D");

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                CHECK_GLES_ERROR("GlesRenderTarget::initialize: glTexParameteri");

                glBindTexture(GL_TEXTURE_2D, 0);
                CHECK_GLES_ERROR("GlesRenderTarget::initialize: glBindTexture");

                if(renderableTexture_ == 0)
                        return false;

                return true;
        }

        //----------------------------------------------------------------------------------------------------
        void GlesRenderTarget::destroy()
        {
                if(renderableTexture_ != 0)
                {
                        glDeleteTextures(1, &renderableTexture_);
                        renderableTexture_ = 0;
                }
        }

        //----------------------------------------------------------------------------------------------------
        GLuint GlesRenderTarget::getRenderableTexture() const
        {
                return renderableTexture_;
        }

}
