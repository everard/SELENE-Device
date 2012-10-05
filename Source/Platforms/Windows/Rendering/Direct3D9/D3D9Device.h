// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef D3D9_DEVICE_H
#define D3D9_DEVICE_H

#include <d3dx9.h>
#include <d3d9.h>

namespace selene
{

        // Represents D3D9 device. This is singleton.
        class D3d9Device
        {
        public:
                // Returns device instance
                static D3d9Device* getInstance();

                // Returns device interface
                static LPDIRECT3DDEVICE9 getInterface();

                // Destroys device
                void destroy();

        private:
                friend class D3d9Renderer;

                static D3d9Device* device_;
                LPDIRECT3DDEVICE9 d3dDevice_;
                LPDIRECT3D9 d3d_;

                D3d9Device();
                ~D3d9Device();

        };

}

#endif
