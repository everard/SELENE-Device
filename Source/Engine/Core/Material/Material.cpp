// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Material.h"

namespace selene
{

        Material::Material():
                specularLevel_(0.0f), glossiness_(0.0f), opacity_(1.0f),
                shadingType_(MATERIAL_SHADING_UNKNOWN) {}
        Material::~Material() {}

        //---------------------------------------------------------------------------------------
        void Material::setColor(const Vector3d& color, uint8_t type)
        {
                if(type >= NUM_OF_MATERIAL_COLOR_TYPES)
                        return;

                colors_[type] = color;
        }

        //---------------------------------------------------------------------------------------
        const Vector3d& Material::getColor(uint8_t type) const
        {
                if(type >= NUM_OF_MATERIAL_COLOR_TYPES)
                        return colors_[MATERIAL_COLOR_AMBIENT];

                return colors_[type];
        }

        //---------------------------------------------------------------------------------------
        void Material::setSpecularLevel(float specularLevel)
        {
                specularLevel_ = specularLevel;
        }

        //---------------------------------------------------------------------------------------
        float Material::getSpecularLevel() const
        {
                return specularLevel_;
        }

        //---------------------------------------------------------------------------------------
        void Material::setGlossiness(float glossiness)
        {
                glossiness_ = glossiness;
        }

        //---------------------------------------------------------------------------------------
        float Material::getGlossiness() const
        {
                return glossiness_;
        }

        //---------------------------------------------------------------------------------------
        void Material::setOpacity(float opacity)
        {
                opacity_ = opacity;
        }

        //---------------------------------------------------------------------------------------
        float Material::getOpacity() const
        {
                return opacity_;
        }

        //---------------------------------------------------------------------------------------
        void Material::setTextureMap(const Resource::Instance<Texture>& textureMap, uint8_t type)
        {
                if(type >= NUM_OF_TEXTURE_MAP_TYPES)
                        return;

                textureMaps_[type] = textureMap;
        }

        //---------------------------------------------------------------------------------------
        const Resource::Instance<Texture>& Material::getTextureMap(uint8_t type) const
        {
                if(type >= NUM_OF_TEXTURE_MAP_TYPES)
                        return textureMaps_[TEXTURE_MAP_AMBIENT];

                return textureMaps_[type];
        }

        //---------------------------------------------------------------------------------------
        void Material::setShadingType(uint8_t type)
        {
                shadingType_ = type;
        }

        //---------------------------------------------------------------------------------------
        uint8_t Material::getShadingType() const
        {
                return shadingType_;
        }

}
