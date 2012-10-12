// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Platform.h"
#include <io.h>

namespace selene
{

        bool Platform::fileExists(const char* fileName)
        {
                return (_access(fileName, 0) == 0);
        }

}
