// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLRenderer.h"

#include "../../../../Engine/Scene/Nodes/ParticleSystem.h"
#include "../../../../Engine/Scene/Nodes/Camera.h"
#include "../../../../Engine/Scene/Nodes/Actor.h"
#include "../../Application/LinuxApplication.h"
#include "../../../../Engine/GUI/GUI.h"

#include "Resources/GLTexture.h"
#include "Resources/GLMesh.h"

namespace selene
{

        GlRenderer::GlRenderer(): parameters_(nullptr, nullptr, 0, 0, nullptr, 0) {}
        GlRenderer::~GlRenderer()
        {
                destroy();
        }

        //-----------------------------------------------------------------
        bool GlRenderer::initialize(const Renderer::Parameters& parameters)
        {
                destroy();

                parameters_ = parameters;

                writeLogEntry("--- Initializing OpenGL renderer ---");
                return true;
        }

        //-----------------------------------------------------------------
        void GlRenderer::destroy() {}

        //-----------------------------------------------------------------
        void GlRenderer::render(const Camera& camera)
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

                // clear rendering lists
                clearLists();
        }

        //-----------------------------------------------------------------
        void GlRenderer::writeLogEntry(const char* entry)
        {
                if(parameters_.getLog() != nullptr)
                        (*parameters_.getLog()) << entry << std::endl;
        }

}
