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

                if(!Platform::createCompatibleContext(surface_, context_, display_,
                                                      surfaceWidth_, surfaceHeight_))
                        return false;

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
                Platform::destroyContext(surface_, context_, display_);
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
