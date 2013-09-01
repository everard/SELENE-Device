#include "SimpleAnimation.h"

namespace selene
{

        // Entry point
        Platform::Application* Platform::createApplication()
        {
                return new(std::nothrow) SimpleAnimation("SELENE Device",
                                                         Platform::getDefaultScreenWidth(),
                                                         Platform::getDefaultScreenHeight());
        }

        SimpleAnimation::SimpleAnimation(const char* name, uint32_t width, uint32_t height):
                Platform::Application(name, width, height), textureManager_(), meshManager_(),
                meshAnimationManager_(), fileManager_(), scene_(), gui_(), camera_(),
                buttonToggleSsao_(), buttonToggleBloom_(), buttonToggleShadows_(),
                buttonToggleSettings_(), isCameraRotationEnabled_(false),
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
        SimpleAnimation::~SimpleAnimation() {}

        //--------------------------------------------------------------------------------------------------------------
        bool SimpleAnimation::onInitialize()
        {
                // initialize memory buffer of the renderer
                if(!Renderer::initializeMemoryBuffer(1024 * 20))
                        return false;

                // initialize renderer
                uint8_t flags = RENDERING_BLOOM_ENABLED | RENDERING_SSAO_ENABLED | RENDERING_SHADOWS_ENABLED;
                Renderer::Parameters parameters(this, &fileManager_, width_, height_, &std::cout, flags);

                if(!renderer_.initialize(parameters))
                        return false;

                // initialize GUI
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

                auto button =
                        new(std::nothrow) Button(std::function<void (int32_t, uint8_t)>([this] (int32_t, uint8_t)
                                                 {this->onButtonMessageExit();}),
                                                 buttonBackgroundColors,
                                                 textColors,
                                                 Vector2d(0.05f, 0.07f),
                                                 Vector2d(1.4f, 0.5f),
                                                 Vector2d(0.5f, 0.1f),
                                                 "Exit");
                gui_.addElement(button);

                button =
                        new(std::nothrow) Button(std::function<void (int32_t, uint8_t)>([this] (int32_t, uint8_t)
                                                 {this->onButtonMessageWalk();}),
                                                 buttonBackgroundColors,
                                                 textColors,
                                                 Vector2d(0.05f, 0.07f),
                                                 Vector2d(0.1f, 0.50f),
                                                 Vector2d(0.5f, 0.1f),
                                                 "Walk");
                gui_.addElement(button);

                button =
                        new(std::nothrow) Button(std::function<void (int32_t, uint8_t)>([this] (int32_t, uint8_t)
                                                 {this->onButtonMessageShoot();}),
                                                 buttonBackgroundColors,
                                                 textColors,
                                                 Vector2d(0.05f, 0.07f),
                                                 Vector2d(0.1f, 0.63f),
                                                 Vector2d(0.5f, 0.1f),
                                                 "Shoot");
                gui_.addElement(button);

                button =
                        new(std::nothrow) Button(std::function<void (int32_t, uint8_t)>([this] (int32_t, uint8_t)
                                                 {this->onButtonMessageLookLeft();}),
                                                 buttonBackgroundColors,
                                                 textColors,
                                                 Vector2d(0.05f, 0.07f),
                                                 Vector2d(0.1f, 0.76f),
                                                 Vector2d(0.5f, 0.1f),
                                                 "Look left");
                gui_.addElement(button);

                button =
                        new(std::nothrow) Button(std::function<void (int32_t, uint8_t)>([this] (int32_t, uint8_t)
                                                 {this->onButtonMessageLookRight();}),
                                                 buttonBackgroundColors,
                                                 textColors,
                                                 Vector2d(0.05f, 0.07f),
                                                 Vector2d(0.1f, 0.89f),
                                                 Vector2d(0.5f, 0.1f),
                                                 "Look right");
                gui_.addElement(button);

                button =
                        new(std::nothrow) Button(std::function<void (int32_t, uint8_t)>([this] (int32_t, uint8_t)
                                                 {this->onButtonMessageToggleSsao();}),
                                                 buttonBackgroundColors,
                                                 textColors,
                                                 Vector2d(0.05f, 0.07f),
                                                 Vector2d(1.4f, 0.76f),
                                                 Vector2d(0.5f, 0.1f),
                                                 "SSAO");
                int32_t elementId = gui_.addElement(button);
                buttonToggleSsao_ = gui_.getElement(elementId);

                button =
                        new(std::nothrow) Button(std::function<void (int32_t, uint8_t)>([this] (int32_t, uint8_t)
                                                 {this->onButtonMessageToggleBloom();}),
                                                 buttonBackgroundColors,
                                                 textColors,
                                                 Vector2d(0.05f, 0.07f),
                                                 Vector2d(1.4f, 0.89f),
                                                 Vector2d(0.5f, 0.1f),
                                                 "Bloom");
                elementId = gui_.addElement(button);
                buttonToggleBloom_ = gui_.getElement(elementId);

                button =
                        new(std::nothrow) Button(std::function<void (int32_t, uint8_t)>([this] (int32_t, uint8_t)
                                                 {this->onButtonMessageToggleShadows();}),
                                                 buttonBackgroundColors,
                                                 textColors,
                                                 Vector2d(0.05f, 0.07f),
                                                 Vector2d(1.4f, 1.02f),
                                                 Vector2d(0.5f, 0.1f),
                                                 "Shadows");
                elementId = gui_.addElement(button);
                buttonToggleShadows_ = gui_.getElement(elementId);

                button =
                        new(std::nothrow) Button(std::function<void (int32_t, uint8_t)>([this] (int32_t, uint8_t)
                                                 {this->onButtonMessageToggleSettings();}),
                                                 buttonBackgroundColors,
                                                 textColors,
                                                 Vector2d(0.05f, 0.07f),
                                                 Vector2d(1.4f, 0.63f),
                                                 Vector2d(0.5f, 0.1f),
                                                 "[+]Effects");
                elementId = gui_.addElement(button);
                buttonToggleSettings_ = gui_.getElement(elementId);

                auto label =
                        new(std::nothrow) Label(std::function<void (int32_t, uint8_t)>(),
                                                labelBackgroundColors,
                                                textColors,
                                                Vector2d(0.05f, 0.07f),
                                                Vector2d(0.1f, 0.1f),
                                                Vector2d(1.6f, 0.1f),
                                                "Use controller to rotate camera.");
                gui_.addElement(label);

                toggleGuiElementVisibility(buttonToggleSsao_);
                toggleGuiElementVisibility(buttonToggleBloom_);
                toggleGuiElementVisibility(buttonToggleShadows_);

                // load mesh
                MeshFactory<Platform::Mesh> meshFactory(&fileManager_);
                TextureFactory<Platform::Texture> textureFactory(&fileManager_);
                MeshAnimationFactory<MeshAnimation> meshAnimationFactory(&fileManager_);

                meshFactory.setResourceFactory(&textureFactory);
                meshFactory.setResourceManager(&textureManager_);

                const char* meshes[] =
                {
                        "girl.sdmf", "floor.sdmf"
                };
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

                // create scene objects
                scene_.addNode(new(std::nothrow) Actor("object",
                                                       meshManager_.requestResource<Mesh>("girl.sdmf"),
                                                       Vector3d(0.0f, 0.0f),
                                                       Quaternion(),
                                                       Vector3d(0.08f, 0.08f, 0.08f)));

                scene_.addNode(new(std::nothrow) Actor("floor",
                                                       meshManager_.requestResource<Mesh>("floor.sdmf"),
                                                       Vector3d(0.0f, 0.0f),
                                                       Quaternion(Vector3d(-1.0f, 0.0f, 0.0f) *
                                                                  std::sin(SELENE_PI * 0.25f),
                                                                  std::cos(SELENE_PI * 0.25f))));

                scene_.addNode(new(std::nothrow) DirectionalLight("directional light",
                                                                  Vector3d(-1.0f, -0.3f, 0.0f),
                                                                  Vector3d( 1.0f, 1.0f, 0.0f),
                                                                  0.5f));

                scene_.addNode(new(std::nothrow) SpotLight("spot light",
                                                           Vector3d(2.0f, 11.0f, 0.0f),
                                                           Vector3d(-5.0f, -17.0f, 0.0f),
                                                           Vector3d(1.0f, 1.0f, 1.0f),
                                                           1.0f, 30.0f));

                auto light = scene_.getLight("spot light").lock();
                if(light)
                        light->setFlags(Light::SHADOW_CASTER);

                scene_.addNode(new(std::nothrow) PointLight("point light",
                                                            Vector3d(-5.0f, 10.0f, -4.0f),
                                                            Vector3d(0.3f, 0.3f, 0.8f),
                                                            1.0f, 30.0f));

                scene_.addNode(new(std::nothrow) Camera("Camera",
                                                        Vector3d(0.0f, 5.0f),
                                                        Vector3d(0.0f, 0.0f, 1.0f),
                                                        Vector3d(0.0f, 1.0f),
                                                        Vector4d(45.0f,
                                                                 static_cast<float>(height_) /
                                                                 static_cast<float>(width_),
                                                                 1.0f,
                                                                 1000.0f),
                                                        15.0f,
                                                        &gui_));
                camera_ = scene_.getCamera("Camera");
                auto camera = camera_.lock();
                if(camera)
                        camera->enableEffect(Renderer::Effects::SHADOWS);

                auto weakObject = scene_.getActor("object");
                auto object = weakObject.lock();
                if(object)
                {
                        object->setFlags(Actor::SHADOW_CASTER);

                        // animations must be added to the actor

                        // first animation (index is zero)
                        object->addMeshAnimation(meshAnimationManager_.requestResource<MeshAnimation>("stand.sdaf"),
                                                 0.0f, 0.5f, 0.5f, 15.0f, 1.0f);

                        // second animation (index is one)
                        object->addMeshAnimation(meshAnimationManager_.requestResource<MeshAnimation>("walk.sdaf"),
                                                 0.0f, 0.5f, 0.5f, 1.0f, 1.0f);

                        // third animation (index is two)
                        object->addMeshAnimation(meshAnimationManager_.requestResource<MeshAnimation>("shoot.sdaf"),
                                                 0.0f, 0.1f, 0.25f, 0.5f, 1.0f);

                        // etc...
                        object->addMeshAnimation(meshAnimationManager_.requestResource<MeshAnimation>("lookleft.sdaf"),
                                                 0.0f, 0.5f, 0.5f, 1.0f, 1.0f);
                        object->addMeshAnimation(meshAnimationManager_.requestResource<MeshAnimation>("lookright.sdaf"),
                                                 0.0f, 0.5f, 0.5f, 1.0f, 1.0f);

                        // play the first animation (stand)
                        object->getMeshAnimation(0).play();
                }

                return true;
        }

