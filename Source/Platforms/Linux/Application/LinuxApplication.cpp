// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "LinuxApplication.h"

namespace selene
{

        LinuxApplication::LinuxApplication(const char* name, uint32_t width, uint32_t height): Application(name, width, height) {}
        LinuxApplication::~LinuxApplication()
        {
                destroy();
        }

        //------------------------------------------
        bool LinuxApplication::initialize()
        {
                if(!onInitialize())
                {
                        destroy();
                        return false;
                }

                return true;
        }

        //------------------------------------------
        bool LinuxApplication::run()
        {
                onDestroy();
                return true;
        }

        //------------------------------------------
        void LinuxApplication::halt() {}

        //------------------------------------------
        float LinuxApplication::getKeyState(uint8_t)
        {
                return 0.0f;
        }

        //------------------------------------------
        void LinuxApplication::destroy() {}

}
