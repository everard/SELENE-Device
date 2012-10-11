// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef RENDERER_H
#define RENDERER_H

#include "../Core/Resources/Mesh/Mesh.h"
#include "../Core/Math/Matrix.h"

#include <algorithm>
#include <ostream>
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
        class Camera;
        class Actor;
        class Light;
        class Gui;

        /**
         * Represents renderer. This is base class for all renderers.
         */
        class Renderer
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

                        UNIT_LIGHT_DIRECTIONAL = 0,
                        UNIT_LIGHT_POINT,
                        UNIT_LIGHT_SPOT,
                        NUM_OF_LIGHT_UNITS,

                        UNIT_PARTICLE_SYSTEM = 0,
                        NUM_OF_PARTICLE_SYSTEM_UNITS
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

                        // Application
                        Application* application_;

                        // File manager
                        FileManager* fileManager_;

                        // Width and height
                        uint32_t width_, height_;

                        // Log
                        std::ostream* log_;

                        // Flags
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
                 * \brief Clears lists.
                 * \param[in] freeMemory flag which forces memory deallocation
                 */
                void clearLists(bool freeMemory = false);

                /**
                 * \brief Adds actor.
                 * \param[in] actor actor
                 * \return true if actor was successfully added to the rendering lists
                 */
                bool addActor(const Actor& actor);

                /**
                 * \brief Adds actor.
                 * \param[in] light light which illuminates given actor
                 * \param[in] actor actor
                 * \return true if actor was successfully added to the rendering lists
                 * of given light
                 */
                bool addActor(const Light& light, const Actor& actor);

                /**
                 * \brief Adds light.
                 * \param[in] light light
                 * \return true if light was successfully added to the rendering lists
                 */
                bool addLight(const Light& light);

                /**
                 * \brief Adds shadow.
                 * \param[in] light light which illuminates given shadow caster
                 * \param[in] caster actor which casts shadow
                 * \return true if shadow was successfully added to the rendering lists
                 */
                bool addShadow(const Light& light, const Actor& caster);

                /**
                 * \brief Adds particle system.
                 * \param[in] particleSystem particle system
                 * \return true if particle system was successfully added to the rendering lists
                 */
                bool addParticleSystem(const ParticleSystem& particleSystem);

                /**
                 * \brief Initializes renderer.
                 * \param[in] parameters rendering parameters
                 * \return true if renderer was successfully initialized
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
                /// Lights rendering list types
                enum
                {
                        LIGHTS_LIST_WITHOUT_SHADOWS = 0,
                        LIGHTS_LIST_WITH_SHADOWS,
                        NUM_OF_LIGHTS_LIST_TYPES
                };

                /**
                 * Represents rendering list, where K stands for key element, D - for data element and
                 * N - for number of rendering units. For example, list with K as string, D as vector
                 * of ints and N = 2 will have the following structure.
                 *
                 * | Key     | Data                           |
                 * |:-------:|--------------------------------|
                 * |string[0]|-vector0[0] -- [int0, int1, ...]|
                 * |         |-vector0[1] -- [int0, int1, ...]|
                 * |string[1]|-vector1[0] -- [int0, int1, ...]|
                 * |         |-vector1[1] -- [int0, int1, ...]|
                 * |   ...   |                                |
                 * |string[N]|-vectorN[0] -- [int0, int1, ...]|
                 * |         |-vectorN[1] -- [int0, int1, ...]|
                 */
                template <class K, class D, uint8_t N> class List
                {
                public:
                        List()
                        {
                                for(uint8_t i = 0; i < N; ++i)
                                        elements_[i].reserve(100);

                                currentUnit_ = -1;
                        }
                        ~List()
                        {
                                clear(true);
                        }

                        /**
                         * \brief Clears list.
                         * \param[in] freeMemory flag which forces memory deallocation
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
                         * \return true if first element from specified unit was successfully read
                         */
                        bool readFirstElement(uint8_t unit)
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
                         * \return true if next element from current unit was successfully read,
                         * false if reading of current unit was ended
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
                         * This function will return key of the current element, which was
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
                         * This function will return data of the current element, which was
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
                         * \param[in] element element
                         * \param[in] unit unit
                         */
                        void addElement(Element* element, uint8_t unit)
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
                         * \param[in] key key
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
                        // Elements map
                        std::map<K*, std::unique_ptr<Element>> elementsMap_;

                        // Elements
                        std::vector<Element*> elements_[N];

                        // Current element
                        typename std::vector<Element*>::iterator currentElement_;

                        // Current unit
                        int16_t currentUnit_;

                };

                /**
                 * Represents mesh subsets rendering list.
                 */
                class MeshSubsetsList: public List<Mesh::Subset, std::vector<Actor*>, NUM_OF_MATERIAL_UNITS>
                {
                public:
                        MeshSubsetsList();
                        ~MeshSubsetsList();

                        /**
                         * \brief Adds mesh subset.
                         * \param[in] meshSubset mesh subset
                         * \param[in] actor actor which possesses given mesh subset
                         * \return true if mesh subset was successfully added
                         */
                        bool addMeshSubset(const Mesh::Subset& meshSubset, const Actor& actor);

                };

                /**
                 * Represents actors rendering list.
                 */
                class ActorsList: public List<Mesh, MeshSubsetsList, NUM_OF_MESH_UNITS>
                {
                public:
                        ActorsList();
                        ~ActorsList();

                        /**
                         * \brief Adds actor.
                         * \param[in] actor actor
                         * \return true if actor was successfully added
                         */
                        bool addActor(const Actor& actor);

                };

                /**
                 * Represents light rendering data.
                 */
                class LightData
                {
                public:
                        LightData();
                        ~LightData();

                        /**
                         * \brief Clears data.
                         */
                        void clear();

                        /**
                         * \brief Returns actors list.
                         * \return reference to the list of actors
                         */
                        ActorsList& getActorsList();

                        /**
                         * \brief Returns shadows list.
                         * \return reference to the list of shadows
                         */
                        ActorsList& getShadowsList();

                private:
                        // Rendering lists
                        ActorsList actorsList_;
                        ActorsList shadowsList_;

                };

                /**
                 * Represents lights rendering list.
                 */
                class LightsList: public List<Light, LightData, NUM_OF_LIGHT_UNITS>
                {
                public:
                        LightsList();
                        ~LightsList();

                        /**
                         * \brief Adds light.
                         * \param[in] light light
                         * \param[in] actor actor which is illuminated by given light
                         * \param[in] shadow actor which casts shadow from given light
                         * \return true if light was successfully added
                         */
                        bool addLight(const Light& light, Actor* actor = nullptr, Actor* shadow = nullptr);

                };

                /**
                 * Represents particle systems rendering list.
                 */
                class ParticleSystemsList: public List<Texture, std::vector<ParticleSystem*>, NUM_OF_PARTICLE_SYSTEM_UNITS>
                {
                public:
                        ParticleSystemsList();
                        ~ParticleSystemsList();

                        /**
                         * \brief Adds particle system.
                         * \param[in] particleSystem particle system
                         * \return true if particle system was successfully added
                         */
                        bool addParticleSystem(const ParticleSystem& particleSystem);

                };

                // Rendering lists
                ActorsList actorsList_;
                LightsList lightsLists_[NUM_OF_LIGHTS_LIST_TYPES];
                ParticleSystemsList particleSystemsList_;

                // GUI
                Gui* gui_;

        };

}

#endif
