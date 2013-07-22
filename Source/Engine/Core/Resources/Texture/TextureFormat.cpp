// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "TextureFormat.h"
#include <cstring>

namespace selene
{

        DdsPixelFormat::DdsPixelFormat():
                size(0), flags(0), fourCc(0), rgbBitCount(0),
                rBitMask(0), gBitMask(0), bBitMask(0), aBitMask(0) {}
        DdsPixelFormat::~DdsPixelFormat() {}

        DdsCaps::DdsCaps(): caps1(0), caps2(0)
        {
                reserved[0] = reserved[1] = 0;
        }
        DdsCaps::~DdsCaps() {}

        DdsHeader::DdsHeader():
                size(0), flags(0), height(0), width(0), pitchOrLinearSize(0),
                depth(0), mipMapCount(0), pixelFormat(), caps(), reserved2(0)
        {
                static_assert(sizeof(DdsHeader) == 124, "DdsHeader must be 124 bytes long");

                std::memset(reserved1, 0, sizeof(reserved1));
        }
        DdsHeader::~DdsHeader() {}

}
