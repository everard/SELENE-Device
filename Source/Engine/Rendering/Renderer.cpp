// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Renderer.h"

#include "../Core/FileManager/FileManager.h"
#include "../Application/Application.h"

#include "../Scene/Nodes/ParticleSystem.h"
#include "../Scene/Nodes/Camera.h"
#include "../Scene/Nodes/Light.h"
#include "../Scene/Nodes/Actor.h"

namespace selene
{

        RenderingMemoryBuffer Renderer::memoryBuffer_;

        Renderer::Data::MeshSubsetNode::MeshSubsetNode(): RenderingNode(Renderer::memoryBuffer_) {}
        Renderer::Data::MeshSubsetNode::~MeshSubsetNode() {}

        //---------------------------------------------------------------------------------------------------------------
        bool Renderer::Data::MeshSubsetNode::add(const Mesh::Subset& meshSubset, const Actor::Instance& instance)
        {
                Element* element = requestElement(const_cast<Mesh::Subset*>(&meshSubset));
                if(element == nullptr)
                        return false;

                if(!element->data.addElement(instance))
                        return false;

                return addElement(element);
        }

        Renderer::Data::MeshNode::MeshNode(): RenderingNode(Renderer::memoryBuffer_) {}
        Renderer::Data::MeshNode::~MeshNode() {}

        //---------------------------------------------------------------------------------------------------------------
        bool Renderer::Data::MeshNode::add(const Mesh& mesh, const Mesh::Subset& meshSubset,
                                           const Actor::Instance& instance)
        {
                Element* element = requestElement(const_cast<Mesh*>(&mesh));
                if(element == nullptr)
                        return false;

                if(!element->data.add(meshSubset, instance))
                        return false;

                return addElement(element);
        }

        Renderer::Data::MaterialNode::MaterialNode(): RenderingNode(Renderer::memoryBuffer_) {}
        Renderer::Data::MaterialNode::~MaterialNode() {}

        //---------------------------------------------------------------------------------------------------------------
        bool Renderer::Data::MaterialNode::add(const Material& material, const Mesh& mesh,
                                               const Mesh::Subset& meshSubset,
                                               const Actor::Instance& instance)
        {
                Element* element = requestElement(const_cast<Material*>(&material));
                if(element == nullptr)
                        return false;

                if(!element->data.add(mesh, meshSubset, instance))
                        return false;

                uint8_t renderingUnit = material.is(MATERIAL_TWO_SIDED) ? static_cast<uint8_t>(UNIT_MATERIAL_TWO_SIDED) :
                                                                          static_cast<uint8_t>(UNIT_MATERIAL_ONE_SIDED);

                return addElement(element, renderingUnit);
        }

        Renderer::Data::ActorNode::ActorNode() {}
        Renderer::Data::ActorNode::~ActorNode() {}

        //---------------------------------------------------------------------------------------------------------------
        void Renderer::Data::ActorNode::clear()
        {
                for(uint8_t i = 0; i < NUM_OF_MESH_UNITS; ++i)
                        materialNodes_[i].clear();
        }

        //---------------------------------------------------------------------------------------------------------------
        bool Renderer::Data::ActorNode::add(const Actor& actor, const Actor::Instance& instance)
        {
                int16_t renderingUnit = actor.getRenderingUnit();
                if(renderingUnit < 0 || renderingUnit >= NUM_OF_MESH_UNITS)
                        return false;

                auto& materialNode = materialNodes_[renderingUnit];

                Mesh* mesh = *actor.getMesh();
                if(mesh == nullptr)
                        return false;

                const auto& meshData = mesh->getData();
                for(uint16_t i = 0; i < meshData.subsets.getSize(); ++i)
                {
                        if(!meshData.subsets[i].material)
                                continue;

                        if(!materialNode.add(*meshData.subsets[i].material, *mesh, meshData.subsets[i], instance))
                                return false;
                }

                return true;
        }

        //---------------------------------------------------------------------------------------------------------------
        Renderer::Data::MaterialNode& Renderer::Data::ActorNode::getMaterialNode(uint8_t unit)
        {
                if(unit >= NUM_OF_MESH_UNITS)
                        return emptyMaterialNode_;

                return materialNodes_[unit];
        }

