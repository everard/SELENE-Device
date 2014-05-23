// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef RENDERER_H
#define RENDERER_H

#include "../Core/Resources/Mesh/Mesh.h"
#include "../Core/Math/Matrix.h"

#include "../Scene/Nodes/Actor.h"

#include "RenderingMemoryAllocator.h"
#include "RenderingNode.h"
#include "Effect.h"

#include <algorithm>
#include <ostream>
#include <utility>
#include <deque>
#include <new>

namespace selene
{

        /**
         * \addtogroup Rendering
         * \brief Renderer and rendering data.
         * @{
         */

        // Forward declaration of classes
        class Application;
        class FileManager;
        class Material;
        class Camera;
        class Light;
        class Gui;

        /**
         * Represents renderer. This is base class for all renderers.
         * \see Renderer::Data to obtain information about how scene objects are passed to the renderer.
         */
        class Renderer
        {
        public:
                /**
                 * Represents rendering data. It is organized in tree structure, which reduces state change during
                 * rendering. Rendering data on the top level contains ActorNode (holds instances of actors, which
                 * should be rendered), LightNode (contains lights and shadow casters from each light) and
                 * ParticleSystemNode (contains particle systems, which should be rendered).
                 * \see RenderingNode
                 *
                 * LightNode
                 * ---------
                 * Rendering element contains:
                 * - pointer to the selene::Light, which is sort key;
                 * - ActorNode, which is data (contains shadow casters).
                 *
                 * Each element can be added to one of the following units:
                 * - Renderer::Data::UNIT_LIGHT_NO_SHADOWS_DIRECTIONAL,
                 * - Renderer::Data::UNIT_LIGHT_NO_SHADOWS_POINT,
                 * - Renderer::Data::UNIT_LIGHT_NO_SHADOWS_SPOT,
                 * - Renderer::Data::UNIT_LIGHT_DIRECTIONAL,
                 * - Renderer::Data::UNIT_LIGHT_POINT,
                 * - Renderer::Data::UNIT_LIGHT_SPOT.
                 *
                 * ActorNode
                 * ---------
                 * Contains:
                 * - MaterialNode for each mesh unit:
                 *   + Renderer::Data::UNIT_MESH_STATIC,
                 *   + Renderer::Data::UNIT_MESH_SKIN.
                 *
                 * MaterialNode
                 * ------------
                 * Rendering element contains:
                 * - pointer to the selene::Material, which is sort key;
                 * - MeshNode, which is data.
                 *
                 * Each element can be added to one of the following units:
                 * - Renderer::Data::UNIT_MATERIAL_ONE_SIDED,
                 * - Renderer::Data::UNIT_MATERIAL_TWO_SIDED.
                 *
                 * MeshNode
                 * --------
                 * Rendering element contains:
                 * - pointer to the selene::Mesh, which is sort key;
                 * - MeshSubsetNode, which is data.
                 *
                 * There is only one unit.
                 *
                 * MeshSubsetNode
                 * --------------
                 * Rendering element contains:
                 * - pointer to the selene::Mesh::Subset, which is sort key;
                 * - List of instances of the actors, which is data.
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
                                NUM_OF_LIGHT_UNITS
                        };

                        /**
                         * Represents rendering list.
                         */
                        template <class T> class List
                        {
                        public:
                                /**
                                 * Represents container of the elements.
                                 */
                                typedef std::deque<T, RenderingMemoryAllocator<T>> ElementsContainer;

                                List(): elements_(RenderingMemoryAllocator<T>(Renderer::getMemoryBuffer())) {}
                                List(const List&) = delete;
                                ~List() {}
                                List& operator =(const List&) = delete;

                                /**
                                 * \brief Adds element to the rendering list.
                                 * \param[in] element element, which should be rendered
                                 * \return true if element has been successfully added to the rendering list
                                 */
                                bool addElement(const T& element)
                                {
                                        try
                                        {
                                                elements_.push_back(element);
                                                return true;
                                        }
                                        catch(...) {}

                                        return false;
                                }

                                /**
                                 * \brief Returns elements.
                                 * \return const reference to the container of the elements
                                 */
                                const ElementsContainer& getElements() const
                                {
                                        return elements_;
                                }

                        private:
                                ElementsContainer elements_;

                        };

