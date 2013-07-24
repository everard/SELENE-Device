// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef D3D9_SHADER_H
#define D3D9_SHADER_H

#include "../../../../../Engine/Core/Macros/Macros.h"
#include <d3dx9.h>
#include <d3d9.h>
#include <string>

namespace selene
{

        /**
         * \addtogroup Windows
         * @{
         */

        // Forward declaration of classes
        class D3d9Capabilities;

        /**
         * Represents D3D9 shader. This class compiles shader from resource.
         */
        class D3d9Shader
        {
        public:
                /// Library types
                enum
                {
                        LIBRARY_VERTEX_SHADER = 0,
                        LIBRARY_PIXEL_SHADER,
                        LIBRARY_EMPTY
                };

                /**
                 * \brief Constructs shader.
                 *
                 * Loads shader source code from resource with given name, then
                 * compiles it with given compilation flags, library type and version.
                 * \param[in] name name of the resource, which holds source code of the shader
                 * \param[in] version version of the shader
                 * \param[in] flags compilation flags
                 * \param[in] libraryType type of the shader library
                 * \param[in] capabilities D3D capabilities
                 */
                D3d9Shader(const char* name, const char* version, DWORD flags, uint8_t libraryType,
                           const D3d9Capabilities& capabilities);
                D3d9Shader(const D3d9Shader&) = default;
                ~D3d9Shader();
                D3d9Shader& operator =(const D3d9Shader&) = default;

                /**
                 * \brief Returns binary code of the shader.
                 * \return pointer to the buffer, which contains binary code of the shader,
                 * nullptr if shader has not been successfully compiled during construction
                 */
                LPD3DXBUFFER getCode() const;

        private:
                LPD3DXBUFFER code_;

        };

        /**
         * Represents D3D9 vertex shader.
         */
        class D3d9VertexShader
        {
        public:
                D3d9VertexShader();
                D3d9VertexShader(const D3d9VertexShader&) = delete;
                ~D3d9VertexShader();
                D3d9VertexShader& operator =(const D3d9VertexShader&) = delete;

                /**
                 * \brief Creates vertex shader.
                 * \param[in] shader D3D9 shader, which holds binary code
                 * \return true if vertex shader has been successfully created
                 */
                bool create(const D3d9Shader& shader);

                /**
                 * \brief Destroys vertex shader.
                 */
                void destroy();

                /**
                 * \brief Sets vertex shader.
                 */
                void set();

        private:
                LPDIRECT3DVERTEXSHADER9 d3dShader_;
                LPDIRECT3DDEVICE9 d3dDevice_;

        };

        /**
         * Represents D3D9 pixel shader.
         */
        class D3d9PixelShader
        {
        public:
                D3d9PixelShader();
                D3d9PixelShader(const D3d9PixelShader&) = delete;
                ~D3d9PixelShader();
                D3d9PixelShader& operator =(const D3d9PixelShader&) = delete;

                /**
                 * \brief Creates pixel shader.
                 * \param[in] shader D3D9 shader, which holds binary code
                 * \return true if pixel shader has been successfully created
                 */
                bool create(const D3d9Shader& shader);

                /**
                 * \brief Destroys pixel shader.
                 */
                void destroy();

                /**
                 * \brief Sets pixel shader.
                 */
                void set();

        private:
                LPDIRECT3DPIXELSHADER9 d3dShader_;
                LPDIRECT3DDEVICE9 d3dDevice_;

        };

        /**
         * @}
         */

}

#endif
