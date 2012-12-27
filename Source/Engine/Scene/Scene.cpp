// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Scene.h"

#include "../Rendering/Renderer.h"
#include "Nodes/ParticleSystem.h"
#include "Nodes/Camera.h"
#include "Nodes/Actor.h"
#include "Nodes/Light.h"

namespace selene
{

        Scene::Node::Node(const char* name): Entity(name)
        {
                scale_[ORIGINAL].define(1.0f);

                skeletonInstance_ = nullptr;
                boneIndex_ = -1;

                parentNode_ = nullptr;
        }
        Scene::Node::~Node()
        {
                // detach from parent
                detach();

                // detach children
                for(auto it = childNodes_.begin(); it != childNodes_.end(); ++it)
                        (*it)->parentNode_ = nullptr;

                childNodes_.clear();
        }

        //----------------------------------------------------------------------
        void Scene::Node::setPosition(const Vector3d& position)
        {
                positions_[ORIGINAL] = position;
                requestUpdateOperation();
        }

        //----------------------------------------------------------------------
        const Vector3d& Scene::Node::getPosition() const
        {
                performUpdateOperation();
                return positions_[MODIFIED];
        }

        //----------------------------------------------------------------------
        void Scene::Node::setRotation(const Quaternion& rotation)
        {
                rotations_[ORIGINAL] = rotation;
                rotations_[ORIGINAL].normalize();
                requestUpdateOperation();
        }

        //----------------------------------------------------------------------
        const Quaternion& Scene::Node::getRotation() const
        {
                performUpdateOperation();
                return rotations_[MODIFIED];
        }

        //----------------------------------------------------------------------
        void Scene::Node::setScale(const Vector3d& scale)
        {
                scale_[ORIGINAL] = scale;
                requestUpdateOperation();
        }

        //----------------------------------------------------------------------
        const Vector3d& Scene::Node::getScale() const
        {
                performUpdateOperation();
                return scale_[MODIFIED];
        }

        //----------------------------------------------------------------------
        bool Scene::Node::attach(Node& node, const char* boneName)
        {
                // cannot attach to itself or own child
                if(this == &node || hasChild(&node))
                        return false;

                // detach from parent
                detach();

                // set new parent node
                parentNode_ = &node;

                // add this node to children of parent
                auto result = node.childNodes_.insert(this);
                if(!result.second)
                {
                        parentNode_ = nullptr;
                        return false;
                }

                if(boneName != nullptr && node.skeletonInstance_ != nullptr)
                        boneIndex_ = node.skeletonInstance_->getBoneIndex(std::string(boneName));

                return true;
        }

        //----------------------------------------------------------------------
        void Scene::Node::detach()
        {
                if(parentNode_ != nullptr)
                        parentNode_->childNodes_.erase(this);

                parentNode_ = nullptr;
                boneIndex_ = -1;

                requestUpdateOperation();
        }

        //----------------------------------------------------------------------
        const Matrix& Scene::Node::getWorldMatrix() const
        {
                performUpdateOperation();
                return worldMatrix_;
        }

        //----------------------------------------------------------------------
        void Scene::Node::requestUpdateOperation() const
        {
                if(!is(UPDATED))
                        return;

                clearFlags(UPDATED);
                requestChildNodesUpdateOperation();
        }

        //----------------------------------------------------------------------
        void Scene::Node::requestChildNodesUpdateOperation() const
        {
                if(childNodes_.size() > 0)
                {
                        for(auto it = childNodes_.begin(); it != childNodes_.end(); ++it)
                                (*it)->requestUpdateOperation();
                }
        }

