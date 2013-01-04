// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLESRenderer.h"

#include "../../../../Engine/Scene/Nodes/ParticleSystem.h"
#include "../../../../Engine/Scene/Nodes/Camera.h"
#include "../../../../Engine/Scene/Nodes/Actor.h"
#include "../../Application/AndroidApplication.h"
#include "../../../../Engine/GUI/GUI.h"

#include "Resources/GLESTexture.h"
#include "Resources/GLESMesh.h"

namespace selene
{

        GlesRenderer::GlesRenderer(): parameters_(nullptr, nullptr, 0, 0, nullptr, 0)
        {
                surface_ = EGL_NO_SURFACE;
                context_ = EGL_NO_CONTEXT;
                display_ = EGL_NO_DISPLAY;
        }
        GlesRenderer::~GlesRenderer()
        {
                destroy();
        }

        //-------------------------------------------------------------------
        bool GlesRenderer::initialize(const Renderer::Parameters& parameters)
        {
                destroy();

                parameters_ = parameters;

                writeLogEntry("--- Initializing OpenGL ES renderer ---");

                if(!initializeContext())
                {
                        writeLogEntry("ERROR: Context initialization failed.");
                        return false;
                }

                return true;
        }

        //-------------------------------------------------------------------
        void GlesRenderer::destroy()
        {
                if(display_ != EGL_NO_DISPLAY)
                {
                        eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
                        eglDestroyContext(display_, context_);
                        eglDestroySurface(display_, surface_);
                        eglTerminate(display_);
                }
        }

        //-------------------------------------------------------------------
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

                glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                eglSwapBuffers(display_, surface_);
        }

        //-------------------------------------------------------------------
        void GlesRenderer::writeLogEntry(const char* entry)
        {
                if(parameters_.getLog() != nullptr)
                        (*parameters_.getLog()) << entry << std::endl;
        }

        //-------------------------------------------------------------------
        bool GlesRenderer::initializeContext()
        {
                AndroidApplication* application = dynamic_cast<AndroidApplication*>(parameters_.getApplication());
                if(application == nullptr)
                        return false;

                android_app* state = application->getState();
                if(state == nullptr)
                        return false;

                const EGLint attribs[] =
                {
                        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                        EGL_BLUE_SIZE, 8,
                        EGL_GREEN_SIZE, 8,
                        EGL_RED_SIZE, 8,
                        EGL_NONE
                };

                EGLConfig config;
                EGLint numConfigs;

                display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
                if(display_ == EGL_NO_DISPLAY)
                        return false;

                if(!eglInitialize(display_, 0, 0))
                        return false;

                if(!eglChooseConfig(display_, attribs, &config, 1, &numConfigs))
                        return false;

                EGLint format;
                if(!eglGetConfigAttrib(display_, config, EGL_NATIVE_VISUAL_ID, &format))
                        return false;

                ANativeWindow_setBuffersGeometry(state->window, 0, 0, format);

                surface_ = eglCreateWindowSurface(display_, config, state->window, nullptr);
                if(!surface_)
                        return false;

                context_ = eglCreateContext(display_, config, nullptr, nullptr);
                if(!context_)
                        return false;

                if(eglMakeCurrent(display_, surface_, surface_, context_) == EGL_FALSE)
                        return false;

                // prepare OpenGL ES
                glDisable(GL_DITHER);
                glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
                glEnable(GL_CULL_FACE);
                glEnable(GL_DEPTH_TEST);

                glViewport(0, 0, parameters_.getWidth(), parameters_.getHeight());

                return true;
        }

}
