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
                try
                {
                        auto result = node.childNodes_.insert(this);
                        if(!result.second)
                        {
                                parentNode_ = nullptr;
                                return false;
                        }
                }
                catch(...)
                {
                        parentNode_ = nullptr;
                        return false;
                }

                if(boneName != nullptr && node.skeletonInstance_ != nullptr)
                        boneIndex_ = node.skeletonInstance_->getBoneIndex(std::string(boneName));

                return true;
        }

        //----------------------------------------------------------------------
        bool Scene::Node::detach()
        {
                if(parentNode_ == nullptr)
                        return false;

                parentNode_->childNodes_.erase(this);
                parentNode_ = nullptr;
                boneIndex_ = -1;

                requestUpdateOperation();
                return true;
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
                {
                        if((*it)->hasChild(childNode))
                                return true;
                }

                return false;
        }

        Scene::Scene()
        {
                numVisibleActors_ = numVisibleLights_ = 0;
                numVisibleParticleSystems_ = 0;
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
                cameras_.clear();
                particleSystems_.clear();
        }

        //----------------------------------------------------------------------
        bool Scene::setActiveCamera(const char* name)
        {
                auto camera = getCamera(name);
                if(camera.expired())
                        return false;

                activeCamera_ = camera;
                return true;
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
        size_t Scene::getNumCameras() const
        {
                return cameras_.size();
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

                try
                {
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

                        Camera* camera = dynamic_cast<Camera*>(node);
                        if(camera != nullptr)
                        {
                                auto result = cameras_.insert(std::make_pair(std::string(node->getName()),
                                                                             std::shared_ptr<Camera>(camera)));
                                if(!result.second)
                                        return false;

                                if(activeCamera_.expired())
                                        activeCamera_ = result.first->second;

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
                }
                catch(...) {}

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
        bool Scene::removeCamera(const char* name)
        {
                auto it = cameras_.find(std::string(name));

                if(it == cameras_.end())
                        return false;

                cameras_.erase(it);

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
        std::weak_ptr<Camera> Scene::getCamera(const char* name)
        {
                auto it = cameras_.find(std::string(name));

                if(it == cameras_.end())
                        return std::weak_ptr<Camera>();

                return std::weak_ptr<Camera>(it->second);
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
                if(activeCamera_.expired())
                        return false;

                auto camera = activeCamera_.lock();

                auto& renderingData = camera->getRenderingData();

                renderingData.clear();
                renderer.getMemoryBuffer().clear();

                numVisibleActors_ = numVisibleLights_ = 0;
                numVisibleParticleSystems_ = 0;

                const Volume& cameraFrustum = camera->getFrustum();

                for(auto it = actors_.begin(); it != actors_.end(); ++it)
                {
                        Actor& actor = *it->second.get();

                        if(actor.determineRelation(cameraFrustum) != OUTSIDE)
                        {
                                ++numVisibleActors_;

                                actor.processMeshAnimations(elapsedTime);
                                if(!renderingData.addActor(actor))
                                        break;
                        }
                }

                for(auto it = lights_.begin(); it != lights_.end(); ++it)
                {
                        Light& light = *it->second.get();

                        if(light.determineRelation(cameraFrustum) != OUTSIDE)
                        {
                                ++numVisibleLights_;
                                if(!renderingData.addLight(light))
                                        break;

                                if(!camera->isEffectEnabled(Renderer::Effects::SHADOWS) ||
                                   !renderer.is(RENDERING_SHADOWS_ENABLED))
                                        continue;

                                if(!light.is(Node::SHADOW_CASTER))
                                        continue;

                                for(auto it1 = actors_.begin(); it1 != actors_.end(); ++it1)
                                {
                                        Actor& actor = *it1->second.get();

                                        if(!actor.is(Node::SHADOW_CASTER))
                                                continue;

                                        if(actor.determineRelation(light.getVolume()) != OUTSIDE)
                                        {
                                                if(!renderingData.addShadow(light, actor))
                                                        break;
                                        }
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
                                if(!renderingData.addParticleSystem(particleSystem))
                                        break;
                        }
                }

                renderer.render(*camera);
                return true;
        }

}
