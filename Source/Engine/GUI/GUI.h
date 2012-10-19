// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GUI_H
#define GUI_H

#include "../Application/Application.h"
#include "../Core/Status/Status.h"
#include "../Core/Math/Sphere.h"

#include <functional>
#include <memory>
#include <string>
#include <map>

namespace selene
{

        /// GUI element flags, GUI element color types, GUI message types and GUI flags
        enum
        {
                // GUI element flags
                GUI_ELEMENT_HIDDEN   = 0x01,
                GUI_ELEMENT_TOUCHED  = 0x02,
                GUI_ELEMENT_DISABLED = 0x04,
                GUI_ELEMENT_SELECTED = 0x08,
                GUI_ELEMENT_UPDATED  = 0x10,

                // GUI element color types
                GUI_ELEMENT_COLOR_DEFAULT = 0,
                GUI_ELEMENT_COLOR_TOUCHED,
                GUI_ELEMENT_COLOR_SELECTED,
                NUM_OF_GUI_ELEMENT_COLOR_TYPES,

                // GUI message types
                GUI_MESSAGE_ON_CLICK = 0,

                // GUI flags
                GUI_UPDATED = 0x01,
                GUI_HIDDEN  = 0x02
        };

        /**
         * Represents GUI.
         */
        class Gui: public Status
        {
        public:
                /**
                 * Represents GUI Element.
                 */
                class Element: public Status
                {
                public:
                        /**
                         * \brief Constructs GUI element with given callback function, background colors, text colors,
                         * font size, position, size and text.
                         * \param[in] callbackFunction callback function
                         * \param[in] backgroundColors background colors for different states
                         * \param[in] textColors text colors for different states
                         * \param[in] fontSize font size
                         * \param[in] position position
                         * \param[in] size size
                         * \param[in] text text
                         */
                        Element(std::function<void (int32_t, uint8_t)> callbackFunction,
                                const Vector4d* backgroundColors,
                                const Vector4d* textColors,
                                const Vector2d& fontSize,
                                const Vector2d& position,
                                const Vector2d& size,
                                const char* text);
                        virtual ~Element();

                        /**
                         * \brief Returns ID.
                         * \return ID of the element
                         */
                        int32_t getId() const;

                        /**
                         * \brief Returns background color.
                         * \param[in] type type of the background color (must be one of the
                         * GUI element color types)
                         * \return const reference to the background color of specified type
                         */
                        const Vector4d& getBackgroundColor(uint8_t type) const;

                        /**
                         * \brief Returns text color.
                         * \param[in] type type of the text color (must be one of the
                         * GUI element color types)
                         * \return const reference to the text color of specified type
                         */
                        const Vector4d& getTextColor(uint8_t type) const;

                        /**
                         * \brief Returns font size.
                         * \return font size
                         */
                        const Vector2d& getFontSize() const;

                        /**
                         * \brief Returns position.
                         * \return position
                         */
                        const Vector2d& getPosition() const;

                        /**
                         * \brief Returns size.
                         * \return size
                         */
                        const Vector2d& getSize() const;

                        /**
                         * \brief Sets text.
                         * \param[in] text text
                         */
                        virtual void setText(const char* text);

                        /**
                         * \brief Returns text.
                         * \return text
                         */
                        virtual const std::string& getText() const;

                        /**
                         * \brief Determines relation between GUI element and cursor position.
                         * \param[in] cursorPosition position of the cursor
                         * \return INSIDE if cursor is inside the GUI element, OUTSIDE otherwise
                         */
                        RELATION determineRelation(const Vector2d& cursorPosition) const;

                protected:
                        friend class Gui;

                        // Callback function
                        std::function<void (int32_t, uint8_t)> callbackFunction_;

                        // Background colors
                        Vector4d backgroundColors_[NUM_OF_GUI_ELEMENT_COLOR_TYPES];

                        // Text colors
                        Vector4d textColors_[NUM_OF_GUI_ELEMENT_COLOR_TYPES];

                        // Black color
                        Vector4d blackColor_;

                        // Font size, position and size
                        Vector2d fontSize_, position_, size_;

                        // Text
                        mutable std::string text_;

                        // ID
                        int32_t id_;

                        // GUI
                        Gui* gui_;

                        /**
                         * \brief Status change event callback.
                         */
                        void onChange();

                        /**
                         * \brief Sets GUI.
                         * \param[in] gui GUI
                         */
                        void setGui(Gui* gui);

                        /**
                         * \brief Sets ID.
                         * \param[in] id ID
                         */
                        void setId(int32_t id);

                        /**
                         * \brief Processes element.
                         * \param[in] cursorPosition position of the cursor
                         * \param[in] pressedControlButtons pressed control buttons (mask)
                         * \param[in] key keyboard key
                         */
                        virtual void process(const Vector2d& cursorPosition,
                                             uint8_t pressedControlButtons,
                                             uint8_t key);

                };

                Gui();
                virtual ~Gui();

                /**
                 * \brief Destroys GUI.
                 */
                void destroy();

                /**
                 * \brief Adds element.
                 * \param[in] element GUI element
                 * \return ID of the element if it was successfully added, -1 otherwise (in this case
                 * element is destroyed, no manual memory deallocation is needed)
                 */
                int32_t addElement(Element* element);

                /**
                 * \brief Removes element.
                 * \param[in] elementId ID of the element
                 * \return true if element with given ID was successfully deleted
                 */
                bool removeElement(int32_t elementId);

                /**
                 * \brief Returns element.
                 * \param[in] elementId ID of the element
                 * \return std::weak_ptr to the element with given ID
                 */
                std::weak_ptr<Element> getElement(int32_t elementId);

                /**
                 * \brief Returns elements.
                 * \return const reference to the std::map which contains GUI elements
                 */
                const std::map<int32_t, std::shared_ptr<Element>>& getElements() const;

                /**
                 * \brief Sets active element.
                 * \param[in] elementId ID of the element
                 * \return true if element with given ID was successfully activated
                 */
                bool setActiveElement(int32_t elementId);

                /**
                 * \brief Hides all elements.
                 */
                void hideAllElements();

                /**
                 * \brief Processes GUI.
                 *
                 * Each GUI element is processed separately (for each one of them their own process()
                 * function is called).
                 * \param[in] cursorPosition position of the cursor
                 * \param[in] pressedControlButtons pressed control buttons (mask)
                 * \param[in] key keyboard key
                 */
                void process(const Vector2d& cursorPosition,
                             uint8_t pressedControlButtons,
                             uint8_t key = 0);

        protected:
                // Elements
                std::map<int32_t, std::shared_ptr<Element>> elements_;

                // Active element
                std::weak_ptr<Element> activeElement_;

                // Element ID
                int32_t elementId_;

                /**
                 * \brief Sets active element.
                 * \param[in] element element which must be activated
                 */
                void setActiveElement(const std::shared_ptr<Element>& element);

        };

}

#endif
