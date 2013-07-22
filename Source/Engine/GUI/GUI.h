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

        /**
         * \addtogroup GUI
         * @{
         */

        /// GUI element flag
        enum GUI_ELEMENT_FLAG
        {
                GUI_ELEMENT_HIDDEN   = 0x01,
                GUI_ELEMENT_TOUCHED  = 0x02,
                GUI_ELEMENT_DISABLED = 0x04,
                GUI_ELEMENT_SELECTED = 0x08,
                GUI_ELEMENT_UPDATED  = 0x10
        };

        /// GUI element color type
        enum GUI_ELEMENT_COLOR_TYPE
        {
                GUI_ELEMENT_COLOR_DEFAULT = 0,
                GUI_ELEMENT_COLOR_TOUCHED,
                GUI_ELEMENT_COLOR_SELECTED,
                NUM_OF_GUI_ELEMENT_COLOR_TYPES
        };

        /// GUI message type
        enum GUI_MESSAGE_TYPE
        {
                GUI_MESSAGE_ON_CLICK = 0
        };

        /// GUI flag
        enum GUI_FLAG
        {
                GUI_DISABLED = 0x01,
                GUI_UPDATED  = 0x02,
                GUI_HIDDEN   = 0x04
        };

        /**
         * Represents GUI. This class can be used in actual application as-is. Gui contains
         * methods for adding, deleting and getting GUI elements. Each GUI element has unique
         * ID, which is basically an integer. GUI elements must not be deleted outside the Gui,
         * which holds them. After the element is added to the Gui with Gui::addElement method,
         * it will be automatically managed by the latter. This is the reason why GUI elements
         * must not be created on stack. They can only be created with operator new.
         *
         * The following example shows how to use Gui:
         * \code
         * // somewhere in program GUI object is declared
         * selene::Gui gui;
         *
         * ...
         * // here we add some buttons, labels and text boxes to the GUI
         * int32_t labelId0 = gui.addElement(new(std::nothrow) Label(...));
         * int32_t labelId1 = gui.addElement(new(std::nothrow) Label(...));
         * int32_t textBoxId0 = gui.addElement(new(std::nothrow) TextBox(...));
         * \endcode
         * \see Gui::Element
         * \see Label Button TextBox
         */
        class Gui: public Status
        {
        public:
                /**
                 * Represents GUI Element. This is base class for all GUI elements and should not be used
                 * as-is.
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
                        Element(const Element&) = delete;
                        virtual ~Element();
                        Element& operator =(const Element&) = delete;

                        /**
                         * \brief Returns ID.
                         * \return ID of the element
                         */
                        int32_t getId() const;

                        /**
                         * \brief Returns background color.
                         * \param[in] type type of the background color (must be one of the
                         * selene::GUI_ELEMENT_COLOR_TYPE)
                         * \return const reference to the background color of specified type
                         */
                        const Vector4d& getBackgroundColor(uint8_t type) const;

                        /**
                         * \brief Returns text color.
                         * \param[in] type type of the text color (must be one of the
                         * selene::GUI_ELEMENT_COLOR_TYPE)
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

                        std::function<void (int32_t, uint8_t)> callbackFunction_;

                        Vector4d backgroundColors_[NUM_OF_GUI_ELEMENT_COLOR_TYPES];
                        Vector4d textColors_[NUM_OF_GUI_ELEMENT_COLOR_TYPES];
                        Vector4d blackColor_;

                        Vector2d fontSize_, position_, size_;
                        mutable std::string text_;

                        int32_t id_;
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
                Gui(const Gui&) = delete;
                virtual ~Gui();
                Gui& operator =(const Gui&) = delete;

                /**
                 * \brief Destroys GUI.
                 */
                void destroy();

                /**
                 * \brief Adds element.
                 * \param[in] element GUI element
                 * \return ID of the element if it has been successfully added, -1 otherwise (in this case
                 * element has been destroyed, no manual memory deallocation is needed)
                 */
                int32_t addElement(Element* element);

                /**
                 * \brief Removes element.
                 * \param[in] elementId ID of the element
                 * \return true if element with given ID has been successfully deleted
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
                 * \return true if element with given ID has been successfully activated
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

                /**
                 * \brief Returns position of the cursor.
                 * \return position of the cursor
                 */
                const Vector2d& getCursorPosition() const;

        protected:
                std::map<int32_t, std::shared_ptr<Element>> elements_;
                std::weak_ptr<Element> activeElement_;
                Vector2d cursorPosition_;
                int32_t nextElementId_;

                /**
                 * \brief Sets active element.
                 * \param[in] element element which must be activated
                 */
                void setActiveElement(const std::shared_ptr<Element>& element);

        };

        /**
         * @}
         */

}

#endif
