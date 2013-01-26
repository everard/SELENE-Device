// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef D3D9_FRAME_PARAMETERS_H
#define D3D9_FRAME_PARAMETERS_H

#include "../../../../../Engine/Core/Math/Matrix.h"
#include "../../../../../Engine/Core/Math/Vector.h"

namespace selene
{

        /**
         * \addtogroup Windows
         * @{
         */

        /**
         * Represents D3D9 frame parameters. This POD's purpose is to hold
         * per-frame constants. It is used to share data between different
         * rendering passes.
         */
        class D3d9FrameParameters
        {
        public:
                Matrix viewProjectionMatrix;
                Matrix projectionMatrix;
                Matrix viewInvMatrix;
                Matrix normalsMatrix;
                Matrix viewMatrix;

                Vector4d textureCoordinatesAdjustment;
                Vector4d edgeDetectionParameters;
                Vector4d projectionParameters;
                Vector4d shadowMapKernelSize;
                Vector4d unprojectionVector;
                Vector4d ssaoParameters;
                Vector4d screenSize;

                D3d9FrameParameters();
                ~D3d9FrameParameters();

        };

        /**
         * @}
         */

}

#endif
