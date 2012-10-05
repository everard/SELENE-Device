// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "../../../../Engine/Core/Macros/Macros.h"
#include "D3D9Device.h"
#include <iostream>
#include <new>

namespace selene
{

        D3d9Device* D3d9Device::device_ = nullptr;

        //------------------------------------------
        D3d9Device* D3d9Device::getInstance()
        {
                if(device_ == nullptr)
                        device_ = new(std::nothrow) D3d9Device;
                return device_;
        }

        //------------------------------------------
        LPDIRECT3DDEVICE9 D3d9Device::getInterface()
        {
                D3d9Device* device = getInstance();
                if(device == nullptr)
                        return nullptr;

                return device->d3dDevice_;
        }

        //------------------------------------------
        void D3d9Device::destroy()
        {
                SAFE_DELETE(device_);
        }

        D3d9Device::D3d9Device()
        {
                std::cout << "Creating D3D9 device\n";
                d3dDevice_ = nullptr;
                d3d_ = nullptr;
        }
        D3d9Device::~D3d9Device()
        {
                std::cout << "Destroying D3D9 device\n";
                SAFE_RELEASE(d3dDevice_);
                SAFE_RELEASE(d3d_);
                device_ = nullptr;
        }

}
