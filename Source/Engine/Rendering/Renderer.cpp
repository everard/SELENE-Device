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
        void Renderer::setGui(const Gui& gui)
        {
                gui_ = const_cast<Gui*>(&gui);
        }

        //-----------------------------------------------------------------------------------------------
        void Renderer::clearLists(bool freeMemory)
        {
                actorsList_.clear(freeMemory);

                for(uint8_t i = 0; i < NUM_OF_LIGHTS_LIST_TYPES; ++i)
                        lightsLists_[i].clear(freeMemory);

                particleSystemsList_.clear(freeMemory);
        }

        //-----------------------------------------------------------------------------------------------
        bool Renderer::addActor(const Actor& actor)
        {
                return actorsList_.addActor(actor);
        }

        //-----------------------------------------------------------------------------------------------
        bool Renderer::addActor(const Light& light, const Actor& actor)
        {
                if(light.is(Scene::Node::SHADOW_CASTER))
                        return lightsLists_[LIGHTS_LIST_WITH_SHADOWS].addLight(light, const_cast<Actor*>(&actor));

                return lightsLists_[LIGHTS_LIST_WITHOUT_SHADOWS].addLight(light, const_cast<Actor*>(&actor));
        }

        //-----------------------------------------------------------------------------------------------
        bool Renderer::addLight(const Light& light)
        {
                if(light.is(Scene::Node::SHADOW_CASTER))
                        return lightsLists_[LIGHTS_LIST_WITH_SHADOWS].addLight(light);

                return lightsLists_[LIGHTS_LIST_WITHOUT_SHADOWS].addLight(light);
        }

        //-----------------------------------------------------------------------------------------------
        bool Renderer::addShadow(const Light& light, const Actor& caster)
        {
                if(!light.is(Scene::Node::SHADOW_CASTER))
                        return false;

                return lightsLists_[LIGHTS_LIST_WITH_SHADOWS].addLight(light, nullptr,
                                                                       const_cast<Actor*>(&caster));
        }

        //-----------------------------------------------------------------------------------------------
        bool Renderer::addParticleSystem(const ParticleSystem& particleSystem)
        {
                return particleSystemsList_.addParticleSystem(particleSystem);
        }

        Renderer::MeshSubsetsList::MeshSubsetsList() {}
        Renderer::MeshSubsetsList::~MeshSubsetsList() {}

        //-----------------------------------------------------------------------------------------------
        bool Renderer::MeshSubsetsList::addMeshSubset(const Mesh::Subset& meshSubset, const Actor& actor)
        {
                // get element
                Element* element = requestElement(const_cast<Mesh::Subset*>(&meshSubset));

                if(element == nullptr)
                        return false;

                // add actor
                element->data.push_back(const_cast<Actor*>(&actor));

                // add element
                uint8_t unit = UNIT_MATERIAL_ONE_SIDED;

                if(meshSubset.material.is(MATERIAL_TWO_SIDED))
                        unit = UNIT_MATERIAL_TWO_SIDED;

                addElement(element, unit);
                return true;
        }

        Renderer::ActorsList::ActorsList() {}
        Renderer::ActorsList::~ActorsList() {}

        //-----------------------------------------------------------------------------------------------
        bool Renderer::ActorsList::addActor(const Actor& actor)
        {
                // get mesh
                Mesh* mesh = *actor.getMesh();

                if(mesh == nullptr)
                        return false;

                // get unit
                int16_t unit = actor.getRenderingUnit();

                if(unit < 0 || unit >= NUM_OF_MESH_UNITS)
                        return false;

                // get element
                Element* element = requestElement(mesh);

                if(element == nullptr)
                        return false;

                // add mesh subset instances
                const Array<Mesh::Subset, uint16_t>& meshSubsets = mesh->getData().subsets;

                for(uint16_t i = 0; i < meshSubsets.getSize(); ++i)
                        element->data.addMeshSubset(meshSubsets[i], actor);

                // add element
                addElement(element, static_cast<uint8_t>(unit));
                return true;
        }

        Renderer::LightData::LightData() {}
        Renderer::LightData::~LightData() {}

        //-----------------------------------------------------------------------------------------------
        void Renderer::LightData::clear()
        {
                actorsList_.clear();
                shadowsList_.clear();
        }

        //-----------------------------------------------------------------------------------------------
        Renderer::ActorsList& Renderer::LightData::getActorsList()
        {
                return actorsList_;
        }

        //-----------------------------------------------------------------------------------------------
        Renderer::ActorsList& Renderer::LightData::getShadowsList()
        {
                return shadowsList_;
        }

        Renderer::LightsList::LightsList() {}
        Renderer::LightsList::~LightsList() {}

        //-----------------------------------------------------------------------------------------------
        bool Renderer::LightsList::addLight(const Light& light, Actor* actor, Actor* shadow)
        {
                // get unit
                int16_t unit = light.getRenderingUnit();

                if(unit < 0 || unit >= NUM_OF_LIGHT_UNITS)
                        return false;

                // get element
                Element* element = requestElement(const_cast<Light*>(&light));

                if(element == nullptr)
                        return false;

                // add actor
                if(actor != nullptr)
                {
                        if(!element->data.getActorsList().addActor(*actor))
                                return false;
                }

                // add shadow
                if(shadow != nullptr)
                {
                        if(!element->data.getShadowsList().addActor(*shadow))
                                return false;
                }

                // add element
                addElement(element, static_cast<uint8_t>(unit));
                return true;
        }

        Renderer::ParticleSystemsList::ParticleSystemsList() {}
        Renderer::ParticleSystemsList::~ParticleSystemsList() {}

        //-----------------------------------------------------------------------------------------------
        bool Renderer::ParticleSystemsList::addParticleSystem(const ParticleSystem& particleSystem)
        {
                // get unit
                int16_t unit = particleSystem.getRenderingUnit();

                if(unit < 0 || unit >= NUM_OF_PARTICLE_SYSTEM_UNITS)
                        return false;

                // get element
                Element* element = requestElement(*particleSystem.getTexture());

                if(element == nullptr)
                        return false;

                // add particle system
                element->data.push_back(const_cast<ParticleSystem*>(&particleSystem));

                // add element
                addElement(element, static_cast<uint8_t>(unit));
                return true;
        }

}
