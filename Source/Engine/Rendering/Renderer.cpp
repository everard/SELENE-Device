// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Renderer.h"

#include "../Core/FileManager/FileManager.h"
#include "../Application/Application.h"

#include "../Scene/Nodes/Camera.h"
#include "../Scene/Nodes/Light.h"
#include "../Scene/Nodes/Actor.h"

namespace selene
{

        RenderingMemoryBuffer Renderer::memoryBuffer_;

        Renderer::Data::MeshSubsetNode::MeshSubsetNode(): RenderingNode(Renderer::memoryBuffer_) {}
        Renderer::Data::MeshSubsetNode::~MeshSubsetNode() {}

        //-----------------------------------------------------------------------------------------------------------
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

        //-----------------------------------------------------------------------------------------------------------
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

        //-----------------------------------------------------------------------------------------------------------
        bool Renderer::Data::MaterialNode::add(const Material& material, const Mesh& mesh,
                                               const Mesh::Subset& meshSubset,
                                               const Actor::Instance& instance)
        {
                Element* element = requestElement(const_cast<Material*>(&material));
                if(element == nullptr)
                        return false;

                if(!element->data.add(mesh, meshSubset, instance))
                        return false;

                uint8_t renderingUnit =
                        material.is(MATERIAL_TWO_SIDED) ? static_cast<uint8_t>(UNIT_MATERIAL_TWO_SIDED) :
                                                          static_cast<uint8_t>(UNIT_MATERIAL_ONE_SIDED);

                return addElement(element, renderingUnit);
        }

        Renderer::Data::ActorNode::ActorNode(): emptyMaterialNode_() {}
        Renderer::Data::ActorNode::~ActorNode() {}

        //-----------------------------------------------------------------------------------------------------------
        void Renderer::Data::ActorNode::clear()
        {
                for(uint8_t i = 0; i < NUM_OF_MESH_UNITS; ++i)
                        materialNodes_[i].clear();
        }

        //-----------------------------------------------------------------------------------------------------------
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

        //-----------------------------------------------------------------------------------------------------------
        Renderer::Data::MaterialNode& Renderer::Data::ActorNode::getMaterialNode(uint8_t unit)
        {
                if(unit >= NUM_OF_MESH_UNITS)
                        return emptyMaterialNode_;

                return materialNodes_[unit];
        }

        Renderer::Data::LightNode::LightNode(): RenderingNode(Renderer::memoryBuffer_) {}
        Renderer::Data::LightNode::~LightNode() {}

        //-----------------------------------------------------------------------------------------------------------
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

        Renderer::Data::Data(): actorNode_(), lightNode_(), camera_(nullptr) {}
        Renderer::Data::~Data() {}

        //-----------------------------------------------------------------------------------------------------------
        void Renderer::Data::setCamera(const Camera& camera)
        {
                camera_ = const_cast<Camera*>(&camera);
        }

        //-----------------------------------------------------------------------------------------------------------
        void Renderer::Data::clear()
        {
                actorNode_.clear();
                lightNode_.clear();
        }

        //-----------------------------------------------------------------------------------------------------------
        Renderer::Data::ActorNode& Renderer::Data::getActorNode()
        {
                return actorNode_;
        }

        //-----------------------------------------------------------------------------------------------------------
        Renderer::Data::LightNode& Renderer::Data::getLightNode()
        {
                return lightNode_;
        }

        //-----------------------------------------------------------------------------------------------------------
        bool Renderer::Data::addActor(const Actor& actor)
        {
                if(camera_ == nullptr)
                        return false;

                Actor::ViewProjectionTransform viewProjectionTransform;
                viewProjectionTransform.compute(actor, camera_->getViewMatrix(), camera_->getViewProjectionMatrix());

                Actor::Instance instance(viewProjectionTransform, &actor.getSkeletonInstance());
                return actorNode_.add(actor, instance);
        }

        //-----------------------------------------------------------------------------------------------------------
        bool Renderer::Data::addLight(const Light& light)
        {
                return lightNode_.add(light);
        }

        //-----------------------------------------------------------------------------------------------------------
        bool Renderer::Data::addShadow(const Light& light, const Actor& shadowCaster)
        {
                return lightNode_.add(light, const_cast<Actor*>(&shadowCaster));
        }

        Renderer::Parameters::Parameters(Application* application, FileManager* fileManager,
                                         uint32_t width, uint32_t height, std::ostream* log,
                                         bool isFullScreenEnabledFlag):
                application_(application), fileManager_(fileManager), width_(width),
                height_(height), log_(log), isFullScreenEnabledFlag_(isFullScreenEnabledFlag) {}
        Renderer::Parameters::~Parameters() {}

        //-----------------------------------------------------------------------------------------------------------
        Application* Renderer::Parameters::getApplication() const
        {
                return const_cast<Application*>(application_);
        }

        //-----------------------------------------------------------------------------------------------------------
        FileManager* Renderer::Parameters::getFileManager() const
        {
                return const_cast<FileManager*>(fileManager_);
        }

        //-----------------------------------------------------------------------------------------------------------
        uint32_t Renderer::Parameters::getWidth() const
        {
                return width_;
        }

        //-----------------------------------------------------------------------------------------------------------
        uint32_t Renderer::Parameters::getHeight() const
        {
                return height_;
        }

        //-----------------------------------------------------------------------------------------------------------
        std::ostream* Renderer::Parameters::getLog() const
        {
                return const_cast<std::ostream*>(log_);
        }

        //-----------------------------------------------------------------------------------------------------------
        bool Renderer::Parameters::isFullScreenEnabled() const
        {
                return isFullScreenEnabledFlag_;
        }

        Renderer::Renderer(): parameters_(nullptr, nullptr, 0, 0, nullptr, false), effectsList_() {}
        Renderer::~Renderer() {}

        //-----------------------------------------------------------------------------------------------------------
        const Renderer::EffectsList& Renderer::getEffects() const
        {
                return effectsList_;
        }

        //-----------------------------------------------------------------------------------------------------------
        bool Renderer::initializeMemoryBuffer(std::size_t size)
        {
                return memoryBuffer_.initialize(size);
        }

        //-----------------------------------------------------------------------------------------------------------
        void Renderer::destroyMemoryBuffer()
        {
                memoryBuffer_.destroy();
        }

        //-----------------------------------------------------------------------------------------------------------
        RenderingMemoryBuffer& Renderer::getMemoryBuffer()
        {
                return memoryBuffer_;
        }

}
