// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Demo.h"

namespace selene
{

        // Application entry point. C++ main() function is implemented inside platform, so
        // actual application must not define it. This function must return pointer to the
        // actual application object.
        Platform::Application* Platform::createApplication()
        {
                return new(std::nothrow) DemoApplication("SELENE Device demo",
                                                         Platform::getDefaultScreenWidth(),
                                                         Platform::getDefaultScreenHeight());
        }

        DemoApplication::DemoApplication(const char* name, uint32_t width, uint32_t height):
                Platform::Application(name, width, height), textureManager_(), meshManager_(),
                meshAnimationManager_(), fileManager_(), scene_(), gui_(), camera_(),
                buttonToggleSsao_(), buttonToggleBloom_(), buttonToggleShadows_(),
                buttonToggleSettings_(), actor_(),
                isCameraRotationEnabled_(false),
                isSettingsMenuVisible_(false)
        {
                // specify search folders for file manager
                const char* folders[] =
                {
                        "Assets/",
                        "Assets/Meshes/",
                        "Assets/Textures/",
                        "Assets/Animations/",

                        "../Assets/",
                        "../Assets/Meshes/",
                        "../Assets/Textures/",
                        "../Assets/Animations/",

                        "../../Assets/",
                        "../../Assets/Meshes/",
                        "../../Assets/Textures/",
                        "../../Assets/Animations/",

                        "../../../Assets/",
                        "../../../Assets/Meshes/",
                        "../../../Assets/Textures/",
                        "../../../Assets/Animations/",

                        "../../../../Assets/",
                        "../../../../Assets/Meshes/",
                        "../../../../Assets/Textures/",
                        "../../../../Assets/Animations/",

                        "../../../../../Assets/",
                        "../../../../../Assets/Meshes/",
                        "../../../../../Assets/Textures/",
                        "../../../../../Assets/Animations/"
                };
                const uint32_t numFolders = sizeof(folders) / sizeof(folders[0]);

                for(uint32_t i = 0; i < numFolders; ++i)
                        fileManager_.addFolder(folders[i]);
        }
        DemoApplication::~DemoApplication() {}

