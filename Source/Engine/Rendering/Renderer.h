// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef RENDERER_H
#define RENDERER_H

#include "../Core/Resources/Mesh/Mesh.h"
#include "../Core/Math/Matrix.h"

#include "../Scene/Nodes/Actor.h"

#include <algorithm>
#include <ostream>
#include <utility>
#include <memory>
#include <vector>
#include <map>
#include <new>

namespace selene
{

        // Forward declaration of classes
        class ParticleSystem;
        class Application;
        class FileManager;
        class Material;
        class Camera;
        class Light;
        class Gui;

        /// Rendering flag
        enum RENDERING_FLAG
        {
                RENDERING_SSAO_ENABLED  = 0x01,
                RENDERING_BLOOM_ENABLED = 0x02,
                RENDERING_FULL_SCREEN_ENABLED = 0x04
        };

        /**
         * Represents renderer. This is base class for all renderers.
         */
        class Renderer
        {
        public:
                /**
                 * Represents rendering data.
                 */
                class Data
                {
                public:
                        /// Rendering units
                        enum
                        {
                                UNIT_MATERIAL_ONE_SIDED = 0,
                                UNIT_MATERIAL_TWO_SIDED,
                                NUM_OF_MATERIAL_UNITS,

                                UNIT_MESH_STATIC = 0,
                                UNIT_MESH_SKIN,
                                NUM_OF_MESH_UNITS,

                                UNIT_LIGHT_NO_SHADOWS_DIRECTIONAL = 0,
                                UNIT_LIGHT_NO_SHADOWS_POINT,
                                UNIT_LIGHT_NO_SHADOWS_SPOT,
                                UNIT_LIGHT_DIRECTIONAL,
                                UNIT_LIGHT_POINT,
                                UNIT_LIGHT_SPOT,
                                NUM_OF_LIGHT_UNITS,

                                UNIT_PARTICLE_SYSTEM = 0,
                                NUM_OF_PARTICLE_SYSTEM_UNITS
                        };

