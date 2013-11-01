// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLESFrameParameters.h"

namespace selene
{

        GlesFrameParameters::GlesFrameParameters():
                viewProjectionMatrix(),
                projectionMatrix(),
                viewInvMatrix(),
                normalsMatrix(),
                viewMatrix(),
                textureCoordinatesAdjustment(),
                conversionParameters(),
                projectionParameters(),
                unprojectionVector(),
                renderTargetSize(),
                bloomParameters(),
                shadowMapSize(),
                screenSize(),
                bloomQuality(0),
                shadowsQuality(0) {}
        GlesFrameParameters::~GlesFrameParameters() {}

}