        //----------------------------------------------------------------------
        void Scene::Node::performUpdateOperation() const
        {
                if(is(UPDATED))
                        return;

                positions_[MODIFIED] = positions_[ORIGINAL];
                rotations_[MODIFIED] = rotations_[ORIGINAL];
                scale_[MODIFIED] = scale_[ORIGINAL];

                if(parentNode_ != nullptr)
                {
                        parentNode_->performUpdateOperation();

                        if(boneIndex_ >= 0 && parentNode_->skeletonInstance_ != nullptr)
                        {
                                const Array<Skeleton::Transform, uint16_t>& boneTransforms =
                                        parentNode_->skeletonInstance_->getCombinedBoneTransforms();

                                if(boneIndex_ < boneTransforms.getSize())
                                {
                                        const Skeleton::Transform& boneTransform =
                                                boneTransforms[boneIndex_];

                                        positions_[MODIFIED] = boneTransform.position +
                                                               boneTransform.rotation.rotate(positions_[MODIFIED]);
                                        rotations_[MODIFIED] = boneTransform.rotation * rotations_[MODIFIED];
                                }
                        }

                        positions_[MODIFIED] = positions_[MODIFIED].scale(parentNode_->scale_[MODIFIED]);

                        positions_[MODIFIED] = parentNode_->positions_[MODIFIED] +
                                               parentNode_->rotations_[MODIFIED].rotate(positions_[MODIFIED]);
                        rotations_[MODIFIED] = parentNode_->rotations_[MODIFIED] * rotations_[MODIFIED];

                        scale_[MODIFIED] = scale_[MODIFIED].scale(parentNode_->scale_[MODIFIED]);
                }

                // compute world matrix
                Matrix translationMatrix, rotationMatrix, scaleMatrix;

                translationMatrix.translation(positions_[MODIFIED]);
                rotationMatrix = rotations_[MODIFIED].convert();
                rotationMatrix.transpose();
                scaleMatrix.scale(scale_[MODIFIED]);

                worldMatrix_ = scaleMatrix * rotationMatrix * translationMatrix;

                update();
                setFlags(UPDATED);
        }

        //----------------------------------------------------------------------
        bool Scene::Node::hasChild(Node* childNode)
        {
                if(childNodes_.size() == 0)
                        return false;

                auto it = childNodes_.find(childNode);

                if(it != childNodes_.end())
                        return true;

                for(it = childNodes_.begin(); it != childNodes_.end(); ++it)
                        if((*it)->hasChild(childNode))
                                return true;

                return false;
        }

        Scene::Scene()
        {
                numVisibleActors_ = numVisibleLights_ = 0;
                numVisibleParticleSystems_ = 0;

                camera_ = nullptr;
        }
        Scene::~Scene()
        {
                destroy();
        }

        //----------------------------------------------------------------------
        void Scene::destroy()
        {
                actors_.clear();
                lights_.clear();
                particleSystems_.clear();
        }

        //----------------------------------------------------------------------
        void Scene::setCamera(Camera* camera)
        {
                camera_ = camera;
        }

        //----------------------------------------------------------------------
        uint32_t Scene::getNumVisibleActors() const
        {
                return numVisibleActors_;
        }

        //----------------------------------------------------------------------
        uint32_t Scene::getNumVisibleLights() const
        {
                return numVisibleLights_;
        }

        //----------------------------------------------------------------------
        uint32_t Scene::getNumVisibleParticleSystems() const
        {
                return numVisibleParticleSystems_;
        }

        //----------------------------------------------------------------------
        size_t Scene::getNumActors() const
        {
                return actors_.size();
        }

        //----------------------------------------------------------------------
        size_t Scene::getNumLights() const
        {
                return lights_.size();
        }

        //----------------------------------------------------------------------
        size_t Scene::getNumParticleSystems() const
        {
                return particleSystems_.size();
        }

        //----------------------------------------------------------------------
        bool Scene::addNode(Node* node)
        {
                if(node == nullptr)
                        return false;

                Actor* actor = dynamic_cast<Actor*>(node);
                if(actor != nullptr)
                {
                        auto result = actors_.insert(std::make_pair(std::string(node->getName()),
                                                                    std::shared_ptr<Actor>(actor)));
                        if(!result.second)
                                return false;

                        return true;
                }

                Light* light = dynamic_cast<Light*>(node);
                if(light != nullptr)
                {
                        auto result = lights_.insert(std::make_pair(std::string(node->getName()),
                                                                    std::shared_ptr<Light>(light)));
                        if(!result.second)
                                return false;

                        return true;
                }

                ParticleSystem* particleSystem = dynamic_cast<ParticleSystem*>(node);
                if(particleSystem != nullptr)
                {
                        auto result = particleSystems_.insert(std::make_pair(std::string(node->getName()),
                                                                             std::shared_ptr<ParticleSystem>(particleSystem)));
                        if(!result.second)
                                return false;

                        return true;
                }

                SAFE_DELETE(node);
                return false;
        }

