// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GLES_TEXTURE_HANDLER_H
#define GLES_TEXTURE_HANDLER_H

#include "../Resources/GLESTexture.h"

namespace selene
{

        /**
         * \addtogroup Android
         * @{
         */

        // Forward declaration of classes
        class GlesRenderTarget;

        /**
         * Represents OpenGL ES texture handler.
         */
        class GlesTextureHandler
        {
        public:
                /// Dummy texture indices
                enum
                {
                        DUMMY_TEXTURE_WHITE = 0,
                        DUMMY_TEXTURE_NORMAL_MAP,
                        NUM_OF_DUMMY_TEXTURES
                };

                GlesTextureHandler();
                GlesTextureHandler(const GlesTextureHandler&) = delete;
                ~GlesTextureHandler();
                GlesTextureHandler& operator =(const GlesTextureHandler&) = delete;

                /**
                 * \brief Initializes texture handler.
                 * \return true if texture handler has been successfully initialized
                 */
                bool initialize();

                /**
                 * \brief Destroys texture handler.
                 */
                void destroy();

                /**
                 * \brief Sets given texture on the given stage.
                 * \param[in] texture instance of the texture, which should be set
                 * \param[in] sampler texture sampler
                 * \param[in] dummyTextureIndex index of the dummy texture, which will be
                 * set if given texture instance is empty
                 */
                void setTexture(const Resource::Instance<Texture>& texture, GLenum sampler,
                                uint8_t dummyTextureIndex);

                /**
                 * \brief Sets given texture on the given stage.
                 * \param[in] texture pointer to the texture, which should be set
                 * \param[in] sampler texture sampler
                 * \param[in] dummyTextureIndex index of the dummy texture, which will be
                 * set if texture equals to nullptr
                 */
                void setTexture(Texture* texture, GLenum sampler, uint8_t dummyTextureIndex);

                /**
                 * \brief Sets given texture on the given stage.
                 * \param[in] renderTarget render target, which contains texture
                 * \param[in] sampler texture sampler
                 */
                void setTexture(const GlesRenderTarget& renderTarget, GLenum sampler);

                /**
                 * \brief Sets given texture on the given stage.
                 * \param[in] texture OpenGL texture name
                 * \param[in] sampler texture sampler
                 */
                void setTexture(GLuint texture, GLenum sampler);

                /**
                 * \brief Sets texture sampler state.
                 * \param[in] sampler texture sampler
                 * \param[in] magFilter texture magnification filter
                 * \param[in] minFilter texture minification filter
                 * \param[in] textureCoordinateModeS mode for S texture coordinate
                 * \param[in] textureCoordinateModeT mode for T texture coordinate
                 */
                void setSamplerState(GLenum sampler, GLint magFilter, GLint minFilter,
                                     GLint textureCoordinateModeS = GL_REPEAT,
                                     GLint textureCoordinateModeT = GL_REPEAT);

        private:
                GlesTexture dummyTextures_[NUM_OF_DUMMY_TEXTURES];

        };

        /**
         * @}
         */

}

#endif