        //--------------------------------------------------------------------------------------------------------------
        void SimpleAnimation::onDestroy()
        {
                Renderer::destroyMemoryBuffer();

                meshAnimationManager_.destroyResources(true);
                textureManager_.destroyResources(true);
                meshManager_.destroyResources(true);
        }

        //--------------------------------------------------------------------------------------------------------------
        void SimpleAnimation::onKeyPress(uint8_t key)
        {
                gui_.process(cursorPosition_, pressedControlButtons_, key);
        }

        //--------------------------------------------------------------------------------------------------------------
        void SimpleAnimation::onControlButtonPress(uint8_t button)
        {
                if(IS_SET(button, CONTROL_BUTTON_0))
                        isCameraRotationEnabled_ = true;
        }

        //--------------------------------------------------------------------------------------------------------------
        void SimpleAnimation::onControlButtonRelease(uint8_t button)
        {
                if(IS_SET(button, CONTROL_BUTTON_0))
                        isCameraRotationEnabled_ = false;
        }

        //--------------------------------------------------------------------------------------------------------------
        void SimpleAnimation::onUpdate(float)
        {
                // rotate camera
                if(isCameraRotationEnabled_)
                {
                        auto camera = camera_.lock();

                        if(camera)
                        {
                                camera->rotateHorizontally(cursorShift_.x * -5.0f);
                                camera->rotateVertically(cursorShift_.y * 5.0f);
                        }
                }

                // process GUI
                gui_.process(cursorPosition_, pressedControlButtons_, 0);
        }

