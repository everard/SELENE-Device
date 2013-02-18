// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GLES_GUI_RENDERER_H
#define GLES_GUI_RENDERER_H

#include "../../../../../Engine/Core/Math/Vector.h"
#include "../Resources/GLESTexture.h"
#include "GLESGLSLProgram.h"

namespace selene
{

        /**
         * \addtogroup Android
         * @{
         */

        // Forward declaration of classes
        class GlesTextureHandler;
        class FileManager;
        class Gui;

        /**
         * Represents OpenGL ES GUI renderer.
         */
        class GlesGuiRenderer
        {
        public:
                GlesGuiRenderer();
                ~GlesGuiRenderer();

                /**
                 * \brief Initializes GUI renderer.
                 * \param[in] textureHandler texture handler
                 * \param[in] fileManager file manager, which helps to load textures
                 * \return true if GUI renderer has been successfully initialized
                 */
                bool initialize(GlesTextureHandler& textureHandler,
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
                        LOCATION_ATTRIBUTE_POSITION = 0,
                        LOCATION_ATTRIBUTE_COLOR
                };

                GlesGlslProgram framesRenderingProgram_;
                GlesGlslProgram textRenderingProgram_;

                GLint fontTextureLocation_;
                std::unique_ptr<GlesTexture> fontTexture_;

                GlesTextureHandler* textureHandler_;

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
