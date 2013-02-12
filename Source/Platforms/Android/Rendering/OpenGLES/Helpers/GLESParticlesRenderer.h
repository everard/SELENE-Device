// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

/*#ifndef D3D9_PARTICLES_RENDERER_H
#define D3D9_PARTICLES_RENDERER_H

#include "../../../../../Engine/Rendering/Renderer.h"
#include "D3D9Shader.h"

namespace selene
{*/

        /**
         * \addtogroup Windows
         * @{
         */

        // Forward declaration of classes
        /*class D3d9RenderTargetContainer;
        class D3d9FrameParameters;
        class D3d9TextureHandler;
        class D3d9Capabilities;*/

        /**
         * Represents D3D9 particles renderer.
         */
        /*class D3d9ParticlesRenderer
        {
        public:
                D3d9ParticlesRenderer();
                ~D3d9ParticlesRenderer();*/

                /**
                 * \brief Initializes particles renderer.
                 * \param[in] renderTargetContainer render target container
                 * \param[in] frameParameters frame parameters
                 * \param[in] textureHandler texture handler
                 * \param[in] capabilities D3D capabilities
                 * \return true if particles renderer has been successfully initialized
                 */
                /*bool initialize(D3d9RenderTargetContainer& renderTargetContainer,
                                D3d9FrameParameters& frameParameters,
                                D3d9TextureHandler& textureHandler,
                                D3d9Capabilities& capabilities);*/

                /**
                 * \brief Destroys particles renderer.
                 */
                //void destroy();

                /**
                 * \brief Renders particle systems.
                 * \param[in] particleSystemNode particle system node
                 */
                //void renderParticleSystems(Renderer::Data::ParticleSystemNode& particleSystemNode);

        //private:
                /// Helper constants
                /*enum
                {
                        VERTEX_SHADER_PARTICLES_PASS = 0,
                        NUM_OF_VERTEX_SHADERS,

                        PIXEL_SHADER_PARTICLES_PASS = 0,
                        NUM_OF_PIXEL_SHADERS,

                        LOCATION_PROJECTION_MATRIX = 0,
                        LOCATION_VIEW_MATRIX = 4,

                        LOCATION_TEXTURE_COORDINATES_ADJUSTMENT = 0,
                        LOCATION_UNPROJECTION_VECTOR = 1,
                        LOCATION_PROJECTION_PARAMETERS = 2,

                        LOCATION_POSITIONS_MAP = 0,
                        LOCATION_PARTICLE_TEXTURE = 1
                };

                D3d9VertexShader vertexShaders_[NUM_OF_VERTEX_SHADERS];
                D3d9PixelShader pixelShaders_[NUM_OF_PIXEL_SHADERS];

                LPDIRECT3DVERTEXDECLARATION9 d3dVertexDeclaration_;
                LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffer_;
                LPDIRECT3DDEVICE9 d3dDevice_;

                D3d9RenderTargetContainer* renderTargetContainer_;
                D3d9FrameParameters* frameParameters_;
                D3d9TextureHandler* textureHandler_;
                D3d9Capabilities* capabilities_;*/

                /**
                 * \brief Renders particle system.
                 * \param[in] particleSystem particle system, which will be rendered
                 */
                //void renderParticleSystem(ParticleSystem* particleSystem);

                /**
                 * \brief Renders particles.
                 * \param[in] numParticles number of particles
                 */
                //void renderParticles(uint32_t numParticles);

        //};

        /**
         * @}
         */

/*}

#endif*/
