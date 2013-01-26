// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef D3D9_GUI_RENDERER_H
#define D3D9_GUI_RENDERER_H

#include "../../../../../Engine/Core/Math/Vector.h"
#include "../Resources/D3D9Texture.h"
#include "D3D9Shader.h"

namespace selene
{

        /**
         * \addtogroup Windows
         * @{
         */

        // Forward declaration of classes
        class D3d9FullScreenQuad;
        class D3d9TextureHandler;
        class D3d9Capabilities;
        class FileManager;
        class Gui;

        // Represents D3D9 GUI renderer.
        class D3d9GuiRenderer
        {
        public:
                D3d9GuiRenderer();
                ~D3d9GuiRenderer();

                /**
                 * \brief Initializes GUI renderer.
                 * \param[in] fullScreenQuad full-screen quad
                 * \param[in] textureHandler texture handler
                 * \param[in] capabilities D3D capabilities
                 * \param[in] fileManager file manager, which helps to load textures
                 */
                bool initialize(D3d9FullScreenQuad& fullScreenQuad,
                                D3d9TextureHandler& textureHandler,
                                D3d9Capabilities& capabilities,
                                FileManager* fileManager);

                /**
                 * \brief Destroys GUI renderer.
                 */
                void destroy();

                /**
                 * \brief Renders GUI.
                 * \param[in] gui GUI, which will be rendered
                 */
                void renderGui(Gui* gui);

        private:
                /// Helper constants
                enum
                {
                        VERTEX_SHADER_GUI_FRAMES_PASS = 0,
                        VERTEX_SHADER_GUI_TEXT_PASS,
                        VERTEX_SHADER_GUI_CURSOR_PASS,
                        NUM_OF_VERTEX_SHADERS,

                        PIXEL_SHADER_GUI_FRAMES_PASS = 0,
                        PIXEL_SHADER_GUI_TEXT_PASS,
                        PIXEL_SHADER_GUI_CURSOR_PASS,
                        NUM_OF_PIXEL_SHADERS,

                        LOCATION_CURSOR_POSITION_AND_SIZE = 0
                };

                D3d9VertexShader vertexShaders_[NUM_OF_VERTEX_SHADERS];
                D3d9PixelShader  pixelShaders_[NUM_OF_PIXEL_SHADERS];

                LPDIRECT3DVERTEXDECLARATION9 d3dVertexDeclaration_;
                LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffer_;
                LPDIRECT3DDEVICE9 d3dDevice_;

                std::unique_ptr<D3d9Texture> fontTexture_, cursorTexture_;

                D3d9FullScreenQuad* fullScreenQuad_;
                D3d9TextureHandler* textureHandler_;
                D3d9Capabilities* capabilities_;

                /**
                 * \brief Renders GUI geometry.
                 * \param[in] vertices array of vertices
                 * \param[in] numFrames number of frames to render
                 */
                void renderGeometry(const Vector4d* vertices, uint32_t numFrames);

        };

        /**
         * @}
         */

}

#endif