                        /**
                         * Represents rendering node. Each node contains elements, which are
                         * sorted by given key K. During data preparation elements are added
                         * to one or more rendering unit (number of units is N). Each element
                         * contains rendering data D, which can be anything.
                         */
                        template <class K, class D, uint8_t N = 1> class Node
                        {
                        public:
                                Node()
                                {
                                        for(uint8_t i = 0; i < N; ++i)
                                                elements_[i].reserve(50);

                                        currentUnit_ = -1;
                                }
                                ~Node()
                                {
                                        clear(true);
                                }

                                /**
                                 * \brief Clears node.
                                 * \param[in] freeMemory flag, which forces memory deallocation
                                 */
                                void clear(bool freeMemory = false)
                                {
                                        currentUnit_ = -1;

                                        if(freeMemory)
                                                elementsMap_.clear();
                                        else
                                        {
                                                for(uint8_t i = 0; i < N; ++i)
                                                        for(auto it = elements_[i].begin(); it != elements_[i].end(); ++it)
                                                        {
                                                                (*it)->isListed = false;
                                                                (*it)->data.clear();
                                                        }
                                        }

                                        for(uint8_t i = 0; i < N; ++i)
                                                elements_[i].clear();
                                }

                                /**
                                 * \brief Reads first element from specified unit.
                                 *
                                 * Next element from this unit can be read by readNextElement() function.
                                 * \param[in] unit unit
                                 * \return true if first element from specified unit has been successfully read
                                 */
                                bool readFirstElement(uint8_t unit = 0)
                                {
                                        if(unit >= N)
                                        {
                                                currentUnit_ = -1;
                                                return false;
                                        }

                                        currentUnit_ = unit;
                                        currentElement_ = elements_[currentUnit_].begin();

                                        if(currentElement_ == elements_[currentUnit_].end())
                                        {
                                                currentUnit_ = -1;
                                                return false;
                                        }

                                        return true;
                                }

                                /**
                                 * \brief Reads next element from unit.
                                 *
                                 * This function may be called only after call to the readFirstElement()
                                 * function.
                                 * \see readFirstElement
                                 * \return true if next element from current unit has been successfully read,
                                 * false if reading of current unit has been ended
                                 */
                                bool readNextElement()
                                {
                                        if(currentUnit_ < 0)
                                                return false;

                                        ++currentElement_;

                                        if(currentElement_ == elements_[currentUnit_].end())
                                        {
                                                currentUnit_ = -1;
                                                return false;
                                        }

                                        return true;
                                }

                                /**
                                 * \brief Returns current key.
                                 *
                                 * This function will return key of the current element, which has been
                                 * read with readFirstElement() or readNextElement() functions.
                                 * \return pointer to the current key
                                 */
                                K* getCurrentKey()
                                {
                                        if(currentUnit_ < 0)
                                                return nullptr;

                                        return (*currentElement_)->key;
                                }

                                /**
                                 * \brief Returns current data.
                                 *
                                 * This function will return data of the current element, which has been
                                 * read with readFirstElement() or readNextElement() functions.
                                 * \return pointer to the current data
                                 */
                                D* getCurrentData()
                                {
                                        if(currentUnit_ < 0)
                                                return nullptr;

                                        return &((*currentElement_)->data);
                                }

                        protected:
                                /**
                                 * Represents element.
                                 */
                                class Element
                                {
                                public:
                                        K* key;
                                        D data;
                                        bool isListed;

                                };

                                /**
                                 * \brief Adds element to the specified unit.
                                 * \param[in] element element, which will be added to the given unit
                                 * \param[in] unit unit, which will contain given element
                                 */
                                void addElement(Element* element, uint8_t unit = 0)
                                {
                                        if(element->isListed || unit >= N || currentUnit_ >= 0)
                                                return;

                                        element->isListed = true;
                                        elements_[unit].push_back(element);
                                }

                                /**
                                 * \brief Returns element by given key (creates new if needed).
                                 *
                                 * Element might be created and/or returned only if currently there are
                                 * no read operations performed, i.e. readFirstElement() function was not
                                 * called or read operation ended with multiple calls to the readNextElement() function.
                                 * \param[in] key key of the requested element
                                 * \return pointer to the element
                                 */
                                Element* requestElement(K* key)
                                {
                                        // cannot write while reading
                                        if(currentUnit_ >= 0)
                                                return nullptr;

                                        // find element
                                        auto it = elementsMap_.find(key);
                                        if(it != elementsMap_.end())
                                                return it->second.get();

                                        // try to create new element
                                        std::unique_ptr<Element> element(new(std::nothrow) Element);
                                        if(!element)
                                                return nullptr;

                                        // initialize element
                                        element->key = key;
                                        element->isListed = false;

                                        // insert element in map
                                        auto result = elementsMap_.insert(std::make_pair(key, std::move(element)));
                                        if(!result.second)
                                                return nullptr;

                                        // return created element
                                        return result.first->second.get();
                                }

                        private:
                                std::map<K*, std::unique_ptr<Element>> elementsMap_;
                                std::vector<Element*> elements_[N];
                                typename std::vector<Element*>::iterator currentElement_;
                                int16_t currentUnit_;

                        };

                        /**
                         * Represents rendering instance.
                         */
                        typedef std::pair<const Skeleton::Instance*, Actor::ViewProjectionTransform> Instance;

                        /**
                         * Represents mesh subset node.
                         */
                        class MeshSubsetNode: public Node<Mesh::Subset, std::vector<Instance>>
                        {
                        public:
                                MeshSubsetNode();
                                ~MeshSubsetNode();

                                /**
                                 * \brief Adds mesh subset.
                                 * \param[in] meshSubset mesh subset, which should be added to the node
                                 * \param[in] instance instance of the rendered entity
                                 * \return true if mesh subset has been successfully
                                 */
                                bool add(const Mesh::Subset& meshSubset, const Instance& instance);

                        };