        //--------------------------------------------------------------------------------------------------------------
        void SimpleAnimation::onRender(float elapsedTime)
        {
                // render scene
                scene_.updateAndRender(elapsedTime, renderer_);
        }

        //--------------------------------------------------------------------------------------------------------------
        void SimpleAnimation::toggleEffect(uint8_t type)
        {
                auto camera = camera_.lock();
                if(!camera)
                        return;

                if(camera->isEffectEnabled(type))
                        camera->disableEffect(type);
                else
                        camera->enableEffect(type);
        }

        //--------------------------------------------------------------------------------------------------------------
        void SimpleAnimation::toggleGuiElementVisibility(std::weak_ptr<Gui::Element>& element)
        {
                auto lockedElement = element.lock();
                if(!lockedElement)
                        return;

                if(lockedElement->is(GUI_ELEMENT_HIDDEN))
                        lockedElement->clearFlags(GUI_ELEMENT_HIDDEN);
                else
                        lockedElement->setFlags(GUI_ELEMENT_HIDDEN);
        }

        //--------------------------------------------------------------------------------------------------------------
        void SimpleAnimation::onButtonMessageExit()
        {
                // halt application
                halt();
        }

        //--------------------------------------------------------------------------------------------------------------
        void SimpleAnimation::onButtonMessageWalk()
        {
                auto weakObject = scene_.getActor("object");
                if(!weakObject.expired())
                {
                        auto object = weakObject.lock();
                        object->getMeshAnimation(1).play(3U);
                }
        }

