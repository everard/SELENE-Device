// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef ENTITY_H
#define ENTITY_H

#include "../Macros/Macros.h"
#include <string>

namespace selene
{

        /**
         * \addtogroup Core
         * \brief File management and helpers.
         * @{
         */

        /**
         * Represents entity.
         * This class is base for many other classes, such as resources, scene objects, applications, etc.
         * It contains only one property - name - and provides interface for getting and setting it.
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
                std::string name_;

        };

        /**
         * @}
         */

}

#endif
