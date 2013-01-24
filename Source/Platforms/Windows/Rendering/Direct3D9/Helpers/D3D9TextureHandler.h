// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef D3D9_TEXTURE_HANDLER_H
#define D3D9_TEXTURE_HANDLER_H

#include "../Resources/D3D9Texture.h"
#include "D3D9Shader.h"

namespace selene
{

        /**
         * \addtogroup Windows
         * @{
         */

        /**
         * Represents texture handler.
         */
        class D3d9TextureHandler
        {
        public:
                /// Dummy texture indices
                enum
                {
                        DUMMY_TEXTURE_WHITE = 0,
                        DUMMY_TEXTURE_NORMAL_MAP,
                        NUM_OF_DUMMY_TEXTURES
                };

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
                 * \param[in] stage texture stage
                 * \param[in] dummyTextureIndex index of the dummy texture, which will be
                 * set if given texture instance is empty
                 */
                void setTexture(const Resource::Instance<Texture>& texture, DWORD stage,
                                uint8_t dummyTextureIndex);

                /**
                 * \brief Sets given texture on the given stage.
                 * \param[in] texture pointer to the texture, which should be set
                 * \param[in] stage texture stage
                 * \param[in] dummyTextureIndex index of the dummy texture, which will be
                 * set if texture equals to nullptr
                 */
                void setTexture(Texture* texture, DWORD stage, uint8_t dummyTextureIndex);

                /**
                 * \brief Sets texture stage state.
                 * \param[in] stage texture stage
                 * \param[in] magFilter texture magnification filter
                 * \param[in] minFilter texture minification filter
                 * \param[in] mipFilter texture mipmap filter
                 * \param[in] textureCoordinateModeU mode for U texture coordinate
                 * \param[in] textureCoordinateModeV mode for V texture coordinate
                 */
                void setStageState(DWORD stage, DWORD magFilter, DWORD minFilter, DWORD mipFilter,
                                   DWORD textureCoordinateModeU = D3DTADDRESS_WRAP,
                                   DWORD textureCoordinateModeV = D3DTADDRESS_WRAP);

        private:
                friend class D3d9Renderer;

                LPDIRECT3DTEXTURE9 d3dDummyTextures_[NUM_OF_DUMMY_TEXTURES];
                LPDIRECT3DDEVICE9 d3dDevice_;

                D3d9TextureHandler();
                D3d9TextureHandler(const D3d9TextureHandler& textureHandler_);
                ~D3d9TextureHandler();

        };

        /**
         * @}
         */

}

#endif
