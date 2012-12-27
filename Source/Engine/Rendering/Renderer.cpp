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

        Renderer::Data::MeshSubsetNode::MeshSubsetNode() {}
        Renderer::Data::MeshSubsetNode::~MeshSubsetNode() {}

        //-----------------------------------------------------------------------------------------------
        bool Renderer::Data::MeshSubsetNode::add(const Mesh::Subset& meshSubset, const Renderer::Data::Instance& instance)
        {
                Element* element = requestElement(const_cast<Mesh::Subset*>(&meshSubset));
                if(element == nullptr)
                        return false;

                element->data.push_back(instance);
                addElement(element);
                return true;
        }

        Renderer::Data::MeshNode::MeshNode() {}
        Renderer::Data::MeshNode::~MeshNode() {}

        //-----------------------------------------------------------------------------------------------
        bool Renderer::Data::MeshNode::add(const Mesh& mesh, const Mesh::Subset& meshSubset,
                                           const Renderer::Data::Instance& instance)
        {
                Element* element = requestElement(const_cast<Mesh*>(&mesh));
                if(element == nullptr)
                        return false;

                element->data.add(meshSubset, instance);

                addElement(element);
                return true;
        }

        Renderer::Data::MaterialNode::MaterialNode() {}
        Renderer::Data::MaterialNode::~MaterialNode() {}

        //-----------------------------------------------------------------------------------------------
        bool Renderer::Data::MaterialNode::add(const Material& material, const Mesh& mesh,
                                               const Mesh::Subset& meshSubset,
                                               const Renderer::Data::Instance& instance)
        {
                Element* element = requestElement(const_cast<Material*>(&material));
                if(element == nullptr)
                        return false;

                if(!element->data.add(mesh, meshSubset, instance))
                        return false;

                uint8_t renderingUnit = material.is(MATERIAL_TWO_SIDED) ? static_cast<uint8_t>(UNIT_MATERIAL_TWO_SIDED) : static_cast<uint8_t>(UNIT_MATERIAL_ONE_SIDED);

                addElement(element, renderingUnit);
                return true;
        }

        Renderer::Data::ActorNode::ActorNode() {}
        Renderer::Data::ActorNode::~ActorNode() {}

        //-----------------------------------------------------------------------------------------------
        void Renderer::Data::ActorNode::clear(bool freeMemory)
        {
                for(uint8_t i = 0; i < NUM_OF_MESH_UNITS; ++i)
                        materialNodes_[i].clear(freeMemory);
        }

        //-----------------------------------------------------------------------------------------------
        bool Renderer::Data::ActorNode::add(const Actor& actor, const Renderer::Data::Instance& instance)
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

                        materialNode.add(*meshData.subsets[i].material, *mesh, meshData.subsets[i], instance);
                }

                return true;
        }

        //-----------------------------------------------------------------------------------------------
        Renderer::Data::MaterialNode& Renderer::Data::ActorNode::getMaterialNode(uint8_t unit)
        {
                if(unit >= NUM_OF_MESH_UNITS)
                        return emptyMaterialNode_;

                return materialNodes_[unit];
        }

        Renderer::Data::LightNode::LightNode() {}
        Renderer::Data::LightNode::~LightNode() {}

        //-----------------------------------------------------------------------------------------------
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

                        Instance instance(&shadowCaster->getSkeletonInstance(), Actor::ViewProjectionTransform());
                        instance.second.compute(*shadowCaster, spotLight.getViewMatrix(), spotLight.getViewProjectionMatrix());

                        element->data.add(*shadowCaster, instance);
                }

                addElement(element, static_cast<uint8_t>(renderingUnit));
                return true;
        }

        Renderer::Data::ParticleSystemNode::ParticleSystemNode() {}
        Renderer::Data::ParticleSystemNode::~ParticleSystemNode() {}

        //-----------------------------------------------------------------------------------------------
        bool Renderer::Data::ParticleSystemNode::add(const ParticleSystem& particleSystem)
        {
                int16_t renderingUnit = particleSystem.getRenderingUnit();
                if(renderingUnit < 0 || renderingUnit >= NUM_OF_PARTICLE_SYSTEM_UNITS)
                        return false;

                Element* element = requestElement(const_cast<Texture*>(*particleSystem.getTexture()));
                if(element == nullptr)
                        return false;

                element->data.push_back(const_cast<ParticleSystem*>(&particleSystem));
                addElement(element, static_cast<uint8_t>(renderingUnit));
                return true;
        }

        Renderer::Data::Data() {}
        Renderer::Data::~Data() {}

        //-----------------------------------------------------------------------------------------------
        void Renderer::Data::clear(bool freeMemory)
        {
                actorNode_.clear(freeMemory);
                lightNode_.clear(freeMemory);
                particleSystemNode_.clear(freeMemory);
        }

        //-----------------------------------------------------------------------------------------------
        Renderer::Data::ActorNode& Renderer::Data::getActorNode()
        {
                return actorNode_;
        }

        //-----------------------------------------------------------------------------------------------
        Renderer::Data::LightNode& Renderer::Data::getLightNode()
        {
                return lightNode_;
        }

        //-----------------------------------------------------------------------------------------------
        Renderer::Data::ParticleSystemNode& Renderer::Data::getParticleSystemNode()
        {
                return particleSystemNode_;
        }

        //-----------------------------------------------------------------------------------------------
        bool Renderer::Data::addActor(const Actor& actor, const Camera& camera)
        {
                Instance instance(&actor.getSkeletonInstance(), Actor::ViewProjectionTransform());
                instance.second.compute(actor, camera.getViewMatrix(), camera.getViewProjectionMatrix());
                return actorNode_.add(actor, instance);
        }

        //-----------------------------------------------------------------------------------------------
        bool Renderer::Data::addLight(const Light& light)
        {
                return lightNode_.add(light);
        }

        //-----------------------------------------------------------------------------------------------
        bool Renderer::Data::addShadow(const Light& light, const Actor& caster)
        {
                return lightNode_.add(light, const_cast<Actor*>(&caster));
        }

        //-----------------------------------------------------------------------------------------------
        bool Renderer::Data::addParticleSystem(const ParticleSystem& particleSystem)
        {
                return particleSystemNode_.add(particleSystem);
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

        //-----------------------------------------------------------------------------------------------
        Application* Renderer::Parameters::getApplication() const
        {
                return const_cast<Application*>(application_);
        }

        //-----------------------------------------------------------------------------------------------
        FileManager* Renderer::Parameters::getFileManager() const
        {
                return const_cast<FileManager*>(fileManager_);
        }

        //-----------------------------------------------------------------------------------------------
        uint32_t Renderer::Parameters::getWidth() const
        {
                return width_;
        }

        //-----------------------------------------------------------------------------------------------
        uint32_t Renderer::Parameters::getHeight() const
        {
                return height_;
        }

        //-----------------------------------------------------------------------------------------------
        std::ostream* Renderer::Parameters::getLog() const
        {
                return const_cast<std::ostream*>(log_);
        }

        //-----------------------------------------------------------------------------------------------
        uint8_t Renderer::Parameters::getFlags() const
        {
                return flags_;
        }

        Renderer::Renderer()
        {
                gui_ = nullptr;
        }
        Renderer::~Renderer() {}

        //-----------------------------------------------------------------------------------------------
        Renderer::Data& Renderer::getData()
        {
                return data_;
        }

        //-----------------------------------------------------------------------------------------------
        const Renderer::Data& Renderer::getData() const
        {
                return data_;
        }

        //-----------------------------------------------------------------------------------------------
        void Renderer::setGui(const Gui& gui)
        {
                gui_ = const_cast<Gui*>(&gui);
        }

}