        //-----------------------------------------------------------------------------------------------------------
        void DemoApplication::initializeGui()
        {
                // specify colors for GUI elements
                Vector4d buttonBackgroundColors[NUM_OF_GUI_ELEMENT_COLOR_TYPES] =
                {
                        Vector4d(0.1f, 0.1f, 0.1f, 0.7f),
                        Vector4d(0.1f, 0.1f, 0.4f, 0.9f),
                        Vector4d(0.3f, 0.1f, 0.1f, 0.9f)
                };

                Vector4d labelBackgroundColors[NUM_OF_GUI_ELEMENT_COLOR_TYPES] =
                {
                        Vector4d(0.1f, 0.1f, 0.1f, 0.0f),
                        Vector4d(0.1f, 0.1f, 0.1f, 0.0f),
                        Vector4d(0.1f, 0.1f, 0.1f, 0.0f)
                };

                Vector4d textColors[NUM_OF_GUI_ELEMENT_COLOR_TYPES] =
                {
                        Vector4d(1.0f, 1.0f, 1.0f, 1.0f),
                        Vector4d(1.0f, 1.0f, 1.0f, 1.0f),
                        Vector4d(1.0f, 1.0f, 1.0f, 1.0f)
                };

                // create GUI elements (lambdas shall be used as callbacks)
                auto buttonExitCallback = [this](int32_t, uint8_t){this->onButtonMessageExit();};
                auto button = new(std::nothrow) Button(buttonExitCallback, buttonBackgroundColors, textColors,
                                                       Vector2d(0.05f, 0.07f), Vector2d(1.4f, 0.5f),
                                                       Vector2d(0.5f, 0.1f), "Exit");
                gui_.addElement(button);

                auto buttonWalkCallback = [this](int32_t, uint8_t){this->onButtonMessageWalk();};
                button = new(std::nothrow) Button(buttonWalkCallback, buttonBackgroundColors, textColors,
                                                  Vector2d(0.05f, 0.07f), Vector2d(0.1f, 0.50f),
                                                  Vector2d(0.5f, 0.1f), "Walk");
                gui_.addElement(button);

                auto buttonShootCallback = [this](int32_t, uint8_t){this->onButtonMessageShoot();};
                button = new(std::nothrow) Button(buttonShootCallback, buttonBackgroundColors, textColors,
                                                  Vector2d(0.05f, 0.07f), Vector2d(0.1f, 0.63f),
                                                  Vector2d(0.5f, 0.1f), "Shoot");
                gui_.addElement(button);

                auto buttonLookLeftCallback = [this](int32_t, uint8_t){this->onButtonMessageLookLeft();};
                button = new(std::nothrow) Button(buttonLookLeftCallback, buttonBackgroundColors, textColors,
                                                  Vector2d(0.05f, 0.07f), Vector2d(0.1f, 0.76f),
                                                  Vector2d(0.5f, 0.1f), "Look left");
                gui_.addElement(button);

                auto buttonLookRightCallback = [this](int32_t, uint8_t){this->onButtonMessageLookRight();};
                button = new(std::nothrow) Button(buttonLookRightCallback, buttonBackgroundColors, textColors,
                                                  Vector2d(0.05f, 0.07f), Vector2d(0.1f, 0.89f),
                                                  Vector2d(0.5f, 0.1f), "Look right");
                gui_.addElement(button);

                auto buttonToggleSsaoCallback = [this](int32_t, uint8_t){this->onButtonMessageToggleSsao();};
                button = new(std::nothrow) Button(buttonToggleSsaoCallback, buttonBackgroundColors, textColors,
                                                  Vector2d(0.05f, 0.07f), Vector2d(1.4f, 0.76f),
                                                  Vector2d(0.5f, 0.1f), "SSAO");
                buttonToggleSsao_ = gui_.getElement(gui_.addElement(button));

                auto buttonToggleBloomCallback = [this](int32_t, uint8_t){this->onButtonMessageToggleBloom();};
                button = new(std::nothrow) Button(buttonToggleBloomCallback, buttonBackgroundColors, textColors,
                                                  Vector2d(0.05f, 0.07f), Vector2d(1.4f, 0.89f),
                                                  Vector2d(0.5f, 0.1f), "Bloom");
                buttonToggleBloom_ = gui_.getElement(gui_.addElement(button));

                auto buttonToggleShadowsCallback = [this](int32_t, uint8_t){this->onButtonMessageToggleShadows();};
                button = new(std::nothrow) Button(buttonToggleShadowsCallback, buttonBackgroundColors, textColors,
                                                  Vector2d(0.05f, 0.07f), Vector2d(1.4f, 1.02f),
                                                  Vector2d(0.5f, 0.1f), "Shadows");
                buttonToggleShadows_ = gui_.getElement(gui_.addElement(button));

                auto buttonToggleSettingsCallback = [this](int32_t, uint8_t){this->onButtonMessageToggleSettings();};
                button = new(std::nothrow) Button(buttonToggleSettingsCallback, buttonBackgroundColors, textColors,
                                                  Vector2d(0.05f, 0.07f), Vector2d(1.4f, 0.63f),
                                                  Vector2d(0.5f, 0.1f), "[+]Effects");
                buttonToggleSettings_ = gui_.getElement(gui_.addElement(button));

                auto emptyCallback = [](int32_t, uint8_t){};
                auto label = new(std::nothrow) Label(emptyCallback, labelBackgroundColors, textColors,
                                                     Vector2d(0.05f, 0.07f), Vector2d(0.1f, 0.1f),
                                                     Vector2d(1.6f, 0.1f),
                                                     "Use controller to rotate camera.");
                gui_.addElement(label);

                toggleGuiElementVisibility(buttonToggleSsao_);
                toggleGuiElementVisibility(buttonToggleBloom_);
                toggleGuiElementVisibility(buttonToggleShadows_);
        }