                        /**
                         * Represents mesh node.
                         */
                        class MeshNode: public Node<Mesh, MeshSubsetNode>
                        {
                        public:
                                MeshNode();
                                ~MeshNode();

                                /**
                                 * \brief Adds mesh.
                                 * \param[in] mesh mesh, which should be added to the node
                                 * \param[in] meshSubset mesh subset, which should be added
                                 * to the child nodes
                                 * \param[in] instance instance of the rendered entity
                                 * \return true if mesh has been successfully added
                                 */
                                bool add(const Mesh& mesh, const Mesh::Subset& meshSubset,
                                         const Instance& instance);

                        };

                        /**
                         * Represents material node.
                         */
                        class MaterialNode: public Node<Material, MeshNode, NUM_OF_MATERIAL_UNITS>
                        {
                        public:
                                MaterialNode();
                                ~MaterialNode();

                                /**
                                 * \brief Adds material.
                                 * \param[in] material material, which should be added to the node
                                 * \param[in] mesh mesh, which should be added to the child nodes
                                 * \param[in] meshSubset mesh subset, which is associated with
                                 * given material
                                 * \param[in] instance instance of the rendered entity
                                 * \return true if material has been successfully added
                                 */
                                bool add(const Material& material, const Mesh& mesh,
                                         const Mesh::Subset& meshSubset,
                                         const Instance& instance);

                        };

                        /**
                         * Represents actor node.
                         */
                        class ActorNode
                        {
                        public:
                                ActorNode();
                                ~ActorNode();

                                /**
                                 * \brief Clears actor node.
                                 * \param[in] freeMemory flag, which forces memory deallocation
                                 */
                                void clear(bool freeMemory = false);

                                /**
                                 * \brief Adds actor.
                                 * \param[in] actor actor, which should be added to the node
                                 * \param[in] instance instance of the rendered entity
                                 * \return true if actor has been successfully added
                                 */
                                bool add(const Actor& actor, const Instance& instance);

                                /**
                                 * \brief Returns material node.
                                 * \param[in] unit unit of the material node
                                 * \return reference to the material node
                                 */
                                MaterialNode& getMaterialNode(uint8_t unit);

                        private:
                                MaterialNode materialNodes_[NUM_OF_MESH_UNITS];
                                MaterialNode emptyMaterialNode_;

                        };

                        /**
                         * Represents light node.
                         */
                        class LightNode: public Node<Light, ActorNode, NUM_OF_LIGHT_UNITS>
                        {
                        public:
                                LightNode();
                                ~LightNode();

                                /**
                                 * \brief Adds light.
                                 * \param[in] light light, which should be added to the node
                                 * \param[in] shadowCaster actor, which casts shadow from given light
                                 * \return true if light has been successfully added
                                 */
                                bool add(const Light& light, Actor* shadowCaster = nullptr);

                        };

                        /**
                         * Represents particle system node.
                         */
                        class ParticleSystemNode: public Node<Texture, std::vector<ParticleSystem*>, NUM_OF_PARTICLE_SYSTEM_UNITS>
                        {
                        public:
                                ParticleSystemNode();
                                ~ParticleSystemNode();

                                /**
                                 * \brief Adds particle system.
                                 * \param[in] particleSystem particle system, which should be added to the node
                                 * \return true if particle system has been successfully added
                                 */
                                bool add(const ParticleSystem& particleSystem);

                        };

                        Data();
                        ~Data();

                        /**
                         * \brief Clears data.
                         * \param[in] freeMemory flag, which forces memory deallocation
                         */
                        void clear(bool freeMemory = false);

                        /**
                         * \brief Returns actor node.
                         * \return reference to the actor node
                         */
                        ActorNode& getActorNode();

