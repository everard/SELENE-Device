// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef MACROS_H
#define MACROS_H

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace selene
{

        // Safely deallocates memory
        #define SAFE_DELETE_ARRAY(x) {if((x)) delete[](x); (x) = nullptr;}

        // Safely deallocates memory
        #define SAFE_DELETE(x) {if((x)) delete(x); (x) = nullptr;}

        // Safely releases COM object
        #define SAFE_RELEASE(x) {if((x)) (x)->Release(); (x) = nullptr;}

        // Safely closes file handle
        #define SAFE_CLOSE(x) {if((x) > 0) close((x)); (x) = -1;}

        // Checks bits
        #define IS_SET(x, y) (((x) & (y)) != 0)

        // Clears bits
        #define CLEAR(x, y) {x &= ~(y);}

        // Sets bits
        #define SET(x, y) {x |= (y);}

        // PI constant
        #define SELENE_PI 3.14159265358979323846f

        // Epsilon
        #define SELENE_EPSILON 0.001f

}

#endif