        Renderer::Data::LightNode::LightNode(): RenderingNode(Renderer::memoryBuffer_) {}
        Renderer::Data::LightNode::~LightNode() {}

        //---------------------------------------------------------------------------------------------------------------
        bool Renderer::Data::LightNode::add(const Light& light, Actor* shadowCaster)
        {
                int16_t renderingUnit = light.getRenderingUnit();
                if(renderingUnit < 0 || renderingUnit >= NUM_OF_LIGHT_UNITS)
                        return false;

                Element* element = requestElement(const_cast<Light*>(&light));
                if(element == nullptr)
                        return false;

                if(shadowCaster != nullptr && light.getRenderingUnit() == UNIT_LIGHT_SPOT)
                {
                        const SpotLight& spotLight = static_cast<const SpotLight&>(light);

                        Actor::ViewProjectionTransform viewProjectionTransform;
                        viewProjectionTransform.compute(*shadowCaster, spotLight.getViewMatrix(),
                                                        spotLight.getViewProjectionMatrix());
                        Actor::Instance instance(viewProjectionTransform, &shadowCaster->getSkeletonInstance());

                        element->data.add(*shadowCaster, instance);
                }

                addElement(element, static_cast<uint8_t>(renderingUnit));
                return true;
        }

        Renderer::Data::ParticleSystemNode::ParticleSystemNode(): RenderingNode(Renderer::memoryBuffer_) {}
        Renderer::Data::ParticleSystemNode::~ParticleSystemNode() {}

        //---------------------------------------------------------------------------------------------------------------
        bool Renderer::Data::ParticleSystemNode::add(const ParticleSystem& particleSystem)
        {
                int16_t renderingUnit = particleSystem.getRenderingUnit();
                if(renderingUnit < 0 || renderingUnit >= NUM_OF_PARTICLE_SYSTEM_UNITS)
                        return false;

                Element* element = requestElement(const_cast<Texture*>(*particleSystem.getTexture()));
                if(element == nullptr)
                        return false;

                element->data.addElement(const_cast<ParticleSystem*>(&particleSystem));
                addElement(element, static_cast<uint8_t>(renderingUnit));
                return true;
        }

        Renderer::Data::Data()
        {
                camera_ = nullptr;
        }
        Renderer::Data::~Data() {}

        //---------------------------------------------------------------------------------------------------------------
        void Renderer::Data::setCamera(const Camera& camera)
        {
                camera_ = const_cast<Camera*>(&camera);
        }

        //---------------------------------------------------------------------------------------------------------------
        void Renderer::Data::clear()
        {
                actorNode_.clear();
                lightNode_.clear();
                particleSystemNode_.clear();
        }

        //---------------------------------------------------------------------------------------------------------------
        Renderer::Data::ActorNode& Renderer::Data::getActorNode()
        {
                return actorNode_;
        }

        //---------------------------------------------------------------------------------------------------------------
        Renderer::Data::LightNode& Renderer::Data::getLightNode()
        {
                return lightNode_;
        }

        //---------------------------------------------------------------------------------------------------------------
        Renderer::Data::ParticleSystemNode& Renderer::Data::getParticleSystemNode()
        {
                return particleSystemNode_;
        }

        //---------------------------------------------------------------------------------------------------------------
        bool Renderer::Data::addActor(const Actor& actor)
        {
                if(camera_ == nullptr)
                        return false;

                Actor::ViewProjectionTransform viewProjectionTransform;
                viewProjectionTransform.compute(actor, camera_->getViewMatrix(), camera_->getViewProjectionMatrix());

                Actor::Instance instance(viewProjectionTransform, &actor.getSkeletonInstance());
                return actorNode_.add(actor, instance);
        }

        //---------------------------------------------------------------------------------------------------------------
        bool Renderer::Data::addLight(const Light& light)
        {
                return lightNode_.add(light);
        }

        //---------------------------------------------------------------------------------------------------------------
        bool Renderer::Data::addShadow(const Light& light, const Actor& shadowCaster)
        {
                return lightNode_.add(light, const_cast<Actor*>(&shadowCaster));
        }

