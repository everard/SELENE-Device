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

        GlesRenderer::GlesRenderer(): parameters_(nullptr, nullptr, 0, 0, nullptr, 0) {}
        GlesRenderer::~GlesRenderer()
        {
                destroy();
        }

        //-------------------------------------------------------------------
        bool GlesRenderer::initialize(const Renderer::Parameters& parameters)
        {
                destroy();

                parameters_ = parameters;

                writeLogEntry("--- Initializing OpenGLES renderer ---");
                return true;
        }

        //-------------------------------------------------------------------
        void GlesRenderer::destroy() {}

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

                data_.clear();
        }

        //-------------------------------------------------------------------
        void GlesRenderer::writeLogEntry(const char* entry)
        {
                if(parameters_.getLog() != nullptr)
                        (*parameters_.getLog()) << entry << std::endl;
        }

}
