// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Status.h"

namespace selene
{

        Status::Status(uint8_t flags)
        {
                flags_ = flags;
        }
        Status::~Status() {}

        //-----------------------------------------------------
        void Status::setFlags(uint8_t flags)
        {
                SET(flags_, flags);
                onChange();
        }

        //-----------------------------------------------------
        void Status::clearFlags(uint8_t flags)
        {
                CLEAR(flags_, flags);
                onChange();
        }

        //-----------------------------------------------------
        uint8_t Status::getFlags() const
        {
                return flags_;
        }

        //-----------------------------------------------------
        bool Status::is(uint8_t flags) const
        {
                return IS_SET(flags_, flags);
        }

        //-----------------------------------------------------
        void Status::onChange() {}

        //-----------------------------------------------------
        void Status::setFlags(uint8_t flags) const
        {
                SET(flags_, flags);
        }

        //-----------------------------------------------------
        void Status::clearFlags(uint8_t flags) const
        {
                CLEAR(flags_, flags);
        }

}