        //-----------------------------------------------------------------------------------------------------------
        void DemoApplication::loadResources()
        {
                MeshFactory<Platform::Mesh> meshFactory(&fileManager_);
                TextureFactory<Platform::Texture> textureFactory(&fileManager_);
                MeshAnimationFactory<MeshAnimation> meshAnimationFactory(&fileManager_);

                meshFactory.setResourceFactory(&textureFactory);
                meshFactory.setResourceManager(&textureManager_);

                const char* meshes[] = {"girl.sdmf", "floor.sdmf"};
                const uint32_t numMeshes = sizeof(meshes) / sizeof(meshes[0]);

                for(uint32_t i = 0; i < numMeshes; ++i)
                {
                        std::cout << "Loading mesh '" << meshes[i] << "'...";
                        if(meshManager_.createResource(meshes[i], meshFactory) != SUCCESS)
                                std::cout << "FAILED" << std::endl;
                        else
                                std::cout << "SUCCEEDED" << std::endl;
                }

                const char* meshAnimations[] =
                {
                        "stand.sdaf", "walk.sdaf", "shoot.sdaf", "lookleft.sdaf", "lookright.sdaf"
                };
                const uint32_t numMeshAnimations = sizeof(meshAnimations) / sizeof(meshAnimations[0]);

                for(uint32_t i = 0; i < numMeshAnimations; ++i)
                {
                        std::cout << "Loading mesh animation '" << meshAnimations[i] << "'...";
                        if(meshAnimationManager_.createResource(meshAnimations[i], meshAnimationFactory) != SUCCESS)
                                std::cout << "FAILED" << std::endl;
                        else
                                std::cout << "SUCCEEDED" << std::endl;
                }
        }

