// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLESTextureHandler.h"
#include "GLESRenderTarget.h"

#include "../../../Platform.h"

namespace selene
{

        GlesTextureHandler::GlesTextureHandler()
        {
                const uint32_t numBytesPerPixel = 4;
                uint8_t pixels[NUM_OF_DUMMY_TEXTURES][numBytesPerPixel] =
                {
                        {255, 255, 255, 255},
                        {127, 127, 255, 255}
                };

                for(uint32_t i = 0; i < NUM_OF_DUMMY_TEXTURES; ++i)
                {
                        auto& textureData = dummyTextures_[i].getData();
                        textureData.bpp   = numBytesPerPixel;
                        textureData.format = TEXTURE_FORMAT_NOT_COMPRESSED;
                        textureData.width  = textureData.height = 1;
                        textureData.numMipMaps = 1;
                        if(!textureData.pixels.create(numBytesPerPixel))
                                return;

                        memcpy(&textureData.pixels[0], pixels[i], numBytesPerPixel);
                }
        }
        GlesTextureHandler::~GlesTextureHandler()
        {
                destroy();
        }

        //----------------------------------------------------------------------------------------------
        bool GlesTextureHandler::initialize()
        {
                for(uint32_t i = 0; i < NUM_OF_DUMMY_TEXTURES; ++i)
                {
                        if(!dummyTextures_[i].retain())
                        {
                                destroy();
                                return false;
                        }
                }

                return true;
        }

        //----------------------------------------------------------------------------------------------
        void GlesTextureHandler::destroy()
        {
                for(uint32_t i = 0; i < NUM_OF_DUMMY_TEXTURES; ++i)
                {
                        dummyTextures_[i].discard();
                }
        }

        //----------------------------------------------------------------------------------------------
        void GlesTextureHandler::setTexture(const Resource::Instance<Texture>& texture, GLenum sampler,
                                            uint8_t dummyTextureIndex)
        {
                setTexture(*texture, sampler, dummyTextureIndex);
        }

        //----------------------------------------------------------------------------------------------
        void GlesTextureHandler::setTexture(Texture* texture, GLenum sampler, uint8_t dummyTextureIndex)
        {
                glActiveTexture(GL_TEXTURE0 + sampler);
                CHECK_GLES_ERROR("GlesTextureHandler::setTexture: glActiveTexture");

                if(texture == nullptr)
                        glBindTexture(GL_TEXTURE_2D, dummyTextures_[dummyTextureIndex].texture_);
                else
                {
                        GlesTexture* glesTexture = static_cast<GlesTexture*>(texture);
                        glBindTexture(GL_TEXTURE_2D, glesTexture->texture_);
                }

                CHECK_GLES_ERROR("GlesTextureHandler::setTexture: glBindTexture");
        }

        //----------------------------------------------------------------------------------------------
        void GlesTextureHandler::setTexture(const GlesRenderTarget& renderTarget, GLenum sampler)
        {
                glActiveTexture(GL_TEXTURE0 + sampler);
                CHECK_GLES_ERROR("GlesTextureHandler::setTexture: glActiveTexture");

                glBindTexture(GL_TEXTURE_2D, renderTarget.renderableTexture_);
                CHECK_GLES_ERROR("GlesTextureHandler::setTexture: glBindTexture");
        }

        //----------------------------------------------------------------------------------------------
        void GlesTextureHandler::setSamplerState(GLenum sampler, GLint magFilter, GLint minFilter,
                                                 GLint textureCoordinateModeS,
                                                 GLint textureCoordinateModeT)
        {
                glActiveTexture(GL_TEXTURE0 + sampler);
                CHECK_GLES_ERROR("GlesTextureHandler::setSamplerState: glActiveTexture");

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureCoordinateModeS);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureCoordinateModeT);

                CHECK_GLES_ERROR("GlesTextureHandler::setSamplerState: glTexParameteri");
        }

}
