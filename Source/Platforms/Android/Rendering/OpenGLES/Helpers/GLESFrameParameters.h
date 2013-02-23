// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GLES_FRAME_PARAMETERS_H
#define GLES_FRAME_PARAMETERS_H

#include "../../../../../Engine/Core/Math/Matrix.h"
#include "../../../../../Engine/Core/Math/Vector.h"

namespace selene
{

        /**
         * \addtogroup Android
         * @{
         */

        /**
         * Represents OpenGL ES frame parameters. This POD's purpose is to hold
         * per-frame constants. It is used to share data between different
         * rendering passes.
         */
        class GlesFrameParameters
        {
        public:
                Matrix viewProjectionMatrix;
                Matrix projectionMatrix;
                Matrix viewInvMatrix;
                Matrix normalsMatrix;
                Matrix viewMatrix;

                Vector4d textureCoordinatesAdjustment;
                Vector4d edgeDetectionParameters;
                Vector4d conversionParameters;
                Vector4d projectionParameters;
                Vector4d shadowMapKernelSize;
                Vector4d unprojectionVector;
                Vector4d bloomParameters;
                Vector4d ssaoParameters;
                Vector4d screenSize;

                GlesFrameParameters();
                ~GlesFrameParameters();

        };

        /**
         * @}
         */

}

#endif