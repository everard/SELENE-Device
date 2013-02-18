// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLESRenderTargetContainer.h"
#include "../GLESRenderer.h"

#include "../../../Platform.h"

namespace selene
{

        // Returns nearest power of two
        uint32_t getNearestPowerOfTwo(uint32_t x)
        {
                --x;
                x |= x >> 1;
                x |= x >> 2;
                x |= x >> 4;
                x |= x >> 8;
                x |= x >> 16;
                return ++x;
        }

        GlesRenderTargetContainer::GlesRenderTargetContainer()
        {
                depthRenderbuffer_ = 0;
        }
        GlesRenderTargetContainer::~GlesRenderTargetContainer()
        {
                destroy();
        }

        //------------------------------------------------------------------------------------
        bool GlesRenderTargetContainer::initialize(GlesFrameParameters& frameParameters,
                                                   Renderer::Parameters& parameters,
                                                   GlesCapabilities& capabilities)
        {
                destroy();

                // prepare parameters
                uint32_t halfWidth  = parameters.getWidth()  / 2;
                uint32_t halfHeight = parameters.getHeight() / 2;
                uint32_t nearestPowerOfTwo = parameters.getWidth() > parameters.getHeight() ?
                                             parameters.getWidth() : parameters.getHeight();

                nearestPowerOfTwo = getNearestPowerOfTwo(nearestPowerOfTwo);
                if(nearestPowerOfTwo < parameters.getWidth() ||
                   nearestPowerOfTwo < parameters.getHeight())
                        nearestPowerOfTwo += nearestPowerOfTwo;

                uint32_t shadowMapSize = nearestPowerOfTwo / 2;

                frameParameters.shadowMapKernelSize.define(1.0f / static_cast<float>(shadowMapSize));
                frameParameters.screenSize.define(static_cast<float>(parameters.getWidth()),
                                                  static_cast<float>(parameters.getHeight()),
                                                  static_cast<float>(halfWidth),
                                                  static_cast<float>(halfHeight));

                frameParameters.textureCoordinatesAdjustment.define(frameParameters.screenSize.x / static_cast<float>(nearestPowerOfTwo),
                                                                    frameParameters.screenSize.y / static_cast<float>(nearestPowerOfTwo),
                                                                    0.0f, 0.0f);

                // create depth and stencil renderbuffers
                depthRenderbuffer_ = GlesRenderTarget::createRenderbuffer(nearestPowerOfTwo, nearestPowerOfTwo, GL_DEPTH_COMPONENT16);
                if(depthRenderbuffer_ == 0)
                {
                        destroy();
                        return false;
                }

                // create rest of the render targets
                for(uint8_t i = 0; i < NUM_OF_RENDER_TARGETS; ++i)
                {
                        if(!renderTargets_[i].initialize(nearestPowerOfTwo, nearestPowerOfTwo, depthRenderbuffer_))
                        {
                                destroy();
                                return false;
                        }
                }

                return true;
        }

        //------------------------------------------------------------------------------------
        void GlesRenderTargetContainer::destroy()
        {
                for(uint8_t i = 0; i < NUM_OF_RENDER_TARGETS; ++i)
                        renderTargets_[i].destroy();

                dummyRenderTarget_.destroy();

                if(depthRenderbuffer_ != 0)
                {
                        glDeleteRenderbuffers(1, &depthRenderbuffer_);
                        depthRenderbuffer_ = 0;
                }
        }

        //------------------------------------------------------------------------------------
        const GlesRenderTarget& GlesRenderTargetContainer::getRenderTarget(uint8_t type) const
        {
                if(type >= NUM_OF_RENDER_TARGETS)
                        return dummyRenderTarget_;

                return renderTargets_[type];
        }

        //------------------------------------------------------------------------------------
        void GlesRenderTargetContainer::setBackBuffer() const
        {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        //------------------------------------------------------------------------------------
        /*const D3d9RenderTarget& D3d9RenderTargetContainer::getShadowMap() const
        {
                return shadowMap_;
        }*/

}
