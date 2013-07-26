// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLESRenderTargetContainer.h"
#include "../GLESRenderer.h"

#include "../../../../../Engine/Core/Helpers/Utility.h"
#include "../../../Platform.h"

namespace selene
{

        GlesRenderTargetContainer::GlesRenderTargetContainer():
                dummyRenderTarget_(), frameBuffer_(0), shadowMapFrameBuffer_(0), isFrameBufferBound_(false)
        {
                for(uint8_t i = 0; i < NUM_OF_DEPTH_BUFFERS; ++i)
                        depthBuffers_[i] = 0;
        }
        GlesRenderTargetContainer::~GlesRenderTargetContainer()
        {
                destroy();
        }

        //-----------------------------------------------------------------------------------------------------
        bool GlesRenderTargetContainer::initialize(GlesFrameParameters& frameParameters,
                                                   Renderer::Parameters& parameters)
        {
                destroy();

                // prepare parameters
                uint32_t halfWidth  = parameters.getWidth()  / 2;
                uint32_t halfHeight = parameters.getHeight() / 2;
                uint32_t nearestPowerOfTwo = parameters.getWidth() > parameters.getHeight() ?
                                             parameters.getWidth() : parameters.getHeight();

                nearestPowerOfTwo = Utility::getNearestPowerOfTwo(nearestPowerOfTwo);
                if(nearestPowerOfTwo < parameters.getWidth() ||
                   nearestPowerOfTwo < parameters.getHeight())
                        nearestPowerOfTwo += nearestPowerOfTwo;

                uint32_t shadowMapSize = nearestPowerOfTwo * 2;

                frameParameters.renderTargetSize.define(static_cast<float>(nearestPowerOfTwo));
                frameParameters.shadowMapSize.define(static_cast<float>(shadowMapSize));
                frameParameters.screenSize.define(static_cast<float>(parameters.getWidth()),
                                                  static_cast<float>(parameters.getHeight()),
                                                  static_cast<float>(halfWidth),
                                                  static_cast<float>(halfHeight));

                frameParameters.textureCoordinatesAdjustment.define(frameParameters.screenSize.x /
                                                                    static_cast<float>(nearestPowerOfTwo),
                                                                    frameParameters.screenSize.y /
                                                                    static_cast<float>(nearestPowerOfTwo),
                                                                    0.0f, 0.0f);

                // create depth buffers
                uint32_t depthBufferSizes[] = {nearestPowerOfTwo, shadowMapSize};
                for(uint8_t i = 0; i < NUM_OF_DEPTH_BUFFERS; ++i)
                {
                        depthBuffers_[i] = createDepthBuffer(depthBufferSizes[i], depthBufferSizes[i]);
                        if(depthBuffers_[i] == 0)
                        {
                                destroy();
                                return false;
                        }
                }

                // create rest of the render targets
                for(uint8_t i = 0; i < NUM_OF_RENDER_TARGETS; ++i)
                {
                        if(!renderTargets_[i].initialize(nearestPowerOfTwo, nearestPowerOfTwo))
                        {
                                destroy();
                                return false;
                        }
                }

                glGenFramebuffers(1, &frameBuffer_);
                CHECK_GLES_ERROR("GlesRenderTargetContainer::initialize: glGenFramebuffers");

                glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_);
                CHECK_GLES_ERROR("GlesRenderTargetContainer::initialize: glBindFramebuffer");

                glGenFramebuffers(1, &shadowMapFrameBuffer_);
                CHECK_GLES_ERROR("GlesRenderTargetContainer::initialize: glGenFramebuffers");

                glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFrameBuffer_);
                CHECK_GLES_ERROR("GlesRenderTargetContainer::initialize: glBindFramebuffer");

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                                       depthBuffers_[DEPTH_BUFFER_SHADOW_MAP], 0);
                CHECK_GLES_ERROR("GlesRenderTargetContainer::setShadowMap: glFramebufferTexture2D");

                if(!isFramebufferComplete())
                {
                        destroy();
                        return false;
                }

                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                CHECK_GLES_ERROR("GlesRenderTargetContainer::initialize: glBindFramebuffer");

                if(frameBuffer_ == 0 || shadowMapFrameBuffer_ == 0)
                {
                        destroy();
                        return false;
                }

                return true;
        }

        //-----------------------------------------------------------------------------------------------------
        void GlesRenderTargetContainer::destroy()
        {
                if(frameBuffer_ != 0)
                {
                        glDeleteFramebuffers(1, &frameBuffer_);
                        frameBuffer_ = 0;
                }

                if(shadowMapFrameBuffer_ != 0)
                {
                        glDeleteFramebuffers(1, &shadowMapFrameBuffer_);
                        shadowMapFrameBuffer_ = 0;
                }

                for(uint8_t i = 0; i < NUM_OF_RENDER_TARGETS; ++i)
                        renderTargets_[i].destroy();

                for(uint8_t i = 0; i < NUM_OF_DEPTH_BUFFERS; ++i)
                {
                        if(depthBuffers_[i] != 0)
                        {
                                glDeleteTextures(1, &depthBuffers_[i]);
                                depthBuffers_[i] = 0;
                        }
                }

                isFrameBufferBound_ = false;
        }

        //-----------------------------------------------------------------------------------------------------
        bool GlesRenderTargetContainer::setRenderTarget(uint8_t type) const
        {
                if(type >= NUM_OF_RENDER_TARGETS)
                        return false;

                if(!isFrameBufferBound_)
                {
                        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_);
                        CHECK_GLES_ERROR("GlesRenderTargetContainer::setRenderTarget: glBindFramebuffer");
                        isFrameBufferBound_ = true;
                }

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                       renderTargets_[type].getRenderableTexture(), 0);
                CHECK_GLES_ERROR("GlesRenderTargetContainer::setRenderTarget: glFramebufferTexture2D");

                GLuint depthBuffer = 0;
                switch(type)
                {
                        case RENDER_TARGET_NORMALS:
                                depthBuffer = depthBuffers_[DEPTH_BUFFER_NORMALS_PASS];
                                break;

                        case RENDER_TARGET_RESULT:
                                depthBuffer = depthBuffers_[DEPTH_BUFFER_SHADING_PASS];
                                break;
                }

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);
                CHECK_GLES_ERROR("GlesRenderTargetContainer::setRenderTarget: glFramebufferTexture2D");

                if(!isFramebufferComplete())
                {
                        setBackBuffer();
                        return false;
                }

                return true;
        }

        //-----------------------------------------------------------------------------------------------------
        const GlesRenderTarget& GlesRenderTargetContainer::getRenderTarget(uint8_t type) const
        {
                if(type >= NUM_OF_RENDER_TARGETS)
                        return dummyRenderTarget_;

                return renderTargets_[type];
        }

        //-----------------------------------------------------------------------------------------------------
        void GlesRenderTargetContainer::setShadowMap() const
        {
                glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFrameBuffer_);
                CHECK_GLES_ERROR("GlesRenderTargetContainer::setShadowMap: glBindFramebuffer");
                isFrameBufferBound_ = false;
        }

        //-----------------------------------------------------------------------------------------------------
        void GlesRenderTargetContainer::setBackBuffer() const
        {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                CHECK_GLES_ERROR("GlesRenderTargetContainer::setBackBuffer: glBindFramebuffer");
                isFrameBufferBound_ = false;
        }

        //-----------------------------------------------------------------------------------------------------
        GLuint GlesRenderTargetContainer::getDepthBuffer(uint8_t type) const
        {
                if(type >= NUM_OF_DEPTH_BUFFERS)
                        return 0;

                return depthBuffers_[type];
        }

        //-----------------------------------------------------------------------------------------------------
        GLuint GlesRenderTargetContainer::createDepthBuffer(uint32_t width, uint32_t height)
        {
                GLuint texture;

                glGenTextures(1, &texture);
                CHECK_GLES_ERROR("GlesRenderTargetContainer::createDepthRenderableTexture: glGenTextures");

                glBindTexture(GL_TEXTURE_2D, texture);
                CHECK_GLES_ERROR("GlesRenderTargetContainer::createDepthRenderableTexture: glBindTexture");

                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT,
                             GL_UNSIGNED_SHORT, nullptr);
                CHECK_GLES_ERROR("GlesRenderTargetContainer::createDepthRenderableTexture: glTexImage2D");

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                CHECK_GLES_ERROR("GlesRenderTargetContainer::createDepthRenderableTexture: glTexParameteri");

                glBindTexture(GL_TEXTURE_2D, 0);
                CHECK_GLES_ERROR("GlesRenderTargetContainer::createDepthRenderableTexture: glBindTexture");

                return texture;
        }

        //-----------------------------------------------------------------------------------------------------
        bool GlesRenderTargetContainer::isFramebufferComplete() const
        {
                GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
                CHECK_GLES_ERROR("GlesRenderTargetContainer::isFramebufferComplete: glCheckFramebufferStatus");

                if(status != GL_FRAMEBUFFER_COMPLETE)
                {
                        switch(status)
                        {
                                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                                        LOGI("****************************** FBO error:"
                                             "FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
                                        break;

                                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                                        LOGI("****************************** FBO error:"
                                             "FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
                                        break;

                                case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
                                        LOGI("****************************** FBO error:"
                                             "FRAMEBUFFER_INCOMPLETE_DIMENSIONS");
                                        break;

                                case GL_FRAMEBUFFER_UNSUPPORTED:
                                        LOGI("****************************** FBO error:"
                                             "FRAMEBUFFER_UNSUPPORTED");
                                        break;

                                default:
                                        LOGI("****************************** Unknown FBO error");
                        }

                        return false;
                }

                return true;
        }

}
