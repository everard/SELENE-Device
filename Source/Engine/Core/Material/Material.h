// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef MATERIAL_H
#define MATERIAL_H

#include "../Resources/Texture/Texture.h"
#include "../Status/Status.h"
#include "../Math/Vector.h"

namespace selene
{

        /**
         * \addtogroup Core
         * @{
         */

        /// Material shading type
        enum MATERIAL_SHADING_TYPE
        {
                MATERIAL_SHADING_BLINN = 0,
                MATERIAL_SHADING_PHONG,
                MATERIAL_SHADING_METAL,
                MATERIAL_SHADING_ANISOTROPIC,
                MATERIAL_SHADING_UNKNOWN,
                NUM_OF_MATERIAL_SHADING_TYPES = MATERIAL_SHADING_UNKNOWN
        };

        /// Material flag
        enum MATERIAL_FLAG
        {
                MATERIAL_TWO_SIDED = 0x80
        };

        /// Material color type
        enum MATERIAL_COLOR_TYPE
        {
                MATERIAL_COLOR_AMBIENT = 0,
                MATERIAL_COLOR_DIFFUSE,
                MATERIAL_COLOR_SPECULAR,
                NUM_OF_MATERIAL_COLOR_TYPES
        };

        /// Texture map type
        enum TEXTURE_MAP_TYPE
        {
                TEXTURE_MAP_AMBIENT = 0,
                TEXTURE_MAP_DIFFUSE,
                TEXTURE_MAP_NORMAL,
                TEXTURE_MAP_SPECULAR,
                NUM_OF_TEXTURE_MAP_TYPES
        };

        /**
         * Represents material.
         */
        class Material: public Status
        {
        public:
                Material();
                virtual ~Material();

                /**
                 * \brief Sets color.
                 *
                 * There are different color types such as ambient, diffuse, etc.
                 * For example, to set ambient color, this function should be called
                 * as follows:
                 *
                 *     material.setColor(ambientColor, MATERIAL_COLOR_AMBIENT);
                 *
                 * \param[in] color material color
                 * \param[in] type color type (must be one of the selene::MATERIAL_COLOR_TYPE)
                 */
                void setColor(const Vector3d& color, uint8_t type);

                /**
                 * \brief Returns color.
                 * \see setColor
                 * \param[in] type color type (must be one of the selene::MATERIAL_COLOR_TYPE)
                 * \return const reference to the color of specified type (if type is out of range,
                 * ambient color is returned)
                 */
                const Vector3d& getColor(uint8_t type) const;

                /**
                 * \brief Sets specular level.
                 * \param[in] specularLevel specular level
                 */
                void setSpecularLevel(float specularLevel);

                /**
                 * \brief Returns specular level.
                 * \return specular level
                 */
                float getSpecularLevel() const;

                /**
                 * \brief Sets glossiness.
                 * \param[in] glossiness glossiness
                 */
                void setGlossiness(float glossiness);

                /**
                 * \brief Returns glossiness.
                 * \return glossiness
                 */
                float getGlossiness() const;

                /**
                 * \brief Sets opacity.
                 * \param[in] opacity opacity
                 */
                void setOpacity(float opacity);

                /**
                 * \brief Returns opacity.
                 * \return opacity
                 */
                float getOpacity() const;

                /**
                 * \brief Sets texture map.
                 *
                 * There are different texture map types such as ambient, diffuse, etc.
                 * For example, to set ambient texture map, this function should be called
                 * as follows:
                 *
                 *     material.setTextureMap(ambientMap, TEXTURE_MAP_AMBIENT);
                 *
                 * \param[in] textureMap texture map type (must be one of the selene::TEXTURE_MAP_TYPE)
                 * \param[in] type texture map type (if type is out of range, texture map is not set)
                 */
                void setTextureMap(const Resource::Instance<Texture>& textureMap, uint8_t type);

                /**
                 * \brief Returns texture map.
                 * \see setTextureMap
                 * \param[in] type texture map type (must be one of the selene::TEXTURE_MAP_TYPE)
                 * \return const reference to the texture map of specified type (if type is out of range,
                 * ambient texture map is returned)
                 */
                const Resource::Instance<Texture>& getTextureMap(uint8_t type) const;

                /**
                 * \brief Sets shading type.
                 * \param[in] type shading type (must be one of the selene::MATERIAL_SHADING_TYPE)
                 */
                void setShadingType(uint8_t type);

                /**
                 * \brief Returns shading type.
                 * \return shading type (one of the selene::MATERIAL_SHADING_TYPE)
                 */
                uint8_t getShadingType() const;

        private:
                Vector3d colors_[NUM_OF_MATERIAL_COLOR_TYPES];
                float specularLevel_, glossiness_, opacity_;
                Resource::Instance<Texture> textureMaps_[NUM_OF_TEXTURE_MAP_TYPES];
                uint8_t shadingType_;

        };

        /**
         * @}
         */

}

#endif
