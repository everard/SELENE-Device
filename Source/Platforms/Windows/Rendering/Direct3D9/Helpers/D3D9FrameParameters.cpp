// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "D3D9FrameParameters.h"

namespace selene
{

        D3d9FrameParameters::D3d9FrameParameters():
                viewProjectionMatrix(),
                projectionMatrix(),
                viewInvMatrix(),
                normalsMatrix(),
                viewMatrix(),
                textureCoordinatesAdjustment(),
                projectionParameters(),
                shadowMapKernelSize(),
                unprojectionVector(),
                bloomParameters(),
                ssaoParameters(),
                screenSize(),
                ssaoQuality(0),
                bloomQuality(0),
                shadowsQuality(0) {}
        D3d9FrameParameters::~D3d9FrameParameters() {}

}
