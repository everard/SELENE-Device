// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef D3D9_SHADER_H
#define D3D9_SHADER_H

#include "D3D9Device.h"
#include <string>

namespace selene
{

        // Forward declaration of classes
        class FileManager;

        // Represents D3D9 shader.
        class D3d9Shader
        {
        public:
                D3d9Shader(const char* fileName,
                           FileManager* fileManager,
                           const std::string& library,
                           const std::string& version,
                           DWORD flags);
                ~D3d9Shader();

                // Returns code
                LPD3DXBUFFER getCode() const;

        private:
                LPD3DXBUFFER code_;

        };

        // Represents D3D9 vertex shader.
        class D3d9VertexShader
        {
        public:
                D3d9VertexShader();
                ~D3d9VertexShader();

                // Creates shader
                bool create(const D3d9Shader& shader);

                // Destroys shader
                void destroy();

                // Sets shader
                void set();

        private:
                LPDIRECT3DVERTEXSHADER9 d3dShader_;
                LPDIRECT3DDEVICE9 d3dDevice_;

        };

        // Represents D3D9 pixel shader.
        class D3d9PixelShader
        {
        public:
                D3d9PixelShader();
                ~D3d9PixelShader();

                // Creates shader
                bool create(const D3d9Shader& shader);

                // Destroys shader
                void destroy();

                // Sets shader
                void set();

        private:
                LPDIRECT3DPIXELSHADER9 d3dShader_;
                LPDIRECT3DDEVICE9 d3dDevice_;

        };

}

#endif
