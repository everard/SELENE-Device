// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLESRenderer.h"

#include "../../../../Engine/Scene/Nodes/ParticleSystem.h"
#include "../../../../Engine/Scene/Nodes/Camera.h"
#include "../../../../Engine/Scene/Nodes/Actor.h"
#include "../../Application/AndroidApplication.h"
#include "../../../../Engine/GUI/GUI.h"
#include "../../Platform.h"

#include "Resources/GLESTexture.h"
#include "Resources/GLESMesh.h"

namespace selene
{

        //--------------------------------------------------------------------------------------------
        bool GlesRenderer::initialize(const Renderer::Parameters& parameters)
        {
                parameters_ = parameters;
                writeLogEntry("--- Initializing OpenGL ES renderer ---");

                // check capabilities
                GLint maxNumVertexAttributes = 0;
                glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxNumVertexAttributes);
                if(maxNumVertexAttributes < NUM_OF_VERTEX_ATTRIBUTES_REQUIRED)
                {
                        writeLogEntry("ERROR: Not enough vertex attributes.");
                        return false;
                }

                GLint maxNumUniforms = 0;
                glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxNumUniforms);
                if(maxNumUniforms < NUM_OF_VERTEX_SHADER_UNIFORMS_REQUIRED)
                {
                        writeLogEntry("ERROR: Not enough vertex uniforms.");
                        return false;
                }

                return initializeHelpers();
        }

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::destroy()
        {
                destroyHelpers();
                destroyGlesContext();
        }

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::render(const Camera& camera)
        {
                // get matrices
                viewProjectionMatrix_ = camera.getViewProjectionMatrix();
                projectionMatrix_ = camera.getProjectionMatrix();
                viewMatrix_ = camera.getViewMatrix();

                normalsMatrix_ = viewMatrix_;
                normalsMatrix_.invert();
                viewInvMatrix_ = normalsMatrix_;
                normalsMatrix_.transpose();

                // get vectors
                const Matrix& projectionInvMatrix = camera.getProjectionInvMatrix();
                projectionParameters_ = camera.getProjectionParameters();
                unprojectionVector_.define(projectionInvMatrix.a[0][0],
                                           projectionInvMatrix.a[1][1],
                                           1.0, 0.0);

                glViewport(0, 0, parameters_.getWidth(), parameters_.getHeight());

                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // render GUI
                Gui* gui = camera.getGui();
                guiRenderer_.render(gui);

                eglSwapBuffers(display_, surface_);
        }

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::checkGlesError(const char* operation)
        {
                for(GLint error = glGetError(); error; error = glGetError())
                {
                        LOGI("after %s() glError (0x%x)\n", operation, error);
                }
        }

        GlesRenderer::GlesRenderer(): parameters_(nullptr, nullptr, 0, 0, nullptr, 0)
        {
                surface_ = EGL_NO_SURFACE;
                context_ = EGL_NO_CONTEXT;
                display_ = EGL_NO_DISPLAY;
        }
        GlesRenderer::GlesRenderer(const GlesRenderer&): Renderer(), Status(),
                                                         parameters_(nullptr, nullptr, 0, 0, nullptr, 0)
        {
                surface_ = EGL_NO_SURFACE;
                context_ = EGL_NO_CONTEXT;
                display_ = EGL_NO_DISPLAY;
        }
        GlesRenderer::~GlesRenderer()
        {
                destroy();
        }

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::writeLogEntry(const char* entry)
        {
                LOGI("%s", entry);
                if(parameters_.getLog() != nullptr)
                        (*parameters_.getLog()) << entry << std::endl;
        }

        //--------------------------------------------------------------------------------------------
        bool GlesRenderer::retain()
        {
                if(!initializeGlesContext())
                        return false;

                if(!retainHelpers())
                        return false;

                if(!ResourceManager::retainResources())
                        return false;

                return true;
        }

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::discard()
        {
                ResourceManager::discardResources();
                discardHelpers();
                destroyGlesContext();
        }

        //--------------------------------------------------------------------------------------------
        bool GlesRenderer::retainHelpers()
        {
                if(!guiRenderer_.retain())
                        return false;

                return true;
        }

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::discardHelpers()
        {
                guiRenderer_.discard();
        }

        //--------------------------------------------------------------------------------------------
        bool GlesRenderer::initializeHelpers()
        {
                destroyHelpers();

                if(!guiRenderer_.initialize(parameters_.getFileManager()))
                {
                        writeLogEntry("ERROR: Could not initialize GUI renderer.");
                        return false;
                }

                // prepare OpenGL ES
                glEnable(GL_CULL_FACE);
                glEnable(GL_DEPTH_TEST);
                glEnable(GL_TEXTURE_2D);

                return true;
        }

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::destroyHelpers()
        {
                guiRenderer_.destroy();
        }

        //--------------------------------------------------------------------------------------------
        bool GlesRenderer::initializeGlesContext()
        {
                const EGLint attributes[] =
                {
                        EGL_BLUE_SIZE, 8,
                        EGL_GREEN_SIZE, 8,
                        EGL_RED_SIZE, 8,
                        EGL_ALPHA_SIZE, 8,
                        EGL_DEPTH_SIZE, 24,
                        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                        EGL_NONE
                };

                const EGLint attributesList[] =
                {
                        EGL_CONTEXT_CLIENT_VERSION, 2,
                        EGL_NONE
                };

                android_app* state = Platform::state_;
                if(state == nullptr)
                        return false;

                EGLConfig config;
                EGLint numConfigs;

                display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
                if(display_ == EGL_NO_DISPLAY)
                        return false;

                if(!eglInitialize(display_, 0, 0))
                        return false;

                if(!eglChooseConfig(display_, attributes, &config, 1, &numConfigs))
                        return false;

                EGLint format;
                if(!eglGetConfigAttrib(display_, config, EGL_NATIVE_VISUAL_ID, &format))
                        return false;

                ANativeWindow_setBuffersGeometry(state->window, 0, 0, format);

                surface_ = eglCreateWindowSurface(display_, config, state->window, nullptr);
                if(surface_ == EGL_NO_SURFACE)
                        return false;

                context_ = eglCreateContext(display_, config, nullptr, attributesList);
                if(context_ == EGL_NO_CONTEXT)
                        return false;

                if(eglMakeCurrent(display_, surface_, surface_, context_) == EGL_FALSE)
                        return false;

                return true;
        }

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::destroyGlesContext()
        {
                if(display_ != EGL_NO_DISPLAY)
                {
                        eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

                        if(context_ != EGL_NO_CONTEXT)
                        {
                                eglDestroyContext(display_, context_);
                                context_ = EGL_NO_CONTEXT;
                        }

                        if(surface_ != EGL_NO_SURFACE)
                        {
                                eglDestroySurface(display_, surface_);
                                surface_ = EGL_NO_SURFACE;
                        }

                        eglTerminate(display_);
                        display_ = EGL_NO_DISPLAY;
                }
        }

}
