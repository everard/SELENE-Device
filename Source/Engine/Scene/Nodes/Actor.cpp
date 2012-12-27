// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Actor.h"

#include "../../Rendering/Renderer.h"
#include "Camera.h"

namespace selene
{

        Actor::ViewProjectionTransform::ViewProjectionTransform() {}
        Actor::ViewProjectionTransform::~ViewProjectionTransform() {}

        //----------------------------------------------------------------------------------------
        void Actor::ViewProjectionTransform::compute(const Actor& actor, const Matrix& viewMatrix,
                                                     const Matrix& viewProjectionMatrix)
        {
                worldViewProjectionMatrix_ = actor.getWorldMatrix() * viewProjectionMatrix;
                worldViewMatrix_ = actor.getWorldMatrix() * viewMatrix;
                normalsMatrix_ = worldViewMatrix_;
                normalsMatrix_.invert();
                normalsMatrix_.transpose();
        }

        //------------------------------------------------------------------------------------
        const Matrix& Actor::ViewProjectionTransform::getWorldViewProjectionMatrix() const
        {
                return worldViewProjectionMatrix_;
        }

        //------------------------------------------------------------------------------------
        const Matrix& Actor::ViewProjectionTransform::getWorldViewMatrix() const
        {
                return worldViewMatrix_;
        }

        //------------------------------------------------------------------------------------
        const Matrix& Actor::ViewProjectionTransform::getNormalsMatrix() const
        {
                return normalsMatrix_;
        }

        Actor::Actor(const char* name,
                     const Resource::Instance<Mesh>& mesh,
                     const Vector3d& position,
                     const Quaternion& rotation,
                     const Vector3d& scale): Scene::Node(name)
        {
                positions_[ORIGINAL] = position;
                rotations_[ORIGINAL] = rotation;
                scale_[ORIGINAL]     = scale;

                setMesh(mesh);
        }
        Actor::~Actor() {}

        //------------------------------------------------------------------------------------
        void Actor::setMesh(const Resource::Instance<Mesh>& mesh)
        {
                skeletonInstance_ = nullptr;
                renderingUnit_ = -1;
                mesh_ = mesh;

                if(*mesh_ == nullptr)
                        return;

                const Mesh::Data& meshData = (*mesh_)->getData();
                boundingBoxes_[ORIGINAL] = meshData.boundingBox;

                if((*mesh_)->hasSkeleton())
                {
                        if(!meshAnimationProcessor_.initialize(meshData.skeleton))
                        {
                                mesh_ = Resource::Instance<Mesh>();
                                skeletonInstance_ = nullptr;

                                return;
                        }

                        renderingUnit_ = Renderer::Data::UNIT_MESH_SKIN;
                        skeletonInstance_ = const_cast<Skeleton::Instance*>(&meshAnimationProcessor_.getSkeletonInstance());
                }
                else
                {
                        renderingUnit_ = Renderer::Data::UNIT_MESH_STATIC;
                        skeletonInstance_ = nullptr;
                }

                requestUpdateOperation();
        }

        //------------------------------------------------------------------------------------
        const Resource::Instance<Mesh>& Actor::getMesh() const
        {
                return mesh_;
        }

        //------------------------------------------------------------------------------------
        const Skeleton::Instance& Actor::getSkeletonInstance() const
        {
                return meshAnimationProcessor_.getSkeletonInstance();
        }

        //------------------------------------------------------------------------------------
        bool Actor::addMeshAnimation(const Resource::Instance<MeshAnimation>& meshAnimation,
                                     float blendFactorTransitionTime,
                                     float startingTransitionTime,
                                     float stoppingTransitionTime,
                                     float animationTime,
                                     float blendFactor)
        {
                if(skeletonInstance_ == nullptr)
                        return false;

                return meshAnimationProcessor_.addMeshAnimation(meshAnimation,
                                                                blendFactorTransitionTime,
                                                                startingTransitionTime,
                                                                stoppingTransitionTime,
                                                                animationTime,
                                                                blendFactor);
        }

        //------------------------------------------------------------------------------------
        bool Actor::removeMeshAnimation(uint32_t index)
        {
                if(skeletonInstance_ == nullptr)
                        return false;

                return meshAnimationProcessor_.removeMeshAnimation(index);
        }

        //------------------------------------------------------------------------------------
        MeshAnimationProcessor::MixableMeshAnimation& Actor::getMeshAnimation(uint32_t index)
        {
                return meshAnimationProcessor_.getMeshAnimation(index);
        }

        //------------------------------------------------------------------------------------
        void Actor::processMeshAnimations(float elapsedTime)
        {
                if(skeletonInstance_ == nullptr)
                        return;

                meshAnimationProcessor_.processMeshAnimations(elapsedTime);
                requestChildNodesUpdateOperation();
        }

        //------------------------------------------------------------------------------------
        const Box& Actor::getBoundingBox() const
        {
                performUpdateOperation();
                return boundingBoxes_[MODIFIED];
        }

        //------------------------------------------------------------------------------------
        int16_t Actor::getRenderingUnit() const
        {
                return renderingUnit_;
        }

        //------------------------------------------------------------------------------------
        RELATION Actor::determineRelation(const Volume& volume) const
        {
                if(is(HIDDEN))
                        return OUTSIDE;

                performUpdateOperation();

                return boundingBoxes_[MODIFIED].determineRelation(volume);
        }

        //------------------------------------------------------------------------------------
        void Actor::update() const
        {
                // transform bounding box to world space
                boundingBoxes_[MODIFIED] = boundingBoxes_[ORIGINAL];
                boundingBoxes_[MODIFIED].transform(worldMatrix_);
        }

}
