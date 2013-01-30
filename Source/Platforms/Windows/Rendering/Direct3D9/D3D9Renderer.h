// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef D3D9_RENDERER_H
#define D3D9_RENDERER_H

#include "../../../../Engine/Rendering/Renderer.h"
#include "../../../../Engine/Core/Resources/Mesh/Mesh.h"
#include "../../../../Engine/Core/Status/Status.h"

#include "Helpers/D3D9RenderTargetContainer.h"
#include "Helpers/D3D9ParticlesRenderer.h"
#include "Helpers/D3D9LightingRenderer.h"
#include "Helpers/D3D9FrameParameters.h"
#include "Helpers/D3D9ActorsRenderer.h"
#include "Helpers/D3D9FullScreenQuad.h"
#include "Helpers/D3D9TextureHandler.h"
#include "Helpers/D3D9BloomRenderer.h"
#include "Helpers/D3D9Capabilities.h"
#include "Helpers/D3D9SSAORenderer.h"
#include "Helpers/D3D9GUIRenderer.h"
#include "Helpers/D3D9Shader.h"

namespace selene
{

        /**
         * \addtogroup Windows
         * @{
         */

        /**
         * Represents D3D9 renderer.
         */
        class D3d9Renderer: public Renderer, public Status
        {
        public:
                // Renderer interface implementation
                bool initialize(const Renderer::Parameters& parameters);
                void destroy();
                void render(const Camera& camera);

                /**
                 * \brief Returns D3D9 device.
                 * \return D3D9 device
                 */
                static LPDIRECT3DDEVICE9 getDevice();

        private:
                friend class WindowsApplication;

                static LPDIRECT3DDEVICE9 d3dDevice_;
                LPDIRECT3D9 d3d_;

                D3d9VertexShader resultVertexShader_;
                D3d9PixelShader  resultPixelShader_;

                D3d9RenderTargetContainer renderTargetContainer_;
                D3d9ParticlesRenderer particlesRenderer_;
                D3d9LightingRenderer lightingRenderer_;
                D3d9ActorsRenderer actorsRenderer_;
                D3d9TextureHandler textureHandler_;
                D3d9FullScreenQuad fullScreenQuad_;
                D3d9BloomRenderer bloomRenderer_;
                D3d9SsaoRenderer ssaoRenderer_;
                D3d9GuiRenderer guiRenderer_;

                D3DPRESENT_PARAMETERS d3dPresentParameters_;
                D3d9FrameParameters frameParameters_;
                D3d9Capabilities capabilities_;
                Parameters parameters_;
                bool isDeviceLost_;

                D3d9Renderer();
                D3d9Renderer(const D3d9Renderer&);
                ~D3d9Renderer();
                D3d9Renderer& operator =(const D3d9Renderer&);

                /**
                 * \brief Initializes helpers.
                 * \return true if helpers have been successfully initialized
                 */
                bool initializeHelpers();

                /**
                 * \brief Destroys helpers.
                 */
                void destroyHelpers();

                /**
                 * \brief Writes log entry.
                 * \param[in] entry string, which shall be written to the log
                 */
                void writeLogEntry(const char* entry);

        };

        /**
         * @}
         */

}

#endif
