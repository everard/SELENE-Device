#ifndef SIMPLE_APPLICATION_H
#define SIMPLE_APPLICATION_H

#include <Framework.h>
#include <Platform.h>

#include <iostream>
#include <string>

namespace selene
{

        // Represents game application.
        class SimpleApplication: public Platform::Application
        {
        public:
                SimpleApplication(const char* name, uint32_t width, uint32_t height);
                ~SimpleApplication();

        private:
                ResourceManager textureManager_, meshManager_;
                Platform::FileManager fileManager_;

                Scene scene_;
                Gui gui_;
                int32_t labelId_, textBoxId_;

                std::weak_ptr<Camera> camera_;
                bool isCameraRotationEnabled_;

                // Initialization event callback
                bool onInitialize();

                // Destruction event callback
                void onDestroy();

                // Key press event callback
                void onKeyPress(uint8_t key);

                // Control button press event callback
                void onControlButtonPress(uint8_t button);

                // Control button release event callback
                void onControlButtonRelease(uint8_t button);

                // Update event callback
                void onUpdate(float elapsedTime);

                // Rendering event callback
                void onRender(float elapsedTime);

                // GUI message callbacks
                void onButtonMessageExit();
                void onButtonMessageCopy();

        };

}

#endif
