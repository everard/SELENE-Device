// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef PLATFORM_H
#define PLATFORM_H

#include "Rendering/Direct3D9/Resources/D3D9Texture.h"
#include "Rendering/Direct3D9/Resources/D3D9Mesh.h"

#include "Rendering/Direct3D9/D3D9Renderer.h"
#include "Application/WindowsApplication.h"
#include "Application/WindowsTimer.h"

namespace selene
{

        // Represents platform.
        class Platform
        {
        public:
                // Checks file existence
                static bool fileExists(const char* fileName);

        };

}

#endif