        //-----------------------------------------------------------------------------------------------------------
        void DemoApplication::prepareScene()
        {
                // create actors
                scene_.addNode(new(std::nothrow) Actor("actor",
                                                       meshManager_.requestResource<Mesh>("girl.sdmf"),
                                                       Vector3d(0.0f, 0.0f), Quaternion(),
                                                       Vector3d(0.08f, 0.08f, 0.08f)));

                scene_.addNode(new(std::nothrow) Actor("floor",
                                                       meshManager_.requestResource<Mesh>("floor.sdmf"),
                                                       Vector3d(0.0f, 0.0f),
                                                       Quaternion(Vector3d(-1.0f, 0.0f, 0.0f) *
                                                                  std::sin(SELENE_PI * 0.25f),
                                                                  std::cos(SELENE_PI * 0.25f))));

                actor_ = scene_.getActor("actor");

                // create lights
                scene_.addNode(new(std::nothrow) DirectionalLight("directional light",
                                                                  Vector3d(-1.0f, -0.3f, 0.0f),
                                                                  Vector3d( 1.0f, 1.0f, 0.0f),
                                                                  0.5f));

                scene_.addNode(new(std::nothrow) SpotLight("spot light",
                                                           Vector3d(2.0f, 11.0f, 0.0f),
                                                           Vector3d(-5.0f, -17.0f, 0.0f),
                                                           Vector3d(1.0f, 1.0f, 1.0f),
                                                           1.0f, 30.0f));

                scene_.addNode(new(std::nothrow) PointLight("point light", Vector3d(-5.0f, 10.0f, -4.0f),
                                                            Vector3d(0.3f, 0.3f, 0.8f), 1.0f, 30.0f));

                // make spot light cast shadows
                auto spotLight = scene_.getLight("spot light").lock();
                if(spotLight)
                        spotLight->setFlags(Light::SHADOW_CASTER);

                // create camera
                scene_.addNode(new(std::nothrow) Camera("Camera",
                                                        getRenderer(),
                                                        Vector3d(0.0f, 5.0f),
                                                        Vector3d(0.0f, 0.0f, 1.0f),
                                                        Vector3d(0.0f, 1.0f),
                                                        Vector4d(45.0f,
                                                                 static_cast<float>(getHeight()) /
                                                                 static_cast<float>(getWidth()),
                                                                 1.0f,
                                                                 1000.0f),
                                                        15.0f,
                                                        &gui_));
                camera_ = scene_.getCamera("Camera");
                auto camera = camera_.lock();
                if(!camera)
                        return;

                // enable shadows (shadows are visible when current camera is used)
                camera->getEffect("Shadows").setQuality(1);

                auto actor = actor_.lock();
                if(!actor)
                        return;

                // make actor cast shadows
                actor->setFlags(Actor::SHADOW_CASTER);

                // add the first animation, its index shall be zero
                actor->addMeshAnimation(meshAnimationManager_.requestResource<MeshAnimation>("stand.sdaf"),
                                        0.0f, 0.5f, 0.5f, 15.0f, 1.0f);

                // add the second animation, its index shall be one
                actor->addMeshAnimation(meshAnimationManager_.requestResource<MeshAnimation>("walk.sdaf"),
                                        0.0f, 0.5f, 0.5f, 1.0f, 1.0f);

                // add the third animation, its index shall be two
                actor->addMeshAnimation(meshAnimationManager_.requestResource<MeshAnimation>("shoot.sdaf"),
                                        0.0f, 0.1f, 0.25f, 0.5f, 1.0f);

                // add the rest of animations
                actor->addMeshAnimation(meshAnimationManager_.requestResource<MeshAnimation>("lookleft.sdaf"),
                                        0.0f, 0.5f, 0.5f, 1.0f, 1.0f);
                actor->addMeshAnimation(meshAnimationManager_.requestResource<MeshAnimation>("lookright.sdaf"),
                                        0.0f, 0.5f, 0.5f, 1.0f, 1.0f);

                // play the first animation (stand.sdaf, which index is zero)
                actor->getMeshAnimation(0).play();
        }

        //-----------------------------------------------------------------------------------------------------------
        bool DemoApplication::onInitialize()
        {
                // initialize memory buffer of the renderer
                if(!Renderer::initializeMemoryBuffer(1024 * 20))
                        return false;

                // initialize renderer
                Renderer::Parameters parameters(this, &fileManager_, getWidth(), getHeight(), &std::cout, false);

                if(!getRenderer().initialize(parameters))
                        return false;

                // initialize GUI, load resources and prepare scene
                initializeGui();
                loadResources();
                prepareScene();

                return true;
        }

        //-----------------------------------------------------------------------------------------------------------
        void DemoApplication::onDestroy()
        {
                Renderer::destroyMemoryBuffer();

                meshAnimationManager_.destroyResources(true);
                textureManager_.destroyResources(true);
                meshManager_.destroyResources(true);
        }

        //-----------------------------------------------------------------------------------------------------------
        void DemoApplication::onKeyPress(uint8_t key)
        {
                gui_.process(getCursorPosition(0), getPressedControlButtonsMask(), key);
        }

        //-----------------------------------------------------------------------------------------------------------
        void DemoApplication::onControlButtonPress(uint8_t button)
        {
                if(IS_SET(button, CONTROL_BUTTON_0))
                        isCameraRotationEnabled_ = true;
        }

        //-----------------------------------------------------------------------------------------------------------
        void DemoApplication::onControlButtonRelease(uint8_t button)
        {
                if(IS_SET(button, CONTROL_BUTTON_0))
                        isCameraRotationEnabled_ = false;
        }

