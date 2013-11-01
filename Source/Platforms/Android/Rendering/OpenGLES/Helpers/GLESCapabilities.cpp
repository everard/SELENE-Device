// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLESCapabilities.h"

#include "../../../Application/AndroidApplication.h"
#include "../../../Platform.h"

namespace selene
{

        GlesCapabilities::GlesCapabilities():
                surface_(EGL_NO_SURFACE), context_(EGL_NO_CONTEXT),
                display_(EGL_NO_DISPLAY), surfaceWidth_(0), surfaceHeight_(0) {}
        GlesCapabilities::~GlesCapabilities()
        {
                destroyContext();
        }

        //-------------------------------------------------------------------------------------------------------
        bool GlesCapabilities::createCompatibleContext(const Renderer::Parameters& parameters,
                                                       Renderer::EffectsList& effectsList)
        {
                destroyContext();

                AndroidApplication* application = dynamic_cast<AndroidApplication*>(parameters.getApplication());
                if(application == nullptr)
                        return false;

                const EGLint attributes[] =
                {
                        EGL_BLUE_SIZE,  8,
                        EGL_GREEN_SIZE, 8,
                        EGL_RED_SIZE,   8,
                        EGL_ALPHA_SIZE, 8,
                        EGL_DEPTH_SIZE, 16,
                        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                        EGL_NONE
                };

                const EGLint attributesList[] =
                {
                        EGL_CONTEXT_CLIENT_VERSION, 2,
                        EGL_NONE
                };

                android_app* state = application->getHandle();
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

                eglQuerySurface(display_, surface_, EGL_WIDTH,  &surfaceWidth_);
                eglQuerySurface(display_, surface_, EGL_HEIGHT, &surfaceHeight_);

                try
                {
                        Effect bloom =
                        {
                                "Bloom", 1,
                                {
                                        {"Luminance", 0.0f, 1.0f, 0.08f},
                                        {"Scale", 0.0f, std::numeric_limits<float>::max(), 1.5f}
                                }
                        };

                        Effect shadows =
                        {
                                "Shadows", 1, {}
                        };

                        effectsList.push_back(bloom);
                        effectsList.push_back(shadows);
                }
                catch(...)
                {
                        return false;
                }

                return true;
        }

        //-------------------------------------------------------------------------------------------------------
        void GlesCapabilities::destroyContext()
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

        //-------------------------------------------------------------------------------------------------------
        EGLSurface GlesCapabilities::getSurface()
        {
                return surface_;
        }

        //-------------------------------------------------------------------------------------------------------
        EGLContext GlesCapabilities::getContext()
        {
                return context_;
        }

        //-------------------------------------------------------------------------------------------------------
        EGLDisplay GlesCapabilities::getDisplay()
        {
                return display_;
        }

        //-------------------------------------------------------------------------------------------------------
        EGLint GlesCapabilities::getSurfaceWidth()
        {
                return surfaceWidth_;
        }

        //-------------------------------------------------------------------------------------------------------
        EGLint GlesCapabilities::getSurfaceHeight()
        {
                return surfaceHeight_;
        }

}
