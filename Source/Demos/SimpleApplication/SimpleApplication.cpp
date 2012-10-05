#include "SimpleApplication.h"
#include <io.h>

namespace selene
{

        // Checks file existance
        bool fileExists(const char* name)
        {
                return (_access(name, 0) == 0);
        }

        SimpleApplication::SimpleApplication(const char* name, const char* windowClassName):
                WindowsApplication(name, windowClassName), fileManager_(fileExists), camera_("Scene camera")
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
        }
        SimpleApplication::~SimpleApplication() {}

        //-----------------------------------------------------------------------------
        bool SimpleApplication::onInitialize()
        {
                // initialize renderer
                std::cout << "Initialising D3D9 renderer\n";

                uint8_t d3dFlags = 0;
                D3d9Renderer::Parameters parameters(&fileManager_, width_, height_, hWnd_, d3dFlags);

                if(!renderer_.initialize(parameters))
                {
                        std::cout<<"ERROR\n";
                        return false;
                }

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
                Vector4d backgroundColors[NUM_OF_GUI_ELEMENT_COLOR_TYPES] =
                {
                        Vector4d(0.1f, 0.1f, 0.1f, 0.7f),
                        Vector4d(0.1f, 0.1f, 0.4f, 0.9f),
                        Vector4d(0.3f, 0.1f, 0.1f, 0.9f)
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
                                                         backgroundColors,
                                                         textColors,
                                                         Vector2d(0.05f, 0.07f),
                                                         Vector2d(0.1f, 0.43f),
                                                         Vector2d(0.6f, 0.1f),
                                                         "Exit"));

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
        void SimpleApplication::onRender(float elapsedTime)
        {
                // rotate actor
                auto weakPointer = scene_.getActor("object");
                auto actor = weakPointer.lock();
                if(actor)
                {
                        Quaternion rotation(Vector3d(1.0f, -1.0f, 1.0f) * sin(elapsedTime * 0.25f), cos(elapsedTime * 0.25f));
                        actor->setRotation(actor->getRotation() * rotation);
                }

                // process GUI
                gui_.process(cursorPosition_, pressedControlButtons_, 0);

                // render scene
                scene_.updateAndRender(elapsedTime, renderer_);
        }

        //-----------------------------------------------------------------------------
        void SimpleApplication::onButtonMessageExit(int32_t elementId, uint8_t message)
        {
                // to prevent compilation warning we should use all parameters
                elementId = message = 0;

                // send WM_DESTROY to the window
                SendMessage(hWnd_, WM_DESTROY, 0, 0);
        }

}
