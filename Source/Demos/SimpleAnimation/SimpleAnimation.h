#ifndef SIMPLE_ANIMATION_H
#define SIMPLE_ANIMATION_H

#include "../../Engine/Framework.h"
#include "../../Platforms/Windows/Platform.h"

#include <iostream>
#include <string>

namespace selene
{

        // Represents game application.
        class SimpleAnimation: public WindowsApplication
        {
        public:
                SimpleAnimation(const char* name, uint32_t width, uint32_t height);
                ~SimpleAnimation();

        private:
                // Resource managers
                ResourceManager textureManager_, meshManager_, meshAnimationManager_;

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
                void onButtonMessageWalk(int32_t elementId, uint8_t message);
                void onButtonMessageShoot(int32_t elementId, uint8_t message);
                void onButtonMessageLookLeft(int32_t elementId, uint8_t message);
                void onButtonMessageLookRight(int32_t elementId, uint8_t message);

        };

}

#endif
