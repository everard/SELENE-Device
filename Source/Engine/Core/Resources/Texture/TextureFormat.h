// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef TEXTURE_FORMAT_H
#define TEXTURE_FORMAT_H

#include "Texture.h"

namespace selene
{

        /**
         * \addtogroup Resources
         * @{
         */

        #pragma pack(push, 1)

        /**
         * Represents DDS texture pixel format.
         */
        class DdsPixelFormat
        {
        public:
                uint32_t size;
                uint32_t flags;
                uint32_t fourCc;
                uint32_t rgbBitCount;
                uint32_t rBitMask;
                uint32_t gBitMask;
                uint32_t bBitMask;
                uint32_t aBitMask;

        };

        /**
         * Represents DDS texture caps.
         */
        class DdsCaps
        {
        public:
                uint32_t caps1;
                uint32_t caps2;
                uint32_t reserved[2];

        };

        /**
         * Represents DDS texture header.
         */
        class DdsHeader
        {
        public:
                uint32_t size;
                uint32_t flags;
                uint32_t height;
                uint32_t width;
                uint32_t pitchOrLinearSize;
                uint32_t depth;
                uint32_t mipMapCount;
                uint32_t reserved1[11];
                DdsPixelFormat pixelFormat;
                DdsCaps caps;
                uint32_t reserved2;

        };

        #pragma pack(pop)

        /// DDS texture constants
        enum
        {
                DDSD_CAPS        = 0x00000001,
                DDSD_HEIGHT      = 0x00000002,
                DDSD_WIDTH       = 0x00000004,
                DDSD_PITCH       = 0x00000008,
                DDSD_PIXELFORMAT = 0x00001000,
                DDSD_MIPMAPCOUNT = 0x00020000,
                DDSD_LINEARSIZE  = 0x00080000,
                DDSD_DEPTH       = 0x00800000,

                DDPF_ALPHA_PIXELS = 0x00000001,
                DDPF_FOURCC       = 0x00000004,
                DDPF_RGB          = 0x00000040,

                DDSCAPS_COMPLEX = 0x00000008,
                DDSCAPS_TEXTURE = 0x00001000,
                DDSCAPS_MIPMAP  = 0x00400000,

                DDSCAPS2_CUBEMAP           = 0x00000200,
                DDSCAPS2_CUBEMAP_POSITIVEX = 0x00000400,
                DDSCAPS2_CUBEMAP_NEGATIVEX = 0x00000800,
                DDSCAPS2_CUBEMAP_POSITIVEY = 0x00001000,
                DDSCAPS2_CUBEMAP_NEGATIVEY = 0x00002000,
                DDSCAPS2_CUBEMAP_POSITIVEZ = 0x00004000,
                DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x00008000,
                DDSCAPS2_VOLUME            = 0x00200000,

                FOURCC_DXT1 = 0x31545844,
                FOURCC_DXT3 = 0x33545844,
                FOURCC_DXT5 = 0x35545844
        };

        /**
         * @}
         */

}

#endif