                        /**
                         * Represents mesh subset node. Contains arrays of actor instances, sorted by
                         * mesh subsets.
                         * \see Actor::Instance MeshNode
                         */
                        class MeshSubsetNode: public RenderingNode<Mesh::Subset, List<Actor::Instance>>
                        {
                        public:
                                MeshSubsetNode();
                                MeshSubsetNode(const MeshSubsetNode&) = delete;
                                ~MeshSubsetNode();
                                MeshSubsetNode& operator =(const MeshSubsetNode&) = delete;

                                /**
                                 * \brief Adds mesh subset.
                                 * \param[in] meshSubset mesh subset, which should be added to the node
                                 * \param[in] instance instance of the rendered actor
                                 * \return true if mesh subset has been successfully added
                                 */
                                bool add(const Mesh::Subset& meshSubset, const Actor::Instance& instance);

                        };

                        /**
                         * Represents mesh node. Contains arrays of mesh subset nodes, sorted by
                         * meshes.
                         * \see MeshSubsetNode MaterialNode
                         */
                        class MeshNode: public RenderingNode<Mesh, MeshSubsetNode>
                        {
                        public:
                                MeshNode();
                                MeshNode(const MeshNode&) = delete;
                                ~MeshNode();
                                MeshNode& operator =(const MeshNode&) = delete;

                                /**
                                 * \brief Adds mesh.
                                 * \param[in] mesh mesh, which should be added to the node
                                 * \param[in] meshSubset mesh subset, which should be added
                                 * to the child nodes
                                 * \param[in] instance instance of the rendered actor
                                 * \return true if mesh has been successfully added
                                 */
                                bool add(const Mesh& mesh, const Mesh::Subset& meshSubset,
                                         const Actor::Instance& instance);

                        };

                        /**
                         * Represents material node. Contains arrays of mesh nodes, sorted by
                         * materials and ordered by material units.
                         * \see MeshNode ActorNode
                         */
                        class MaterialNode: public RenderingNode<Material, MeshNode, NUM_OF_MATERIAL_UNITS>
                        {
                        public:
                                MaterialNode();
                                MaterialNode(const MaterialNode&) = delete;
                                ~MaterialNode();
                                MaterialNode& operator =(const MaterialNode&) = delete;

                                /**
                                 * \brief Adds material.
                                 * \param[in] material material, which should be added to the node
                                 * \param[in] mesh mesh, which should be added to the child nodes
                                 * \param[in] meshSubset mesh subset, which is associated with
                                 * given material
                                 * \param[in] instance instance of the rendered actor
                                 * \return true if material has been successfully added
                                 */
                                bool add(const Material& material, const Mesh& mesh,
                                         const Mesh::Subset& meshSubset,
                                         const Actor::Instance& instance);

                        };

                        /**
                         * Represents actor node. Contains material nodes, ordered by mesh units.
                         * \see MaterialNode
                         */
                        class ActorNode
                        {
                        public:
                                ActorNode();
                                ActorNode(const ActorNode&) = delete;
                                ~ActorNode();
                                ActorNode& operator =(const ActorNode&) = delete;

                                /**
                                 * \brief Clears actor node.
                                 */
                                void clear();

                                /**
                                 * \brief Adds actor.
                                 * \param[in] actor actor, which should be added to the node
                                 * \param[in] instance instance of the rendered actor
                                 * \return true if actor has been successfully added
                                 */
                                bool add(const Actor& actor, const Actor::Instance& instance);

                                /**
                                 * \brief Returns material node.
                                 * \param[in] unit rendering unit of the material
                                 * \return reference to the material node
                                 */
                                MaterialNode& getMaterialNode(uint8_t unit);

                        private:
                                MaterialNode materialNodes_[NUM_OF_MESH_UNITS];
                                MaterialNode emptyMaterialNode_;

                        };

                        /**
                         * Represents light node. Contains arrays of actor nodes (which hold shadows),
                         * sorted by lights and ordered by light units.
                         * \see ActorNode
                         */
                        class LightNode: public RenderingNode<Light, ActorNode, NUM_OF_LIGHT_UNITS>
                        {
                        public:
                                LightNode();
                                LightNode(const LightNode&) = delete;
                                ~LightNode();
                                LightNode& operator =(const LightNode&) = delete;

