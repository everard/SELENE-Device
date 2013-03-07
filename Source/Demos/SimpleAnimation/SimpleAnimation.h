#ifndef SIMPLE_ANIMATION_H
#define SIMPLE_ANIMATION_H

#include <Framework.h>
#include <Platform.h>

#include <iostream>
#include <string>

namespace selene
{

        // Represents game application.
        class SimpleAnimation: public Platform::Application
        {
        public:
                SimpleAnimation(const char* name, uint32_t width, uint32_t height);
                ~SimpleAnimation();

        private:
                ResourceManager textureManager_, meshManager_, meshAnimationManager_;
                Platform::FileManager fileManager_;

                Scene scene_;
                Gui gui_;

                std::weak_ptr<Camera> camera_;
                bool isCameraRotationEnabled_;

                std::weak_ptr<Gui::Element> buttonToggleSsao_;
                std::weak_ptr<Gui::Element> buttonToggleBloom_;
                std::weak_ptr<Gui::Element> buttonToggleShadows_;
                std::weak_ptr<Gui::Element> buttonToggleSettings_;
                bool isSettingsMenuVisible_;

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

                // Toggles effect
                void toggleEffect(uint8_t type);

                // Toggles GUI element visibility
                void toggleGuiElementVisibility(std::weak_ptr<Gui::Element>& element);

                // GUI message callbacks
                void onButtonMessageExit();
                void onButtonMessageWalk();
                void onButtonMessageShoot();
                void onButtonMessageLookLeft();
                void onButtonMessageLookRight();
                void onButtonMessageToggleSsao();
                void onButtonMessageToggleBloom();
                void onButtonMessageToggleShadows();
                void onButtonMessageToggleSettings();

        };

}

#endif
