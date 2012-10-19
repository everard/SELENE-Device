#include "SimpleApplication.h"

namespace selene
{

        // Entry point
        Application* Platform::createApplication()
        {
                return new(std::nothrow) SimpleApplication("SELENE Device", 640, 480);
        }

        SimpleApplication::SimpleApplication(const char* name, uint32_t width, uint32_t height): WindowsApplication(name, width, height),
                                                                                                 fileManager_(Platform::fileExists),
                                                                                                 camera_("Scene camera")
        {
                // specify search folders for file manager
                const char* folders[] =
                {
                        "..//..//..//..//ApplicationData//",
                        "..//..//..//..//ApplicationData//Meshes//",
                        "..//..//..//..//ApplicationData//Textures//",

                        "..//..//..//..//..//ApplicationData//",
                        "..//..//..//..//..//ApplicationData//Meshes//",
                        "..//..//..//..//..//ApplicationData//Textures//"
                };
                const uint32_t numFolders = sizeof(folders) / sizeof(folders[0]);

                for(uint32_t i = 0; i < numFolders; ++i)
                        fileManager_.addFolder(folders[i]);

                isGuiHidden_ = false;
                labelId_ = textBoxId_ = -1;
        }
        SimpleApplication::~SimpleApplication() {}

        //-----------------------------------------------------------------------------
        bool SimpleApplication::onInitialize()
        {
                // initialize renderer
                uint8_t d3dFlags = 0;
                Renderer::Parameters parameters(this, &fileManager_, width_, height_, &std::cout, d3dFlags);

                if(!renderer_.initialize(parameters))
                        return false;

                // set GUI to render
                renderer_.setGui(gui_);

                // set camera for scene
                scene_.setCamera(&camera_);

                // set camera parameters
                camera_.setPerspective(Vector4d(45.0f,
                                                static_cast<float>(height_) / static_cast<float>(width_),
                                                1.0f,
                                                1000.0f));
                camera_.setPosition(Vector3d());
                camera_.setDirection(Vector3d(0.0f, -0.5f, -1.0f));
                camera_.setDistance(15.0f);

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

                Vector4d TextBoxBackgroundColors[NUM_OF_GUI_ELEMENT_COLOR_TYPES] =
                {
                        Vector4d(0.4f, 0.4f, 0.1f, 0.7f),
                        Vector4d(0.1f, 0.4f, 0.1f, 0.9f),
                        Vector4d(0.4f, 0.1f, 0.1f, 0.9f)
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
                                                         Vector2d(0.1f, 0.75f),
                                                         Vector2d(0.2f, 0.1f),
                                                         "Exit"));

                gui_.addElement(new(std::nothrow) Button(std::function<void (int32_t, uint8_t)>([this] (int32_t elementId, uint8_t message)
                                                         {
                                                                 this->onButtonMessageCopy(elementId, message);
                                                         }),
                                                         buttonBackgroundColors,
                                                         textColors,
                                                         Vector2d(0.05f, 0.07f),
                                                         Vector2d(0.1f, 0.63f),
                                                         Vector2d(0.2f, 0.1f),
                                                         "Copy"));

                gui_.addElement(new(std::nothrow) Label(std::function<void (int32_t, uint8_t)>(),
                                                        labelBackgroundColors,
                                                        textColors,
                                                        Vector2d(0.05f, 0.07f),
                                                        Vector2d(0.1f, 0.1f),
                                                        Vector2d(1.6f, 0.1f),
                                                        "Press ESC to show or hide menu."));

                gui_.addElement(new(std::nothrow) Label(std::function<void (int32_t, uint8_t)>(),
                                                        labelBackgroundColors,
                                                        textColors,
                                                        Vector2d(0.05f, 0.07f),
                                                        Vector2d(0.1f, 0.2f),
                                                        Vector2d(1.6f, 0.1f),
                                                        "Use WASD to move the mesh."));

                textBoxId_ =
                        gui_.addElement(new(std::nothrow) TextBox(std::function<void (int32_t, uint8_t)>(),
                                                                  TextBoxBackgroundColors,
                                                                  textColors,
                                                                  Vector2d(0.05f, 0.07f),
                                                                  Vector2d(0.1f, 0.5f),
                                                                  Vector2d(1.6f, 0.1f),
                                                                  "This text may be altered."));

                labelId_ = gui_.addElement(new(std::nothrow) Label(std::function<void (int32_t, uint8_t)>(),
                                                                   labelBackgroundColors,
                                                                   textColors,
                                                                   Vector2d(0.05f, 0.07f),
                                                                   Vector2d(0.1f, 0.3f),
                                                                   Vector2d(1.6f, 0.1f),
                                                                   "Press copy button to copy text here."));