        //---------------------------------------------------------------------------------------------------------------
        bool Renderer::Data::addParticleSystem(const ParticleSystem& particleSystem)
        {
                return particleSystemNode_.add(particleSystem);
        }

        Renderer::Effects::Effects()
        {
                for(uint8_t i = 0; i < NUM_OF_EFFECT_TYPES; ++i)
                        isEnabled_[i] = false;

                setSsaoParameters();
                setBloomParameters();
        }
        Renderer::Effects::~Effects() {}

        //---------------------------------------------------------------------------------------------------------------
        void Renderer::Effects::enableEffect(uint8_t type)
        {
                if(type >= NUM_OF_EFFECT_TYPES)
                        return;

                isEnabled_[type] = true;
        }

        //---------------------------------------------------------------------------------------------------------------
        void Renderer::Effects::disableEffect(uint8_t type)
        {
                if(type >= NUM_OF_EFFECT_TYPES)
                        return;

                isEnabled_[type] = false;
        }

        //---------------------------------------------------------------------------------------------------------------
        bool Renderer::Effects::isEffectEnabled(uint8_t type) const
        {
                if(type >= NUM_OF_EFFECT_TYPES)
                        return false;

                return isEnabled_[type];
        }

        //---------------------------------------------------------------------------------------------------------------
        void Renderer::Effects::setSsaoParameters(float radius, float normalInfluenceBias,
                                                  float minCosAlpha)
        {
                parameters_[SSAO].define(radius, normalInfluenceBias, minCosAlpha, 0.0f);
        }

        //---------------------------------------------------------------------------------------------------------------
        void Renderer::Effects::setBloomParameters(float luminance, float scale)
        {
                parameters_[BLOOM].define(luminance, scale, 0.18f, 0.64f);
        }

        //---------------------------------------------------------------------------------------------------------------
        const Vector4d& Renderer::Effects::getEffectParameters(uint8_t type) const
        {
                if(type >= NUM_OF_EFFECT_TYPES)
                        return parameters_[NUM_OF_EFFECT_TYPES - 1];

                return parameters_[type];
        }

        Renderer::Parameters::Parameters(Application* application, FileManager* fileManager,
                                         uint32_t width, uint32_t height, std::ostream* log,
                                         uint8_t flags)
        {
                application_ = application;
                fileManager_ = fileManager;
                width_  = width;
                height_ = height;
                log_   = log;
                flags_ = flags;
        }
        Renderer::Parameters::~Parameters() {}

        //---------------------------------------------------------------------------------------------------------------
        Application* Renderer::Parameters::getApplication() const
        {
                return const_cast<Application*>(application_);
        }

        //---------------------------------------------------------------------------------------------------------------
        FileManager* Renderer::Parameters::getFileManager() const
        {
                return const_cast<FileManager*>(fileManager_);
        }

        //---------------------------------------------------------------------------------------------------------------
        uint32_t Renderer::Parameters::getWidth() const
        {
                return width_;
        }

        //---------------------------------------------------------------------------------------------------------------
        uint32_t Renderer::Parameters::getHeight() const
        {
                return height_;
        }

        //---------------------------------------------------------------------------------------------------------------
        std::ostream* Renderer::Parameters::getLog() const
        {
                return const_cast<std::ostream*>(log_);
        }

        //---------------------------------------------------------------------------------------------------------------
        uint8_t Renderer::Parameters::getFlags() const
        {
                return flags_;
        }

        Renderer::Renderer() {}
        Renderer::~Renderer() {}

        //---------------------------------------------------------------------------------------------------------------
        bool Renderer::initializeMemoryBuffer(std::size_t size)
        {
                return memoryBuffer_.initialize(size);
        }

        //---------------------------------------------------------------------------------------------------------------
        void Renderer::destroyMemoryBuffer()
        {
                memoryBuffer_.destroy();
        }

        //---------------------------------------------------------------------------------------------------------------
        RenderingMemoryBuffer& Renderer::getMemoryBuffer()
        {
                return memoryBuffer_;
        }

}
