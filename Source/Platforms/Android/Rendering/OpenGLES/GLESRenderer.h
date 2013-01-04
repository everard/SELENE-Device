// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GLES_RENDERER_H
#define GLES_RENDERER_H

#include "../../../../Engine/Rendering/Renderer.h"
#include "../../../../Engine/Core/Resources/Mesh/Mesh.h"
#include "../../../../Engine/Core/Status/Status.h"

#include <EGL/egl.h>
#include <GLES/gl.h>

namespace selene
{

        // Represents OpenGL ES renderer.
        class GlesRenderer: public Renderer, public Status
        {
        public:
                GlesRenderer();
                ~GlesRenderer();

                // Initializes renderer
                bool initialize(const Renderer::Parameters& parameters);

                // Destroys renderer
                void destroy();

                // Renders scene
                void render(const Camera& camera);

        private:
                Matrix viewProjectionMatrix_;
                Matrix projectionMatrix_;
                Matrix viewInvMatrix_;
                Matrix normalsMatrix_;
                Matrix viewMatrix_;

                Vector4d projectionParameters_;
                Vector4d shadowMapKernelSize_;
                Vector4d unprojectionVector_;
                Vector4d screenSize_;

                Parameters parameters_;

                EGLSurface surface_;
                EGLContext context_;
                EGLDisplay display_;

                // Writes log entry
                void writeLogEntry(const char* entry);

                // Initializes context
                bool initializeContext();

        };

}

#endif