                // load mesh
                MeshFactory<D3d9Mesh> d3d9MeshFactory(&fileManager_);
                TextureFactory<D3d9Texture> d3d9TextureFactory(&fileManager_);

                d3d9MeshFactory.setResourceFactory(&d3d9TextureFactory);
                d3d9MeshFactory.setResourceManager(&textureManager_);

                std::cout << "Loading mesh...";
                if(meshManager_.createResource("object.sle", d3d9MeshFactory) != SUCCESS)
                        std::cout << "FAILED";
                else
                        std::cout << "SUCCEEDED";

                // create scene objects
                scene_.addNode(new(std::nothrow) Actor("object",
                                                       meshManager_.requestResource<Mesh>("object.sle")));

                scene_.addNode(new(std::nothrow) SpotLight("light",
                                                           Vector3d(0.0f, 10.f, 0.0f),
                                                           Vector3d(0.0f, -20.0f, 0.0f),
                                                           Vector3d(1.0f, 1.0f, 1.0f),
                                                           1.0f, 30.0f));

                return true;
        }

        //-----------------------------------------------------------------------------
        void SimpleApplication::onDestroy()
        {
                textureManager_.destroyResources(true);
                meshManager_.destroyResources(true);
                renderer_.destroy();
        }

        //-----------------------------------------------------------------------------
        void SimpleApplication::onKeyPress(uint8_t key)
        {
                if(key == VK_ESCAPE)
                {
                        if(isGuiHidden_)
                        {
                                gui_.clearFlags(GUI_HIDDEN);
                                isGuiHidden_ = false;
                        }
                        else
                        {
                                gui_.setFlags(GUI_HIDDEN);
                                isGuiHidden_ = true;
                        }
                }

                if(!isGuiHidden_)
                        gui_.process(cursorPosition_, pressedControlButtons_, key);
        }

        //-----------------------------------------------------------------------------
        void SimpleApplication::onUpdate(float elapsedTime)
        {
                if(isGuiHidden_)
                {
                        // rotate if key is pressed
                        auto weakPointer = scene_.getActor("object");
                        auto actor = weakPointer.lock();
                        if(actor)
                        {
                                if(getKeyState('W') > 0.0f)
                                {
                                        Quaternion rotation(Vector3d(1.0f, 0.0f, 0.0f) * sin(elapsedTime * 0.25f), cos(elapsedTime * 0.25f));
                                        actor->setRotation(actor->getRotation() * rotation);
                                }

                                if(getKeyState('S') > 0.0f)
                                {
                                        Quaternion rotation(Vector3d(-1.0f, 0.0f, 0.0f) * sin(elapsedTime * 0.25f), cos(elapsedTime * 0.25f));
                                        actor->setRotation(actor->getRotation() * rotation);
                                }

                                if(getKeyState('A') > 0.0f)
                                {
                                        Quaternion rotation(Vector3d(0.0f, 1.0f, 0.0f) * sin(elapsedTime * 0.25f), cos(elapsedTime * 0.25f));
                                        actor->setRotation(actor->getRotation() * rotation);
                                }

                                if(getKeyState('D') > 0.0f)
                                {
                                        Quaternion rotation(Vector3d(0.0f, -1.0f, 0.0f) * sin(elapsedTime * 0.25f), cos(elapsedTime * 0.25f));
                                        actor->setRotation(actor->getRotation() * rotation);
                                }
                        }
                }
                else
                {
                        // process GUI
                        gui_.process(cursorPosition_, pressedControlButtons_, 0);
                }
        }

        //-----------------------------------------------------------------------------
        void SimpleApplication::onRender(float elapsedTime)
        {
                // render scene
                scene_.updateAndRender(elapsedTime, renderer_);
        }

        //-----------------------------------------------------------------------------
        void SimpleApplication::onButtonMessageExit(int32_t elementId, uint8_t message)
        {
                // to prevent compilation warning we should use all parameters
                elementId = message = 0;

                // halt application
                halt();
        }

        //-----------------------------------------------------------------------------
        void SimpleApplication::onButtonMessageCopy(int32_t elementId, uint8_t message)
        {
                // to prevent compilation warning we should use all parameters
                elementId = message = 0;

                auto weakLabel   = gui_.getElement(labelId_);
                auto weakTextBox = gui_.getElement(textBoxId_);
                if(weakLabel.expired() || weakTextBox.expired())
                        return;

                auto label   = weakLabel.lock();
                auto textBox = weakTextBox.lock();

                label->setText(textBox->getText().c_str());
        }

}
