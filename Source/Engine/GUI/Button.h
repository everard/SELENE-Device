// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef BUTTON_H
#define BUTTON_H

#include "GUI.h"

namespace selene
{

        /**
         * \addtogroup GUI
         * \brief Graphical user interface subsystem.
         * @{
         */

        /**
         * Represents button. Button will send message selene::GUI_MESSAGE_ON_CLICK when clicked.
         */
        class Button: public Gui::Element
        {
        public:
                /**
                 * \brief Constructs button with given callback function, background colors, text colors,
                 * font size, position, size and text.
                 * \param[in] callbackFunction callback function
                 * \param[in] backgroundColors background colors for different states
                 * \param[in] textColors text colors for different states
                 * \param[in] fontSize font size
                 * \param[in] position position
                 * \param[in] size size
                 * \param[in] text text
                 */
                Button(std::function<void (int32_t, uint8_t)> callbackFunction,
                       const Vector4d* backgroundColors,
                       const Vector4d* textColors,
                       const Vector2d& fontSize,
                       const Vector2d& position,
                       const Vector2d& size,
                       const char* text);
                Button(const Button&) = delete;
                ~Button();
                Button& operator =(const Button&) = delete;

        protected:
                /**
                 * \brief Processes button.
                 * \param[in] cursorPosition position of the cursor
                 * \param[in] pressedControlButtons pressed control buttons (mask)
                 * \param[in] key keyboard key
                 */
                void process(const Vector2d& cursorPosition,
                             uint8_t pressedControlButtons,
                             uint8_t key);

        };

        /**
         * @}
         */

}

#endif