                                /**
                                 * \brief Adds light.
                                 * \param[in] light light, which should be added to the node
                                 * \param[in] shadowCaster actor, which casts shadow from given light
                                 * \return true if light has been successfully added
                                 */
                                bool add(const Light& light, Actor* shadowCaster = nullptr);

                        };

                        Data();
                        Data(const Data&) = delete;
                        ~Data();
                        Data& operator =(const Data&) = delete;

                        /**
                         * \brief Sets camera, which point of view will be used to render the scene.
                         * \param[in] camera camera, which point of view will be used to render the scene
                         */
                        void setCamera(const Camera& camera);

                        /**
                         * \brief Clears data.
                         */
                        void clear();

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
                         * \brief Adds actor.
                         * \param[in] actor actor, which should be rendered
                         * \return true if actor has been successfully added
                         */
                        bool addActor(const Actor& actor);

                        /**
                         * \brief Adds light.
                         * \param[in] light light, which should illuminate scene nodes
                         * \return true if light has been successfully added
                         */
                        bool addLight(const Light& light);

                        /**
                         * \brief Adds shadow.
                         * \param[in] light light, which illuminates given shadow caster
                         * \param[in] shadowCaster actor, which casts shadow from given light
                         * \return true if shadow has been successfully added
                         */
                        bool addShadow(const Light& light, const Actor& shadowCaster);

                private:
                        ActorNode actorNode_;
                        LightNode lightNode_;
                        Camera* camera_;

                };

                /**
                 * Represents rendering parameters.
                 */
                class Parameters
                {
                public:
                        /**
                         * \brief Constructs rendering parameters.
                         * \param[in] application application
                         * \param[in] fileManager file manager
                         * \param[in] width width of the rendering area
                         * \param[in] height height of the rendering area
                         * \param[in] log log
                         * \param[in] isFullScreenEnabledFlag flag which controls full-screen mode
                         */
                        Parameters(Application* application, FileManager* fileManager,
                                   uint32_t width, uint32_t height, std::ostream* log,
                                   bool isFullScreenEnabledFlag);
                        Parameters(const Parameters&) = default;
                        virtual ~Parameters();
                        Parameters& operator =(const Parameters&) = default;

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
                         * \brief Returns true if full-screen mode is enabled.
                         * \return true if full-screen mode is enabled
                         */
                        bool isFullScreenEnabled() const;

                protected:
                        Application* application_;
                        FileManager* fileManager_;

                        uint32_t width_, height_;
                        std::ostream* log_;

                        bool isFullScreenEnabledFlag_;

                };

                /**
                 * Represents list of the effects.
                 */
                typedef std::vector<Effect> EffectsList;

                Renderer();
                Renderer(const Renderer&) = delete;
                virtual ~Renderer();
                Renderer& operator =(const Renderer&) = delete;

                /**
                 * \brief Returns list of the effects.
                 * \return list of the effects
                 */
                const EffectsList& getEffects() const;

                /**
                 * \brief Initializes renderer.
                 * \param[in] parameters rendering parameters
                 * \return true if renderer has been successfully initialized
                 */
                virtual bool initialize(const Parameters& parameters) = 0;

                /**
                 * \brief Destroys renderer.
                 *
                 * Releases all helper resources.
                 */
                virtual void destroy() = 0;

                /**
                 * \brief Renders scene.
                 * \param[in] camera camera, which point of view will be used in rendering
                 */
                virtual void render(const Camera& camera) = 0;

                /**
                 * \brief Initializes memory buffer.
                 * \param[in] size size of the memory buffer in bytes
                 * \return true if memory buffer has been successfully initialized
                 */
                static bool initializeMemoryBuffer(std::size_t size);

                /**
                 * \brief Destroys memory buffer.
                 */
                static void destroyMemoryBuffer();

                /**
                 * \brief Returns memory buffer.
                 * \return reference to the memory buffer
                 */
                static RenderingMemoryBuffer& getMemoryBuffer();

        protected:
                Parameters parameters_;
                EffectsList effectsList_;

        private:
                static RenderingMemoryBuffer memoryBuffer_;

        };

        /**
         * @}
         */

}

#endif
