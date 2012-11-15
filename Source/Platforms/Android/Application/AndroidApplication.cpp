// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "AndroidApplication.h"

namespace selene
{

        AndroidApplication::AndroidApplication(const char* name, uint32_t width, uint32_t height): Application(name, width, height) {}
        AndroidApplication::~AndroidApplication() {}

        //------------------------------------------------
        bool AndroidApplication::initialize()
        {
                if(!onInitialize())
                        return false;

                return true;
        }

        //------------------------------------------------
        bool AndroidApplication::run()
        {
                onDestroy();
                return true;
        }

        //------------------------------------------------
        void AndroidApplication::halt() {}

        //------------------------------------------------
        float AndroidApplication::getKeyState(uint8_t key)
        {
                key = 0;
                return 0.0f;
        }

}
