// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef TEXT_BOX_H
#define TEXT_BOX_H

#include "GUI.h"
#include <vector>

namespace selene
{

        /**
         * \addtogroup GUI
         * @{
         */

        /**
         * Represents text box. This GUI element can take input from keyboard.
         */
        class TextBox: public Gui::Element
        {
        public:
                /**
                 * \brief Constructs text box with given callback function, background colors, text colors,
                 * font size, position, size and text.
                 * \param[in] callbackFunction callback function
                 * \param[in] backgroundColors background colors for different states
                 * \param[in] textColors text colors for different states
                 * \param[in] fontSize font size
                 * \param[in] position position
                 * \param[in] size size
                 * \param[in] text text
                 */
                TextBox(std::function<void (int32_t, uint8_t)> callbackFunction,
                        const Vector4d* backgroundColors,
                        const Vector4d* textColors,
                        const Vector2d& fontSize,
                        const Vector2d& position,
                        const Vector2d& size,
                        const char* text);
                ~TextBox();

                /**
                 * \brief Sets text.
                 * \param[in] text text
                 */
                void setText(const char* text);

                /**
                 * \brief Returns text.
                 * \return text
                 */
                const std::string& getText() const;

        protected:
                std::vector<char> textVector_;

                /**
                 * \brief Processes text box.
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
