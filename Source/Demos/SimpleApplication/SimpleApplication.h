#ifndef SIMPLE_APPLICATION_H
#define SIMPLE_APPLICATION_H

#include "../../Engine/Framework.h"
#include "../../Platforms/Windows/Platform.h"

#include <iostream>
#include <string>

namespace selene
{

        // Represents game application.
        class SimpleApplication: public WindowsApplication
        {
        public:
                SimpleApplication(const char* name, uint32_t width, uint32_t height);
                ~SimpleApplication();

        private:
                // Resource managers
                ResourceManager textureManager_, meshManager_;

                // File manager
                FileManager fileManager_;

                // Renderer
                D3d9Renderer renderer_;

                // Camera
                Camera camera_;

                // Scene
                Scene scene_;

                // GUI
                Gui gui_;

                // GUI elements IDs
                int32_t labelId_, textBoxId_;

                // Flag
                bool isGuiHidden_;

                // Initialization event callback
                bool onInitialize();

                // Destruction event callback
                void onDestroy();

                // Key press event callback
                void onKeyPress(uint8_t key);

                // Update event callback
                void onUpdate(float elapsedTime);

                // Rendering event callback
                void onRender(float elapsedTime);

                // GUI message callbacks
                void onButtonMessageExit(int32_t elementId, uint8_t message);
                void onButtonMessageCopy(int32_t elementId, uint8_t message);

        };

}

#endif