        //--------------------------------------------------------------------------------------------------------------
        void SimpleAnimation::onButtonMessageShoot()
        {
                auto weakObject = scene_.getActor("object");
                if(!weakObject.expired())
                {
                        auto object = weakObject.lock();
                        object->getMeshAnimation(2).play(1U);
                }
        }

        //--------------------------------------------------------------------------------------------------------------
        void SimpleAnimation::onButtonMessageLookLeft()
        {
                auto weakObject = scene_.getActor("object");
                if(!weakObject.expired())
                {
                        auto object = weakObject.lock();
                        object->getMeshAnimation(3).play(1U);
                        object->getMeshAnimation(4).stop();
                }
        }

        //--------------------------------------------------------------------------------------------------------------
        void SimpleAnimation::onButtonMessageLookRight()
        {
                auto weakObject = scene_.getActor("object");
                if(!weakObject.expired())
                {
                        auto object = weakObject.lock();
                        object->getMeshAnimation(3).stop();
                        object->getMeshAnimation(4).play(1U);
                }
        }

        //--------------------------------------------------------------------------------------------------------------
        void SimpleAnimation::onButtonMessageToggleSsao()
        {
                toggleEffect(Renderer::Effects::SSAO);
        }

        //--------------------------------------------------------------------------------------------------------------
        void SimpleAnimation::onButtonMessageToggleBloom()
        {
                toggleEffect(Renderer::Effects::BLOOM);
        }

        //--------------------------------------------------------------------------------------------------------------
        void SimpleAnimation::onButtonMessageToggleShadows()
        {
                toggleEffect(Renderer::Effects::SHADOWS);
        }

        //--------------------------------------------------------------------------------------------------------------
        void SimpleAnimation::onButtonMessageToggleSettings()
        {
                toggleGuiElementVisibility(buttonToggleSsao_);
                toggleGuiElementVisibility(buttonToggleBloom_);
                toggleGuiElementVisibility(buttonToggleShadows_);

                isSettingsMenuVisible_ = !isSettingsMenuVisible_;

                auto buttonToggleSettings = buttonToggleSettings_.lock();
                if(buttonToggleSettings)
                {
                        if(isSettingsMenuVisible_)
                                buttonToggleSettings->setText("[-]Effects");
                        else
                                buttonToggleSettings->setText("[+]Effects");
                }
        }

}
