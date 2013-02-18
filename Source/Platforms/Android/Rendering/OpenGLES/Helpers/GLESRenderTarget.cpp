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
                framebufferObject_ = 0;
                depthRenderBuffer_ = 0;
                isDepthRenderbufferShared_ = true;
        }
        GlesRenderTarget::~GlesRenderTarget()
        {
                destroy();
        }

        //------------------------------------------------------------------------------------------
        bool GlesRenderTarget::initialize(uint32_t width, uint32_t height, GLuint depthRenderBuffer)
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

                // create depth renderbuffer if needed
                if(depthRenderBuffer == 0)
                {
                        depthRenderBuffer_ = createRenderbuffer(width, height, GL_DEPTH_COMPONENT16);
                        isDepthRenderbufferShared_ = false;
                }
                else
                {
                        isDepthRenderbufferShared_ = true;
                        depthRenderBuffer_ = depthRenderBuffer;
                }

                // create framebuffer object
                glGenFramebuffers(1, &framebufferObject_);
                CHECK_GLES_ERROR("GlesRenderTarget::initialize: glGenFramebuffers");

                glBindFramebuffer(GL_FRAMEBUFFER, framebufferObject_);
                CHECK_GLES_ERROR("GlesRenderTarget::initialize: glBindFramebuffer");

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderableTexture_, 0);
                CHECK_GLES_ERROR("GlesRenderTarget::initialize: glFramebufferTexture2D");

                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer_);
                CHECK_GLES_ERROR("GlesRenderTarget::initialize: glFramebufferRenderbuffer");

                GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
                CHECK_GLES_ERROR("GlesRenderTarget::initialize: glCheckFramebufferStatus");

                glBindTexture(GL_TEXTURE_2D, 0);
                CHECK_GLES_ERROR("GlesRenderTarget::initialize: glBindTexture");

                glBindRenderbuffer(GL_RENDERBUFFER, 0);
                CHECK_GLES_ERROR("GlesRenderTarget::initialize: glBindRenderbuffer");

                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                CHECK_GLES_ERROR("GlesRenderTarget::initialize: glBindFramebuffer");

                if(status != GL_FRAMEBUFFER_COMPLETE)
                {
                        switch(status)
                        {
                                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                                        LOGI("****************************** FBO error: FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
                                        break;

                                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                                        LOGI("****************************** FBO error: FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
                                        break;

                                case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
                                        LOGI("****************************** FBO error: FRAMEBUFFER_INCOMPLETE_DIMENSIONS");
                                        break;

                                case GL_FRAMEBUFFER_UNSUPPORTED:
                                        LOGI("****************************** FBO error: FRAMEBUFFER_UNSUPPORTED");
                                        break;

                                default:
                                        LOGI("****************************** Unknown FBO error");
                        }

                        destroy();
                        return false;
                }

                return true;
        }

        //---------------------------------------------------------------------------------------------
        void GlesRenderTarget::destroy()
        {
                if(framebufferObject_ != 0)
                {
                        glDeleteFramebuffers(1, &framebufferObject_);
                        framebufferObject_ = 0;
                }

                if(renderableTexture_ != 0)
                {
                        glDeleteTextures(1, &renderableTexture_);
                        renderableTexture_ = 0;
                }

                if(!isDepthRenderbufferShared_)
                {
                        if(depthRenderBuffer_ != 0)
                        {
                                glDeleteRenderbuffers(1, &depthRenderBuffer_);
                                depthRenderBuffer_ = 0;
                        }
                }

                isDepthRenderbufferShared_ = true;
        }

        //---------------------------------------------------------------------------------------------
        void GlesRenderTarget::set() const
        {
                glBindFramebuffer(GL_FRAMEBUFFER, framebufferObject_);
                CHECK_GLES_ERROR("GlesRenderTarget::set: glBindFramebuffer");
        }

        //---------------------------------------------------------------------------------------------
        GLuint GlesRenderTarget::createRenderbuffer(uint32_t width, uint32_t height, GLenum format)
        {
                GLuint renderbuffer;

                glGenRenderbuffers(1, &renderbuffer);
                CHECK_GLES_ERROR("GlesRenderTarget::createRenderbuffer: glGenRenderbuffers");

                glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
                CHECK_GLES_ERROR("GlesRenderTarget::createRenderbuffer: glBindRenderbuffer");

                glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
                CHECK_GLES_ERROR("GlesRenderTarget::createRenderbuffer: glRenderbufferStorage");

                glBindRenderbuffer(GL_RENDERBUFFER, 0);
                CHECK_GLES_ERROR("GlesRenderTarget::createRenderbuffer: glBindRenderbuffer");

                return renderbuffer;
        }

}
