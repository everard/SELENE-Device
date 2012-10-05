// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef SCENE_H
#define SCENE_H

#include "../Core/Resources/Mesh/Skeleton.h"
#include "../Core/Entity/Entity.h"
#include "../Core/Status/Status.h"
#include "../Core/Math/Matrix.h"
#include "../Core/Math/Sphere.h"

#include <memory>
#include <map>
#include <set>

namespace selene
{

        // Forward declaration of classes
        class ParticleSystem;
        class Renderer;
        class Camera;
        class Light;
        class Actor;

        /**
         * Represents scene.
         */
        class Scene
        {
        public:
                /**
                 * Represents scene node.
                 */
                class Node: public Entity, public Status
                {
                public:
                        /// Flags
                        enum
                        {
                                SHADOW_CASTER = 0x01,
                                DYNAMIC       = 0x02,
                                UPDATED       = 0x04,
                                HIDDEN        = 0x08
                        };

                        /**
                         * \brief Constructs node with given name.
                         * \param[in] name name of the node
                         */
                        Node(const char* name);
                        virtual ~Node();

                        /**
                         * \brief Sets position.
                         * \param[in] position position of the node in local space
                         */
                        void setPosition(const Vector3d& position);

                        /**
                         * \brief Returns position.
                         * \return position of the node in world space
                         */
                        const Vector3d& getPosition() const;

                        /**
                         * \brief Sets rotation.
                         * \param[in] rotation rotation of the node in local space
                         */
                        void setRotation(const Quaternion& rotation);

                        /**
                         * \brief Returns rotation.
                         * \return rotation of the node in world space
                         */
                        const Quaternion& getRotation() const;

                        /**
                         * \brief Sets scale.
                         * \param[in] scale scale of the node in local space
                         */
                        void setScale(const Vector3d& scale);

                        /**
                         * \brief Returns scale.
                         * \return scale of the node in world space
                         */
                        const Vector3d& getScale() const;

                        /**
                         * \brief Attaches to node.
                         * \param[in] node target node, to which current node will be attached
                         * \param[in] boneName name of the bone in target node, to which current
                         * node will be attached
                         * \return true if attachment was successfully done
                         */
                        bool attach(Node& node, const char* boneName = nullptr);

                        /**
                         * \brief Detaches from node.
                         */
                        void detach();

                        /**
                         * \brief Returns world matrix.
                         * \return world matrix
                         */
                        const Matrix& getWorldMatrix() const;

                        /**
                         * \brief Returns rendering unit.
                         * \return rendering unit
                         */
                        virtual int16_t getRenderingUnit() const = 0;

                        /**
                         * \brief Determines relation between node and volume.
                         * \param[in] volume volume
                         * \return INSIDE if node is inside the volume, OUTSIDE if node is outside
                         * the volume and INTERSECTS if node intersects the volume
                         */
                        virtual RELATION determineRelation(const Volume& volume) const = 0;

                protected:
                        /// Indices
                        enum
                        {
                                ORIGINAL = 0,
                                MODIFIED,
                                NUM_OF_INDICES
                        };

                        // Positions
                        mutable Vector3d positions_[NUM_OF_INDICES];

                        // Rotations
                        mutable Quaternion rotations_[NUM_OF_INDICES];

                        // Scale
                        mutable Vector3d scale_[NUM_OF_INDICES];

                        // World matrix
                        mutable Matrix worldMatrix_;

                        // Skeleton
                        Skeleton* skeleton_;

                        // Bone index
                        int32_t boneIndex_;

                        // Parent node
                        Node* parentNode_;

                        // Child nodes
                        std::set<Node*> childNodes_;

                        /**
                         * \brief Requests update operation.
                         */
                        void requestUpdateOperation() const;

                        /**
                         * \brief Requests update operation for child nodes.
                         */
                        void requestChildNodesUpdateOperation() const;

                        /**
                         * \brief Performs update operation.
                         */
                        void performUpdateOperation() const;

                        /**
                         * \brief Returns true if current node has given child.
                         * \param[in] childNode child node
                         * \return true if current node has given child
                         */
                        bool hasChild(Node* childNode);

                        /**
                         * \brief Updates node.
                         */
                        virtual void update() const = 0;

                };

                Scene();
                ~Scene();

                /**
                 * \brief Destroys scene.
                 */
                void destroy();

                /**
                 * \brief Sets camera.
                 * \param[in] camera camera
                 */
                void setCamera(Camera* camera);

                /**
                 * \brief Returns number of visible actors.
                 * \return number of visible actors
                 */
                uint32_t getNumVisibleActors() const;

                /**
                 * \brief Returns number of visible lights.
                 * \return number of visible lights
                 */
                uint32_t getNumVisibleLights() const;

                /**
                 * \brief Returns number of visible particle systems.
                 * \return number of visible particle systems
                 */
                uint32_t getNumVisibleParticleSystems() const;

                /**
                 * \brief Returns number of actors.
                 * \return number of actors
                 */
                size_t getNumActors() const;

                /**
                 * \brief Returns number of lights.
                 * \return number of lights
                 */
                size_t getNumLights() const;

                /**
                 * \brief Returns number of particle systems.
                 * \return number of particle systems
                 */
                size_t getNumParticleSystems() const;

                /**
                 * \brief Adds node.
                 * \param[in] node node
                 * \return true if node was successfully added, otherwise node
                 * is deleted and no manual memory deallocation is needed
                 */
                bool addNode(Node* node);

                /**
                 * \brief Removes actor.
                 * \param[in] name name of the actor
                 * \return true if actor was successfully deleted
                 */
                bool removeActor(const char* name);

                /**
                 * \brief Removes light.
                 * \param[in] name name of the light
                 * \return true if light was successfully deleted
                 */
                bool removeLight(const char* name);

                /**
                 * \brief Removes particle system.
                 * \param[in] name name of the particle system
                 * \return true if particle system was successfully deleted
                 */
                bool removeParticleSystem(const char* name);

                /**
                 * \brief Returns actor.
                 * \param[in] name name of the actor
                 * \return std::weak_ptr to the actor
                 */
                std::weak_ptr<Actor> getActor(const char* name);

                /**
                 * \brief Returns light.
                 * \param[in] name name of the light
                 * \return std::weak_ptr to the light
                 */
                std::weak_ptr<Light> getLight(const char* name);

                /**
                 * \brief Returns particle system.
                 * \param[in] name name of the particle system
                 * \return std::weak_ptr to the particle system
                 */
                std::weak_ptr<ParticleSystem> getParticleSystem(const char* name);

                /**
                 * \brief Updates and renders scene.
                 * \param[in] elapsedTime elapsed time since last render
                 * \param[in] renderer renderer
                 * \return true if rendering was successfully performed
                 */
                bool updateAndRender(float elapsedTime, Renderer& renderer);

        private:
                // Camera
                Camera* camera_;

                // Nodes
                std::map<std::string, std::shared_ptr<Actor>> actors_;
                std::map<std::string, std::shared_ptr<Light>> lights_;
                std::map<std::string, std::shared_ptr<ParticleSystem>> particleSystems_;

                // Number of visible actors, lights and particle systems
                uint32_t numVisibleActors_, numVisibleLights_;
                uint32_t numVisibleParticleSystems_;

        };

}

#endif
