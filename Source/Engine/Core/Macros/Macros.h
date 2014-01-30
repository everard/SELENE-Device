// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef MACROS_H
#define MACROS_H

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <cmath>

namespace selene
{

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

        using std::uint32_t;
        using std::uint16_t;
        using std::uint8_t;

        using std::int32_t;
        using std::int16_t;
        using std::int8_t;

        using std::size_t;

}

#endif