        //-----------------------------------------------------------------------------------------------------------
        void DemoApplication::onUpdate(float)
        {
                gui_.process(getCursorPosition(0), getPressedControlButtonsMask(), 0);

                // rotate camera if needed
                if(isCameraRotationEnabled_)
                {
                        auto camera = camera_.lock();

                        if(camera)
                        {
                                auto cursorShift = getCursorShift(0);
                                camera->rotateHorizontally(cursorShift.x * -5.0f);
                                camera->rotateVertically(cursorShift.y * 5.0f);
                        }
                }
        }

        //-----------------------------------------------------------------------------------------------------------
        void DemoApplication::onRender(float elapsedTime)
        {
                // render scene
                scene_.updateAndRender(elapsedTime, getRenderer());
        }

        //-----------------------------------------------------------------------------------------------------------
        void DemoApplication::toggleEffect(const char* name)
        {
                auto camera = camera_.lock();
                if(!camera)
                        return;

                auto& effect = camera->getEffect(name);
                if(effect.getQuality() == 0)
                        effect.setQuality(1);
                else
                        effect.setQuality(0);
        }

        //-----------------------------------------------------------------------------------------------------------
        void DemoApplication::toggleGuiElementVisibility(std::weak_ptr<Gui::Element>& element)
        {
                auto lockedElement = element.lock();
                if(!lockedElement)
                        return;

                if(lockedElement->is(GUI_ELEMENT_HIDDEN))
                        lockedElement->clearFlags(GUI_ELEMENT_HIDDEN);
                else
                        lockedElement->setFlags(GUI_ELEMENT_HIDDEN);
        }

        //-----------------------------------------------------------------------------------------------------------
        void DemoApplication::onButtonMessageExit()
        {
                halt();
        }

        //-----------------------------------------------------------------------------------------------------------
        void DemoApplication::onButtonMessageWalk()
        {
                auto actor = actor_.lock();
                if(!actor)
                        return;

                actor->getMeshAnimation(1).play(3U);
        }

        //-----------------------------------------------------------------------------------------------------------
        void DemoApplication::onButtonMessageShoot()
        {
                auto actor = actor_.lock();
                if(!actor)
                        return;

                actor->getMeshAnimation(2).play(1U);
        }

        //-----------------------------------------------------------------------------------------------------------
        void DemoApplication::onButtonMessageLookLeft()
        {
                auto actor = actor_.lock();
                if(!actor)
                        return;

                actor->getMeshAnimation(3).play(1U);
                actor->getMeshAnimation(4).stop();
        }

        //-----------------------------------------------------------------------------------------------------------
        void DemoApplication::onButtonMessageLookRight()
        {
                auto actor = actor_.lock();
                if(!actor)
                        return;

                actor->getMeshAnimation(4).play(1U);
                actor->getMeshAnimation(3).stop();
        }

        //-----------------------------------------------------------------------------------------------------------
        void DemoApplication::onButtonMessageToggleSsao()
        {
                toggleEffect("SSAO");
        }

        //-----------------------------------------------------------------------------------------------------------
        void DemoApplication::onButtonMessageToggleBloom()
        {
                toggleEffect("Bloom");
        }

        //-----------------------------------------------------------------------------------------------------------
        void DemoApplication::onButtonMessageToggleShadows()
        {
                toggleEffect("Shadows");
        }

        //-----------------------------------------------------------------------------------------------------------
        void DemoApplication::onButtonMessageToggleSettings()
        {
                toggleGuiElementVisibility(buttonToggleSsao_);
                toggleGuiElementVisibility(buttonToggleBloom_);
                toggleGuiElementVisibility(buttonToggleShadows_);

                isSettingsMenuVisible_ = !isSettingsMenuVisible_;

                auto buttonToggleSettings = buttonToggleSettings_.lock();
                if(!buttonToggleSettings)
                        return;

                if(isSettingsMenuVisible_)
                        buttonToggleSettings->setText("[-]Effects");
                else
                        buttonToggleSettings->setText("[+]Effects");
        }

}