                        /**
                         * \brief Returns light node.
                         * \return reference to the light node
                         */
                        LightNode& getLightNode();

                        /**
                         * \brief Returns particle system node.
                         * \return reference to the particle system node
                         */
                        ParticleSystemNode& getParticleSystemNode();

                        /**
                         * \brief Adds actor.
                         * \param[in] actor actor, which should be rendered
                         * \return true if actor has been successfully added
                         */
                        bool addActor(const Actor& actor, const Camera& camera);

                        /**
                         * \brief Adds light.
                         * \param[in] light light, which should illuminate scene nodes
                         * \return true if light has been successfully added
                         */
                        bool addLight(const Light& light);

                        /**
                         * \brief Adds shadow.
                         * \param[in] light light, which illuminates given shadow caster
                         * \param[in] caster actor, which casts shadow from given light
                         * \return true if shadow has been successfully added
                         */
                        bool addShadow(const Light& light, const Actor& caster);

                        /**
                         * \brief Adds particle system.
                         * \param[in] particleSystem particle system, which should be rendered
                         * \return true if particle system has been successfully added
                         */
                        bool addParticleSystem(const ParticleSystem& particleSystem);

                private:
                        ActorNode actorNode_;
                        LightNode lightNode_;
                        ParticleSystemNode particleSystemNode_;

                };

                /**
                 * Represents rendering parameters.
                 */
                class Parameters
                {
                public:
                        /**
                         * \brief Constructs rendering parameters with given application, file manager,
                         * rendering area width, rendering area height, log and rendering flags.
                         * \param[in] application application
                         * \param[in] fileManager file manager
                         * \param[in] width width of the rendering area
                         * \param[in] height height of the rendering area
                         * \param[in] log log
                         * \param[in] flags rendering flags
                         */
                        Parameters(Application* application, FileManager* fileManager,
                                   uint32_t width, uint32_t height, std::ostream* log,
                                   uint8_t flags);
                        virtual ~Parameters();

                        /**
                         * \brief Returns application.
                         * \return pointer to the application
                         */
                        Application* getApplication() const;

                        /**
                         * \brief Returns file manager.
                         * \return pointer to the file manager
                         */
                        FileManager* getFileManager() const;

                        /**
                         * \brief Returns width of the rendering area.
                         * \return width of the rendering area
                         */
                        uint32_t getWidth() const;

                        /**
                         * \brief Returns height of the rendering area.
                         * \return height of the rendering area
                         */
                        uint32_t getHeight() const;

                        /**
                         * \brief Returns log.
                         * \return pointer to the log
                         */
                        std::ostream* getLog() const;

                        /**
                         * \brief Returns rendering flags.
                         * \return rendering flags
                         */
                        uint8_t getFlags() const;

                protected:
                        friend class Renderer;

                        Application* application_;
                        FileManager* fileManager_;
                        uint32_t width_, height_;
                        std::ostream* log_;
                        uint8_t flags_;

                };

                Renderer();
                virtual ~Renderer();

                /**
                 * \brief Sets GUI.
                 * \param[in] gui GUI
                 */
                void setGui(const Gui& gui);

                /**
                 * \brief Returns rendering data.
                 * \return reference to the rendering data
                 */
                Data& getData();

                /**
                 * \brief Returns rendering data.
                 * \return const reference to the rendering data
                 */
                const Data& getData() const;

                /**
                 * \brief Initializes renderer.
                 * \param[in] parameters rendering parameters
                 * \return true if renderer has been successfully initialized
                 */
                virtual bool initialize(const Parameters& parameters) = 0;

                /**
                 * \brief Destroys renderer.
                 *
                 * Clears all rendering lists and releases all helper resources.
                 */
                virtual void destroy() = 0;

                /**
                 * \brief Renders scene.
                 * \param[in] camera camera
                 */
                virtual void render(const Camera& camera) = 0;

        protected:
                Data data_;
                Gui* gui_;

        };

}

#endif
