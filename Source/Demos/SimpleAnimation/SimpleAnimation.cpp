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

        SimpleAnimation::SimpleAnimation(const char* name, uint32_t width, uint32_t height): Platform::Application(name, width, height)
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

                isCameraRotationEnabled_ = false;
        }
        SimpleAnimation::~SimpleAnimation() {}

        //--------------------------------------------------------------
        bool SimpleAnimation::onInitialize()
        {
                // initialize memory buffer of the renderer
                if(!Renderer::initializeMemoryBuffer(1024 * 20))
                        return false;

                // initialize renderer
                uint8_t flags = 0;
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

                gui_.addElement(new(std::nothrow) Button(std::function<void (int32_t, uint8_t)>([this] (int32_t elementId, uint8_t message)
                                                         {
                                                                 this->onButtonMessageExit(elementId, message);
                                                         }),
                                                         buttonBackgroundColors,
                                                         textColors,
                                                         Vector2d(0.05f, 0.07f),
                                                         Vector2d(1.4f, 0.5f),
                                                         Vector2d(0.5f, 0.1f),
                                                         "Exit"));

                gui_.addElement(new(std::nothrow) Button(std::function<void (int32_t, uint8_t)>([this] (int32_t elementId, uint8_t message)
                                                         {
                                                                 this->onButtonMessageWalk(elementId, message);
                                                         }),
                                                         buttonBackgroundColors,
                                                         textColors,
                                                         Vector2d(0.05f, 0.07f),
                                                         Vector2d(0.1f, 0.50f),
                                                         Vector2d(0.5f, 0.1f),
                                                         "Walk"));

                gui_.addElement(new(std::nothrow) Button(std::function<void (int32_t, uint8_t)>([this] (int32_t elementId, uint8_t message)
                                                         {
                                                                 this->onButtonMessageShoot(elementId, message);
                                                         }),
                                                         buttonBackgroundColors,
                                                         textColors,
                                                         Vector2d(0.05f, 0.07f),
                                                         Vector2d(0.1f, 0.63f),
                                                         Vector2d(0.5f, 0.1f),
                                                         "Shoot"));

                gui_.addElement(new(std::nothrow) Button(std::function<void (int32_t, uint8_t)>([this] (int32_t elementId, uint8_t message)
                                                         {
                                                                 this->onButtonMessageLookLeft(elementId, message);
                                                         }),
                                                         buttonBackgroundColors,
                                                         textColors,
                                                         Vector2d(0.05f, 0.07f),
                                                         Vector2d(0.1f, 0.76f),
                                                         Vector2d(0.5f, 0.1f),
                                                         "Look left"));

                gui_.addElement(new(std::nothrow) Button(std::function<void (int32_t, uint8_t)>([this] (int32_t elementId, uint8_t message)
                                                         {
                                                                 this->onButtonMessageLookRight(elementId, message);
                                                         }),
                                                         buttonBackgroundColors,
                                                         textColors,
                                                         Vector2d(0.05f, 0.07f),
                                                         Vector2d(0.1f, 0.89f),
                                                         Vector2d(0.5f, 0.1f),
                                                         "Look right"));

                gui_.addElement(new(std::nothrow) Label(std::function<void (int32_t, uint8_t)>(),
                                                        labelBackgroundColors,
                                                        textColors,
                                                        Vector2d(0.05f, 0.07f),
                                                        Vector2d(0.1f, 0.1f),
                                                        Vector2d(1.6f, 0.1f),
                                                        "Use controller to rotate camera."));

                // load mesh
                MeshFactory<Platform::Mesh> meshFactory(&fileManager_);
                TextureFactory<Platform::Texture> textureFactory(&fileManager_);
                MeshAnimationFactory<MeshAnimation> meshAnimationFactory(&fileManager_);

                meshFactory.setResourceFactory(&textureFactory);
                meshFactory.setResourceManager(&textureManager_);

                std::cout << "Loading mesh...";
                if(meshManager_.createResource("girl.sle", meshFactory) != SUCCESS)
                        std::cout << "FAILED" << std::endl;
                else
                        std::cout << "SUCCEEDED" << std::endl;

                const char* meshAnimations[] =
                {
                        "stand.sla", "walk.sla", "shoot.sla", "lookleft.sla", "lookright.sla"
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
                                                       meshManager_.requestResource<Mesh>("girl.sle"),
                                                       Vector3d(0.0f, -5.0f),
                                                       Quaternion(),
                                                       Vector3d(0.08f, 0.08f, 0.08f)));

                scene_.addNode(new(std::nothrow) DirectionalLight("directional light",
                                                                  Vector3d(-1.0f, 0.0f, 0.0f),
                                                                  Vector3d( 1.0f, 1.0f, 0.0f)));

                scene_.addNode(new(std::nothrow) SpotLight("spot light",
                                                           Vector3d(0.0f, 10.f, 0.0f),
                                                           Vector3d(0.0f, -20.0f, 0.0f),
                                                           Vector3d(1.0f, 1.0f, 1.0f),
                                                           1.0f, 30.0f));

                scene_.addNode(new(std::nothrow) PointLight("point light",
                                                            Vector3d(-5.0f, 5.0f, -4.0f),
                                                            Vector3d(0.3f, 0.3f, 0.8f),
                                                            1.0f, 30.0f));

                scene_.addNode(new(std::nothrow) Camera("Camera",
                                                        Vector3d(),
                                                        Vector3d(0.0f, -0.5f, 1.0f),
                                                        Vector3d(0.0f, 1.0f),
                                                        Vector4d(45.0f,
                                                                 static_cast<float>(height_) / static_cast<float>(width_),
                                                                 1.0f,
                                                                 1000.0f),
                                                        15.0f,
                                                        &gui_));
                camera_ = scene_.getCamera("Camera");

                auto weakObject = scene_.getActor("object");
                if(!weakObject.expired())
                {
                        auto object = weakObject.lock();

                        // animations must be added to the actor

                        // first animation (index is zero)
                        object->addMeshAnimation(meshAnimationManager_.requestResource<MeshAnimation>("stand.sla"),
                                                 0.0f, 0.5f, 0.5f, 15.0f, 1.0f);

                        // second animation (index is one)
                        object->addMeshAnimation(meshAnimationManager_.requestResource<MeshAnimation>("walk.sla"),
                                                 0.0f, 0.5f, 0.5f, 1.0f, 1.0f);

                        // third animation (index is two)
                        object->addMeshAnimation(meshAnimationManager_.requestResource<MeshAnimation>("shoot.sla"),
                                                 0.0f, 0.1f, 0.25f, 0.5f, 1.0f);

                        // etc...
                        object->addMeshAnimation(meshAnimationManager_.requestResource<MeshAnimation>("lookleft.sla"),
                                                 0.0f, 0.5f, 0.5f, 1.0f, 1.0f);
                        object->addMeshAnimation(meshAnimationManager_.requestResource<MeshAnimation>("lookright.sla"),
                                                 0.0f, 0.5f, 0.5f, 1.0f, 1.0f);

                        // play the first animation (stand)
                        object->getMeshAnimation(0).play();
                }

                return true;
        }

        //--------------------------------------------------------------
        void SimpleAnimation::onDestroy()
        {
                Renderer::destroyMemoryBuffer();

                meshAnimationManager_.destroyResources(true);
                textureManager_.destroyResources(true);
                meshManager_.destroyResources(true);
        }

        //--------------------------------------------------------------
        void SimpleAnimation::onKeyPress(uint8_t key)
        {
                gui_.process(cursorPosition_, pressedControlButtons_, key);
        }

        //--------------------------------------------------------------
        void SimpleAnimation::onControlButtonPress(uint8_t button)
        {
                if(IS_SET(button, CONTROL_BUTTON_0))
                        isCameraRotationEnabled_ = true;
        }

        //--------------------------------------------------------------
        void SimpleAnimation::onControlButtonRelease(uint8_t button)
        {
                if(IS_SET(button, CONTROL_BUTTON_0))
                        isCameraRotationEnabled_ = false;
        }

        //--------------------------------------------------------------
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

        //--------------------------------------------------------------
        void SimpleAnimation::onRender(float elapsedTime)
        {
                // render scene
                scene_.updateAndRender(elapsedTime, renderer_);
        }

        //--------------------------------------------------------------
        void SimpleAnimation::onButtonMessageExit(int32_t, uint8_t)
        {
                // halt application
                halt();
        }

        //--------------------------------------------------------------
        void SimpleAnimation::onButtonMessageWalk(int32_t, uint8_t)
        {
                auto weakObject = scene_.getActor("object");
                if(!weakObject.expired())
                {
                        auto object = weakObject.lock();
                        object->getMeshAnimation(1).play(3U);
                }
        }

        //--------------------------------------------------------------
        void SimpleAnimation::onButtonMessageShoot(int32_t, uint8_t)
        {
                auto weakObject = scene_.getActor("object");
                if(!weakObject.expired())
                {
                        auto object = weakObject.lock();
                        object->getMeshAnimation(2).play(1U);
                }
        }

        //--------------------------------------------------------------
        void SimpleAnimation::onButtonMessageLookLeft(int32_t, uint8_t)
        {
                auto weakObject = scene_.getActor("object");
                if(!weakObject.expired())
                {
                        auto object = weakObject.lock();
                        object->getMeshAnimation(3).play(1U);
                        object->getMeshAnimation(4).stop();
                }
        }

        //--------------------------------------------------------------
        void SimpleAnimation::onButtonMessageLookRight(int32_t, uint8_t)
        {
                auto weakObject = scene_.getActor("object");
                if(!weakObject.expired())
                {
                        auto object = weakObject.lock();
                        object->getMeshAnimation(3).stop();
                        object->getMeshAnimation(4).play(1U);
                }
        }

}
