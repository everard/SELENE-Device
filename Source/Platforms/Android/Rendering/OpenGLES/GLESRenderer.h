// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GLES_RENDERER_H
#define GLES_RENDERER_H

#include "../../../../Engine/Rendering/Renderer.h"
#include "../../../../Engine/Core/Resources/Mesh/Mesh.h"
#include "../../../../Engine/Core/Status/Status.h"
#include "Helpers/GLESGUIRenderer.h"

#include <GLES2/gl2.h>
#include <EGL/egl.h>

namespace selene
{

        // Represents OpenGL ES renderer.
        class GlesRenderer: public Renderer, public Status
        {
        public:
                // Initializes renderer
                bool initialize(const Renderer::Parameters& parameters);

                // Destroys renderer
                void destroy();

                // Renders scene
                void render(const Camera& camera);

                // Checks OpenGL ES error
                static void checkGlesError(const char* operation);

        private:
                friend class AndroidApplication;

                // Capabilities
                enum
                {
                        NUM_OF_VERTEX_SHADER_UNIFORMS_REQUIRED = 70,
                        NUM_OF_VERTEX_ATTRIBUTES_REQUIRED = 6
                };

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

                GlesGuiRenderer guiRenderer_;

                GlesRenderer();
                GlesRenderer(const GlesRenderer& renderer);
                ~GlesRenderer();

                // Writes log entry
                void writeLogEntry(const char* entry);

                // Initializes context and retains all resources
                bool retain();

                // Discards context and all resources
                void discard();

                // Retains all helpers
                bool retainHelpers();

                // Discards all helpers
                void discardHelpers();

                // Initializes helpers
                bool initializeHelpers();

                // Destroys helpers
                void destroyHelpers();

                // Initializes OpenGL ES context
                bool initializeGlesContext();

                // Destroys OpenGL ES context
                void destroyGlesContext();

        };

}

#endif