        //----------------------------------------------------------------------
        bool Scene::removeActor(const char* name)
        {
                auto it = actors_.find(std::string(name));

                if(it == actors_.end())
                        return false;

                actors_.erase(it);

                return true;
        }

        //----------------------------------------------------------------------
        bool Scene::removeLight(const char* name)
        {
                auto it = lights_.find(std::string(name));

                if(it == lights_.end())
                        return false;

                lights_.erase(it);

                return true;
        }

        //----------------------------------------------------------------------
        bool Scene::removeParticleSystem(const char* name)
        {
                auto it = particleSystems_.find(std::string(name));

                if(it == particleSystems_.end())
                        return false;

                particleSystems_.erase(it);

                return true;
        }

        //----------------------------------------------------------------------
        std::weak_ptr<Actor> Scene::getActor(const char* name)
        {
                auto it = actors_.find(std::string(name));

                if(it == actors_.end())
                        return std::weak_ptr<Actor>();

                return std::weak_ptr<Actor>(it->second);
        }

        //----------------------------------------------------------------------
        std::weak_ptr<Light> Scene::getLight(const char* name)
        {
                auto it = lights_.find(std::string(name));

                if(it == lights_.end())
                        return std::weak_ptr<Light>();

                return std::weak_ptr<Light>(it->second);
        }

        //----------------------------------------------------------------------
        std::weak_ptr<ParticleSystem> Scene::getParticleSystem(const char* name)
        {
                auto it = particleSystems_.find(std::string(name));

                if(it == particleSystems_.end())
                        return std::weak_ptr<ParticleSystem>();

                return std::weak_ptr<ParticleSystem>(it->second);
        }

        //----------------------------------------------------------------------
        bool Scene::updateAndRender(float elapsedTime, Renderer& renderer)
        {
                if(camera_ == nullptr)
                        return false;

                auto& renderingData = renderer.getData();
                renderingData.clear();

                numVisibleActors_ = numVisibleLights_ = 0;
                numVisibleParticleSystems_ = 0;

                const Volume& cameraFrustum = camera_->getFrustum();

                for(auto it = actors_.begin(); it != actors_.end(); ++it)
                {
                        Actor& actor = *it->second.get();

                        if(actor.determineRelation(cameraFrustum) != OUTSIDE)
                        {
                                ++numVisibleActors_;

                                actor.processMeshAnimations(elapsedTime);
                                renderingData.addActor(actor, *camera_);
                        }
                }

                for(auto it = lights_.begin(); it != lights_.end(); ++it)
                {
                        Light& light = *it->second.get();

                        if(light.determineRelation(cameraFrustum) != OUTSIDE)
                        {
                                ++numVisibleLights_;
                                renderingData.addLight(light);

                                if(!light.is(Node::SHADOW_CASTER))
                                        continue;

                                for(auto it1 = actors_.begin(); it1 != actors_.end(); ++it1)
                                {
                                        Actor& actor = *it1->second.get();

                                        if(!actor.is(Node::SHADOW_CASTER))
                                                continue;

                                        if(actor.determineRelation(light.getVolume()) != OUTSIDE)
                                                renderingData.addShadow(light, actor);
                                }
                        }
                }

                for(auto it = particleSystems_.begin(); it != particleSystems_.end(); ++it)
                {
                        ParticleSystem& particleSystem = *it->second.get();

                        if(particleSystem.determineRelation(cameraFrustum) != OUTSIDE)
                        {
                                ++numVisibleParticleSystems_;
                                particleSystem.process(elapsedTime);
                                renderingData.addParticleSystem(particleSystem);
                        }
                }

                renderer.render(*camera_);
                return true;
        }

}
