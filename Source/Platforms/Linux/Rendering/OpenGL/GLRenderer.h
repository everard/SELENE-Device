// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GL_RENDERER_H
#define GL_RENDERER_H

#include "../../../../Engine/Rendering/Renderer.h"
#include "../../../../Engine/Core/Resources/Mesh/Mesh.h"
#include "../../../../Engine/Core/Status/Status.h"

namespace selene
{

        enum
        {
                // Rendering flags
                RENDERING_SSAO_ENABLED  = 0x01,
                RENDERING_BLOOM_ENABLED = 0x02,
                RENDERING_FULL_SCREEN_ENABLED = 0x04
        };

        // Represents OpenGL renderer.
        class GlRenderer: public Renderer, public Status
        {
        public:
                GlRenderer();
                ~GlRenderer();

                // Initializes renderer
                bool initialize(const Renderer::Parameters& parameters);

                // Destroys renderer
                void destroy();

                // Renders scene
                void render(const Camera& camera);

        private:
                // Matrices and vectors
                Matrix viewProjectionMatrix_;
                Matrix projectionMatrix_;
                Matrix viewInvMatrix_;
                Matrix normalsMatrix_;
                Matrix viewMatrix_;

                Vector4d projectionParameters_;
                Vector4d shadowMapKernelSize_;
                Vector4d unprojectionVector_;
                Vector4d screenSize_;

                // Parameters
                Parameters parameters_;

                // Writes log entry
                void writeLogEntry(const char* entry);

        };

}

#endif
