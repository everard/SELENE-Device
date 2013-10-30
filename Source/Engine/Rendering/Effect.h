// Copyright (c) 2013 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef EFFECT_H
#define EFFECT_H

#include "../Core/Helpers/Utility.h"

#include <functional>
#include <limits>
#include <vector>
#include <map>

namespace selene
{

        /**
         * \addtogroup Rendering
         * @{
         */

        /**
         * Represents effect.
         */
        class Effect
        {
        public:
                /**
                 * Represents parameter of the effect.
                 */
                class Parameter
                {
                public:
                        /**
                         * \brief Constructs parameter.
                         * \param[in] name name of the parameter
                         * \param[in] lowerLimit lower limit of the parameter
                         * \param[in] upperLimit upper limit of the parameter
                         * \param[in] initialValue initial value of the parameter
                         */
                        Parameter(const char* name,
                                  float lowerLimit = std::numeric_limits<float>::lowest(),
                                  float upperLimit = std::numeric_limits<float>::max(),
                                  float initialValue = 0.0f);
                        Parameter(const Parameter&) = default;
                        ~Parameter() = default;
                        Parameter& operator =(const Parameter&) = default;

                        /**
                         * \brief Returns true if parameter is valid.
                         * \return true if parameter is valid
                         */
                        bool isValid() const;

                        /**
                         * \brief Returns name of the parameter.
                         * \return name of the parameter
                         */
                        const char* getName() const;

                        /**
                         * \brief Returns lower limit of the parameter.
                         * \return lower limit of the parameter
                         */
                        float getLowerLimit() const;

                        /**
                         * \brief Returns upper limit of the parameter.
                         * \return upper limit of the parameter
                         */
                        float getUpperLimit() const;

                        /**
                         * \brief Sets value of the parameter.
                         * \param[in] value value of the parameter
                         * \return actual value, which has been set
                         */
                        float setValue(float value);

                        /**
                         * \brief Returns value of the parameter.
                         * \return value of the parameter
                         */
                        float getValue() const;

                private:
                        const char* name_;
                        float lowerLimit_, upperLimit_, value_;

                };

                /**
                 * Represents list of the parameters.
                 */
                typedef std::vector<Parameter> ParametersList;

                /**
                 * Represents quality of the effect.
                 */
                typedef uint8_t Quality;

                /**
                 * \brief Constructs effect.
                 * \param[in] name name of the effect
                 * \param[in] maxQuality maximum quality of the effect
                 * \param[in] parametersList list of the parameters
                 */
                Effect(const char* name, Quality maxQuality, ParametersList&& parametersList);
                Effect(const Effect& other);
                Effect(Effect&& other);
                ~Effect();
                Effect& operator =(Effect other);

                /**
                 * \brief Returns true if effect is valid.
                 * \return true if effect is valid
                 */
                bool isValid() const;

                /**
                 * \brief Returns name of the effect.
                 * \return name of the effect
                 */
                const char* getName() const;

                /**
                 * \brief Sets quality of the effect.
                 * \param[in] quality quality of the effect (integer in range [0; 255])
                 * \return actual quality, which has been set
                 */
                Quality setQuality(Quality quality);

                /**
                 * \brief Returns quality of the effect.
                 * \return quality of the effect
                 */
                Quality getQuality() const;

                /**
                 * \brief Returns maximum quality of the effect.
                 * \return maximum quality of the effect
                 */
                Quality getMaxQuality() const;

                /**
                 * \brief Returns parameter.
                 * \param[in] name name of the parameter
                 * \return reference to the parameter
                 */
                Parameter& getParameter(const char* name);

                /**
                 * \brief Returns parameter.
                 * \param[in] name name of the parameter
                 * \return const reference to the parameter
                 */
                const Parameter& getParameter(const char* name) const;

                /**
                 * \brief Returns list of the parameters.
                 * \return list of the parameters
                 */
                const ParametersList& getParameters() const;

        private:
                /**
                 * Represents map of the parameters.
                 */
                typedef std::map<const char*,
                                 std::reference_wrapper<Parameter>,
                                 Utility::StringComparator> ParametersMap;

                Parameter invalidParameter_;

                ParametersList parametersList_;
                ParametersMap  parametersMap_;

                const char* name_;
                Quality quality_, maxQuality_;

        };

        /**
         * @}
         */

}

#endif
