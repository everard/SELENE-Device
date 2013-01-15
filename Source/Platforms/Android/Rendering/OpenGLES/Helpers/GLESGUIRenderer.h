#ifndef GLES_GUI_RENDERER_H
#define GLES_GUI_RENDERER_H

#include "../Resources/GLESTexture.h"
#include "../GLESGLSLProgram.h"

namespace selene
{

        // Forward declaration of classes
        class Gui;

        // Represents OpenGL ES GUI renderer.
        class GlesGuiRenderer
        {
        public:
                GlesGuiRenderer();
                ~GlesGuiRenderer();

                // Initializes GUI renderer
                bool initialize(FileManager* fileManager);

                // Destroys GUI renderer
                void destroy();

                // Retains GUI renderer
                bool retain();

                // Discards GUI renderer
                void discard();

                // Renders GUI
                void render(Gui* gui);

        private:
                GlesGlslProgram framesRenderingProgram_;
                GlesGlslProgram textRenderingProgram_;
                GLint framesPositionLocation_, framesColorLocation_;
                GLint textPositionLocation_, textColorLocation_;
                GLint fontTextureLocation_;
                std::unique_ptr<GlesTexture> fontTexture_;

        };

}

#endif
