// Copyright (c) 2013 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Effect.h"

#include <algorithm>
#include <utility>

namespace selene
{

        Effect::Parameter::Parameter(const char* name,
                                     float lowerLimit,
                                     float upperLimit,
                                     float initialValue):
                name_(name), lowerLimit_(lowerLimit),
                upperLimit_(upperLimit), value_(lowerLimit)
        {
                setValue(initialValue);
        }

        //------------------------------------------------------------------------------------
        bool Effect::Parameter::isValid() const
        {
                return (name_ != nullptr);
        }

        //------------------------------------------------------------------------------------
        const char* Effect::Parameter::getName() const
        {
                return name_;
        }

        //------------------------------------------------------------------------------------
        float Effect::Parameter::getLowerLimit() const
        {
                return lowerLimit_;
        }

        //------------------------------------------------------------------------------------
        float Effect::Parameter::getUpperLimit() const
        {
                return upperLimit_;
        }

        //------------------------------------------------------------------------------------
        float Effect::Parameter::setValue(float value)
        {
                value_ = std::max(std::min(value, upperLimit_), lowerLimit_);
                return value_;
        }

        //------------------------------------------------------------------------------------
        float Effect::Parameter::getValue() const
        {
                return value_;
        }

        Effect::Effect(const char* name, Quality maxQuality, ParametersList&& parametersList):
                invalidParameter_(nullptr), parametersList_(std::move(parametersList)),
                parametersMap_(), name_(name), quality_(0), maxQuality_(maxQuality)
        {
                for(auto& parameter: parametersList_)
                        parametersMap_.insert(std::make_pair(parameter.getName(),
                                                             std::ref(parameter)));
        }
        Effect::Effect(const Effect& other):
                Effect(other.name_, other.maxQuality_, ParametersList(other.parametersList_))
        {
                quality_ = other.quality_;
        }
        Effect::Effect(Effect&& other):
                invalidParameter_(nullptr), parametersList_(std::move(other.parametersList_)),
                parametersMap_(std::move(other.parametersMap_)), name_(other.name_),
                quality_(other.quality_), maxQuality_(other.maxQuality_)
        {
                other.name_ = nullptr;

                other.quality_    = 0;
                other.maxQuality_ = 0;
        }
        Effect::~Effect() {}
        Effect& Effect::operator =(Effect other)
        {
                std::swap(parametersList_, other.parametersList_);
                std::swap(parametersMap_,  other.parametersMap_);

                name_ = other.name_;

                quality_    = other.quality_;
                maxQuality_ = other.maxQuality_;

                return *this;
        }

        //------------------------------------------------------------------------------------
        bool Effect::isValid() const
        {
                return (name_ != nullptr);
        }

        //------------------------------------------------------------------------------------
        const char* Effect::getName() const
        {
                return name_;
        }

        //------------------------------------------------------------------------------------
        Effect::Quality Effect::setQuality(Effect::Quality quality)
        {
                quality_ = std::min(quality, maxQuality_);
                return quality_;
        }

        //------------------------------------------------------------------------------------
        Effect::Quality Effect::getQuality() const
        {
                return quality_;
        }

        //------------------------------------------------------------------------------------
        Effect::Quality Effect::getMaxQuality() const
        {
                return maxQuality_;
        }

        //------------------------------------------------------------------------------------
        Effect::Parameter& Effect::getParameter(const char* name)
        {
                auto parameter = parametersMap_.find(name);
                if(parameter == parametersMap_.end())
                        return invalidParameter_;

                return parameter->second.get();
        }

        //------------------------------------------------------------------------------------
        const Effect::Parameter& Effect::getParameter(const char* name) const
        {
                auto parameter = parametersMap_.find(name);
                if(parameter == parametersMap_.end())
                        return invalidParameter_;

                return parameter->second.get();
        }

        //------------------------------------------------------------------------------------
        const Effect::ParametersList& Effect::getParameters() const
        {
                return parametersList_;
        }

}
