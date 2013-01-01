// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef ACTOR_H
#define ACTOR_H

#include "../../Core/Resources/MeshAnimation/MeshAnimationProcessor.h"
#include "../../Core/Resources/Mesh/Mesh.h"
#include "../../Core/Math/Matrix.h"
#include "../../Core/Math/Volume.h"
#include "../../Core/Math/Sphere.h"
#include "../../Core/Math/Box.h"
#include "../Scene.h"

namespace selene
{

        /**
         * \addtogroup Scene
         * @{
         */

        // Forward declaration of classes
        class Camera;

        /**
         * Represents scene actor. This is a body in 3D space, whose appearance is determined
         * by the Mesh. Actor also contains MeshAnimationProcessor, which is used in skeletal
         * animation of the mesh (if it has skeleton).
         */
        class Actor: public Scene::Node
        {
        public:
                /**
                 * Represents view-projection transform.
                 */
                class ViewProjectionTransform
                {
                public:
                        ViewProjectionTransform();
                        ~ViewProjectionTransform();

                        /**
                         * \brief Computes view-projection transform.
                         * \param[in] actor actor, which holds world matrix
                         * \param[in] viewMatrix view matrix
                         * \param[in] viewProjectionMatrix view-projection matrix
                         */
                        void compute(const Actor& actor, const Matrix& viewMatrix,
                                     const Matrix& viewProjectionMatrix);

                        /**
                         * \brief Returns world-view-projection matrix.
                         * \return world-view-projection matrix
                         */
                        const Matrix& getWorldViewProjectionMatrix() const;

                        /**
                         * \brief Returns world-view matrix.
                         * \return world-view matrix
                         */
                        const Matrix& getWorldViewMatrix() const;

                        /**
                         * \brief Returns normals matrix.
                         * \return normals matrix
                         */
                        const Matrix& getNormalsMatrix() const;

                private:
                        Matrix worldViewProjectionMatrix_;
                        Matrix worldViewMatrix_;
                        Matrix normalsMatrix_;

                };

                /**
                 * \brief Constructs actor with given name, mesh, position, rotation and scale.
                 * \param[in] name name of the actor
                 * \param[in] mesh mesh, which represents actor in scene
                 * \param[in] position position of the actor
                 * \param[in] rotation rotation of the actor
                 * \param[in] scale scale of the actor
                 */
                Actor(const char* name,
                      const Resource::Instance<Mesh>& mesh = Resource::Instance<Mesh>(),
                      const Vector3d& position = Vector3d(),
                      const Quaternion& rotation = Quaternion(),
                      const Vector3d& scale = Vector3d(1.0f, 1.0f, 1.0f));
                virtual ~Actor();

                /**
                 * \brief Sets mesh.
                 * \param[in] mesh instance of the mesh
                 */
                void setMesh(const Resource::Instance<Mesh>& mesh);

                /**
                 * \brief Returns mesh.
                 * \return const reference to the mesh instance
                 */
                const Resource::Instance<Mesh>& getMesh() const;

                /**
                 * \brief Returns skeleton instance.
                 * \return const reference to the skeleton instance
                 */
                const Skeleton::Instance& getSkeletonInstance() const;

                /**
                 * \brief Adds mesh animation.
                 * \see MeshAnimationProcessor::addMeshAnimation
                 * \param[in] meshAnimation mesh animation instance
                 * \param[in] blendFactorTransitionTime specifies how much time it takes to change blend factor
                 * \param[in] startingTransitionTime specifies how much time it takes to start the animation
                 * \param[in] stoppingTransitionTime specifies how much time it takes to stop the animation
                 * \param[in] animationTime animation time
                 * \param[in] blendFactor blend factor (if set to zero, then current animation has no effect
                 * on skeleton)
                 * \return true if mesh animation has been successfully added
                 */
                bool addMeshAnimation(const Resource::Instance<MeshAnimation>& meshAnimation,
                                      float blendFactorTransitionTime,
                                      float startingTransitionTime,
                                      float stoppingTransitionTime,
                                      float animationTime,
                                      float blendFactor);

                /**
                 * \brief Removes mesh animation.
                 * \see MeshAnimationProcessor::removeMeshAnimation
                 * \param[in] index index of the animation, which will be removed
                 * \return true if animation has been successfully removed
                 */
                bool removeMeshAnimation(uint32_t index);

                /**
                 * \brief Returns mesh animation.
                 * \see MeshAnimationProcessor::getMeshAnimation
                 * \param[in] index index of the animation
                 * \return reference to the mixable mesh animation
                 */
                MeshAnimationProcessor::MixableMeshAnimation& getMeshAnimation(uint32_t index);

                /**
                 * \brief Processes mesh animations.
                 * \see MeshAnimationProcessor::processMeshAnimations
                 * \param[in] elapsedTime elapsed time since last processing
                 */
                void processMeshAnimations(float elapsedTime);

                /**
                 * \brief Returns bounding box.
                 * \return bounding box
                 */
                const Box& getBoundingBox() const;

                /**
                 * \brief Returns rendering unit.
                 * \return -1 if actor has no mesh, Renderer::Data::UNIT_MESH_STATIC if actor's mesh is static,
                 * Renderer::Data::UNIT_MESH_SKIN if actor's mesh has skeleton
                 */
                int16_t getRenderingUnit() const;

                /**
                 * \brief Determines relation between actor and volume.
                 * \param[in] volume volume
                 * \return INSIDE if bounding box of the actor is inside the volume,
                 * OUTSIDE if bounding box is outside the volume, INTERSECTS if
                 * bounding box intersects the volume
                 */
                RELATION determineRelation(const Volume& volume) const;

        protected:
                /**
                 * \brief Updates actor.
                 */
                void update() const;

        private:
                MeshAnimationProcessor meshAnimationProcessor_;
                mutable Box boundingBoxes_[NUM_OF_INDICES];
                Resource::Instance<Mesh> mesh_;
                int16_t renderingUnit_;

        };

        /**
         * @}
         */

}

#endif
