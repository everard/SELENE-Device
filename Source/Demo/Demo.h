// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef SIMPLE_ANIMATION_H
#define SIMPLE_ANIMATION_H

#include <Framework.h>
#include <Platform.h>

#include <iostream>
#include <string>

namespace selene
{

        // Represents application. This class must inherit from Platform::Application.
        // Application must implement various callback functions (see below).
        class DemoApplication: public Platform::Application
        {
        public:
                // Constructs application object. Defines application's name and
                // rendering area size.
                DemoApplication(const char* name, uint32_t width, uint32_t height);
                DemoApplication(const DemoApplication&) = delete;
                ~DemoApplication();
                DemoApplication& operator =(const DemoApplication&) = delete;

        private:
                // Resource managers are used for resource creation, deletion and
                // sharing. One resource manager may hold resources of any type,
                // but for convenience multiple resource managers shall be used.
                ResourceManager textureManager_, meshManager_, meshAnimationManager_;

                // File manager provides access to the file system: opens and closes
                // file streams in platform independent way.
                Platform::FileManager fileManager_;

                // Scene holds scene graph, which is used for visibility determination
                // and population of rendering lists.
                Scene scene_;

                // GUI object. Holds GUI elements such as buttons, labels, etc.
                Gui gui_;

                std::weak_ptr<Camera> camera_;
                std::weak_ptr<Gui::Element> buttonToggleSsao_;
                std::weak_ptr<Gui::Element> buttonToggleBloom_;
                std::weak_ptr<Gui::Element> buttonToggleShadows_;
                std::weak_ptr<Gui::Element> buttonToggleSettings_;
                std::weak_ptr<Actor> actor_;

                bool isCameraRotationEnabled_;
                bool isSettingsMenuVisible_;

                // Initializes GUI.
                void initializeGui();

                // Loads resources.
                void loadResources();

                // Prepares scene.
                void prepareScene();

                // Application callbacks implementation
                bool onInitialize();
                void onDestroy();
                void onKeyPress(uint8_t key);
                void onControlButtonPress(uint8_t button);
                void onControlButtonRelease(uint8_t button);
                void onUpdate(float elapsedTime);
                void onRender(float elapsedTime);

                // Toggles effect of given type.
                // Type must be one of the Renderer::Effects enumeration.
                void toggleEffect(uint8_t type);

                // Toggles visibility of the given GUI element.
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
