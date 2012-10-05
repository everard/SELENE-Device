// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef ENTITY_H
#define ENTITY_H

#include "../Macros/Macros.h"
#include <string>

namespace selene
{

        /**
         * Represents entity.
         * This class is base for many other classes, such as resources, scene objects, GUI elements, etc.
         */
        class Entity
        {
        public:
                /**
                 * \brief Constructs entity with given name.
                 * \param[in] name entity name
                 */
                Entity(const char* name);
                virtual ~Entity();

                /**
                 * \brief Returns entity name.
                 * \return c-string containing entity name
                 */
                const char* getName() const;

        protected:
                // Name
                std::string name_;

        };

}

#endif
