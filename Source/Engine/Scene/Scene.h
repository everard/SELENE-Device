// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef SCENE_H
#define SCENE_H

#include "../Core/Resources/Mesh/Skeleton.h"
#include "../Core/Entity/Entity.h"
#include "../Core/Status/Status.h"
#include "../Core/Math/Matrix.h"
#include "../Core/Math/Sphere.h"

#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace selene
{

        /**
         * \addtogroup Scene
         * \brief Scene management and scene objects.
         * @{
         */

        // Forward declaration of classes
        class ParticleSystem;
        class Renderer;
        class Camera;
        class Light;
        class Actor;

        /**
         * Represents scene. Manages scene nodes, such as actors, lights, cameras and particle systems.
         * The following piece of code shows how to use scene.
         * \code
         * // somewhere in program scene must be declared
         * selene::Scene scene;
         *
         * // new object in scene must be created with operator new
         * scene.addNode(new(std::nothrow) selene::Camera("main camera"));
         * // note that if addNode method returns false, then object, which has been passed to it,
         * // is already destroyed, so no manual destruction is needed
         *
         * // now added object can be obtained from the scene
         * auto camera = scene.getCamera("main camera");
         *
         * // nodes can be destroyed with removeNode methods
         * scene.removeActor("north wall");
         * scene.removeLight("lamp");
         * scene.removeCamera("main camera");
         * scene.removeParticleSystem("dust");
         * \endcode
         * \see Scene::Node Actor Light Camera ParticleSystem
         */
        class Scene
        {
        public:
                /**
                 * Represents scene node. This is base class for all scene objects.
                 *
                 * Nodes can be attached to each other. In this case one node becomes parent of another.
                 * Child node inherits parent node's transform. If parent node has skeleton, then child
                 * node can be attached to one of the parent's bones.
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
                         * \brief Sets position of the node in local space.
                         *
                         * If current node is a child of any other node, then current node's position is
                         * defined in parent node's space.
                         * \param[in] position position of the node in local space
                         */
                        void setPosition(const Vector3d& position);

                        /**
                         * \brief Returns position of the node in world space.
                         *
                         * It means that if current node is attached to another, then current node's
                         * local position is multiplied by parent node's transform matrix, producing
                         * world space position.
                         * \return position of the node in world space
                         */
                        const Vector3d& getPosition() const;

                        /**
                         * \brief Sets rotation of the node in local space.
                         * \see setPosition
                         * \param[in] rotation rotation of the node in local space
                         */
                        void setRotation(const Quaternion& rotation);

                        /**
                         * \brief Returns rotation of the node in world space.
                         * \see getPosition
                         * \return rotation of the node in world space
                         */
                        const Quaternion& getRotation() const;

                        /**
                         * \brief Sets scale of the node in local space.
                         * \see setPosition
                         * \param[in] scale scale of the node in local space
                         */
                        void setScale(const Vector3d& scale);

                        /**
                         * \brief Returns scale of the node in world space.
                         * \see getPosition
                         * \return scale of the node in world space
                         */
                        const Vector3d& getScale() const;

                        /**
                         * \brief Attaches current node to the target node.
                         * \param[in] node target node, to which current node will be attached
                         * \param[in] boneName name of the bone in target node, to which current
                         * node will be attached
                         * \return true if attachment has been successfully done
                         */
                        bool attach(Node& node, const char* boneName = nullptr);

                        /**
                         * \brief Detaches current node from its parent.
                         * \return true if current node has been successfully detached
                         */
                        bool detach();

                        /**
                         * \brief Returns world matrix.
                         * \return world matrix
                         */
                        const Matrix& getWorldMatrix() const;

                        /**
                         * \brief Returns rendering unit.
                         * \return rendering unit
                         * \see Renderer::Data
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

                        mutable Vector3d   positions_[NUM_OF_INDICES];
                        mutable Quaternion rotations_[NUM_OF_INDICES];
                        mutable Vector3d   scale_[NUM_OF_INDICES];
                        mutable Matrix worldMatrix_;

                        Skeleton::Instance* skeletonInstance_;

                        int32_t boneIndex_;
                        Node* parentNode_;
                        std::unordered_set<Node*> childNodes_;

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
                 * \brief Sets active camera.
                 * \param[in] name name of the camera, which must become active
                 * \return true if camera with given name has been activated
                 */
                bool setActiveCamera(const char* name);

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
                 * \brief Returns number of cameras.
                 * \return number of cameras
                 */
                size_t getNumCameras() const;

                /**
                 * \brief Returns number of particle systems.
                 * \return number of particle systems
                 */
                size_t getNumParticleSystems() const;

                /**
                 * \brief Adds node.
                 * \param[in] node node, which must be added to the scene
                 * \return true if node has been successfully added, otherwise node
                 * has been deleted and no manual memory deallocation is needed
                 */
                bool addNode(Node* node);

                /**
                 * \brief Removes actor.
                 * \param[in] name name of the actor
                 * \return true if actor has been successfully removed
                 */
                bool removeActor(const char* name);

                /**
                 * \brief Removes light.
                 * \param[in] name name of the light
                 * \return true if light has been successfully removed
                 */
                bool removeLight(const char* name);

                /**
                 * \brief Removes camera
                 * \param[in] name name of the camera
                 * \return true if camera has been successfully removed
                 */
                bool removeCamera(const char* name);

                /**
                 * \brief Removes particle system.
                 * \param[in] name name of the particle system
                 * \return true if particle system has been successfully removed
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
                 * \brief Returns camera.
                 * \param[in] name name of the camera
                 * \return std::weak_ptr to the camera
                 */
                std::weak_ptr<Camera> getCamera(const char* name);

                /**
                 * \brief Returns particle system.
                 * \param[in] name name of the particle system
                 * \return std::weak_ptr to the particle system
                 */
                std::weak_ptr<ParticleSystem> getParticleSystem(const char* name);

                /**
                 * \brief Updates and renders scene.
                 * \param[in] elapsedTime elapsed time since last render
                 * \param[in] renderer renderer, which renders scene
                 * \return true if rendering has been successfully performed
                 */
                bool updateAndRender(float elapsedTime, Renderer& renderer);

        private:
                std::weak_ptr<Camera> activeCamera_;

                std::unordered_map<std::string, std::shared_ptr<Actor>> actors_;
                std::unordered_map<std::string, std::shared_ptr<Light>> lights_;
                std::unordered_map<std::string, std::shared_ptr<Camera>> cameras_;
                std::unordered_map<std::string, std::shared_ptr<ParticleSystem>> particleSystems_;

                uint32_t numVisibleActors_, numVisibleLights_;
                uint32_t numVisibleParticleSystems_;

        };

        /**
         * @}
         */

}

#endif
