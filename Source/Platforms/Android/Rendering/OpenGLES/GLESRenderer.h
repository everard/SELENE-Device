// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GLES_RENDERER_H
#define GLES_RENDERER_H

#include "../../../../Engine/Rendering/Renderer.h"
#include "../../../../Engine/Core/Resources/Mesh/Mesh.h"
#include "../../../../Engine/Core/Status/Status.h"

#include "Helpers/GLESRenderTargetContainer.h"
#include "Helpers/GLESLightingRenderer.h"
#include "Helpers/GLESFrameParameters.h"
#include "Helpers/GLESActorsRenderer.h"
#include "Helpers/GLESFullScreenQuad.h"
#include "Helpers/GLESTextureHandler.h"
#include "Helpers/GLESBloomRenderer.h"
#include "Helpers/GLESCapabilities.h"
#include "Helpers/GLESGUIRenderer.h"

namespace selene
{

        /**
         * \addtogroup Android
         * @{
         */

        /**
         * Represents OpenGL ES renderer.
         */
        class GlesRenderer: public Renderer
        {
        public:
                // Renderer interface implementation
                bool initialize(const Renderer::Parameters& parameters);
                void destroy();
                void render(const Camera& camera);

        private:
                friend class AndroidApplication;

                GlesGlslProgram resultRenderingProgram_;
                GLint textureCoordinatesAdjustmentLocation_;
                GLint resultTextureLocation_;

                GlesRenderTargetContainer renderTargetContainer_;
                GlesLightingRenderer lightingRenderer_;
                GlesActorsRenderer actorsRenderer_;
                GlesFullScreenQuad fullScreenQuad_;
                GlesTextureHandler textureHandler_;
                GlesBloomRenderer bloomRenderer_;
                GlesGuiRenderer guiRenderer_;

                GlesFrameParameters frameParameters_;
                GlesCapabilities capabilities_;
                Parameters parameters_;

                GlesRenderer();
                GlesRenderer(const GlesRenderer&) = delete;
                ~GlesRenderer();
                GlesRenderer& operator =(const GlesRenderer&) = delete;

                /**
                 * \brief Writes log entry.
                 * \param[in] entry string, which should be written to the log
                 */
                void writeLogEntry(const char* entry);

                /**
                 * \brief Initializes context and retains all resources.
                 * \return true if context has been successfully initialized and
                 * all resources have been retained
                 */
                bool retain();

                /**
                 * \brief Discards context and all resources.
                 */
                void discard();

                /**
                 * \brief Initializes helpers.
                 * \return true if helpers have been successfully initialized
                 */
                bool initializeHelpers();

                /**
                 * \brief Destroys helpers.
                 */
                void destroyHelpers();

        };

        /**
         * @}
         */

}

#endif
