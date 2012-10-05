// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "../../../../Engine/Core/FileManager/FileManager.h"
#include "../../../../Engine/Core/Helpers/Array.h"
#include "D3D9Shader.h"

namespace selene
{

        D3d9Shader::D3d9Shader(const char* fileName,
                               FileManager* fileManager,
                               const std::string& library,
                               const std::string& version,
                               DWORD flags)
        {
                code_ = nullptr;

                if(fileManager == nullptr)
                        return;

                std::ifstream stream;
                if(!fileManager->open(fileName, stream))
                        return;

                stream.seekg(0, std::ios::end);
                Array<uint8_t, uint32_t> fileContent;
                uint32_t length = (uint32_t)stream.tellg();
                if(!fileContent.create(length + 1))
                        return;

                stream.seekg(0, std::ios::beg);
                stream.read((char*)&fileContent[0], length);
                fileContent[length] = 0;

                std::string sourceCode = (char*)&fileContent[0];
                sourceCode = library + sourceCode;

                if(FAILED(D3DXCompileShader(sourceCode.c_str(), sourceCode.length(),
                                            nullptr, nullptr, "main", version.c_str(),
                                            flags, &code_, nullptr, nullptr)))
                        code_ = nullptr;
        }
        D3d9Shader::~D3d9Shader()
        {
                SAFE_RELEASE(code_);
        }

        //-----------------------------------------------------
        LPD3DXBUFFER D3d9Shader::getCode() const
        {
                return (LPD3DXBUFFER)code_;
        }

        D3d9VertexShader::D3d9VertexShader()
        {
                d3dShader_ = nullptr;
                d3dDevice_ = nullptr;
        }
        D3d9VertexShader::~D3d9VertexShader()
        {
                destroy();
        }

        //-----------------------------------------------------
        bool D3d9VertexShader::create(const D3d9Shader& shader)
        {
                d3dDevice_ = D3d9Device::getInterface();
                if(d3dDevice_ == nullptr)
                        return false;

                SAFE_RELEASE(d3dShader_);
                LPD3DXBUFFER code = shader.getCode();
                if(code == nullptr)
                        return false;

                if(FAILED(d3dDevice_->CreateVertexShader((DWORD*)code->GetBufferPointer(),
                                                         &d3dShader_)))
                {
                        d3dShader_ = nullptr;
                        return false;
                }

                return true;
        }

        //-----------------------------------------------------
        void D3d9VertexShader::destroy()
        {
                SAFE_RELEASE(d3dShader_);
        }

        //-----------------------------------------------------
        void D3d9VertexShader::set()
        {
                if(d3dDevice_ != nullptr)
                        d3dDevice_->SetVertexShader(d3dShader_);
        }

        D3d9PixelShader::D3d9PixelShader()
        {
                d3dShader_ = nullptr;
                d3dDevice_ = nullptr;
        }
        D3d9PixelShader::~D3d9PixelShader()
        {
                destroy();
        }

        //-----------------------------------------------------
        bool D3d9PixelShader::create(const D3d9Shader& shader)
        {
                d3dDevice_ = D3d9Device::getInterface();
                if(d3dDevice_ == nullptr)
                        return false;

                SAFE_RELEASE(d3dShader_);
                LPD3DXBUFFER code = shader.getCode();
                if(code == nullptr)
                        return false;

                if(FAILED(d3dDevice_->CreatePixelShader((DWORD*)code->GetBufferPointer(),
                                                        &d3dShader_)))
                {
                        d3dShader_ = nullptr;
                        return false;
                }

                return true;
        }

        //-----------------------------------------------------
        void D3d9PixelShader::destroy()
        {
                SAFE_RELEASE(d3dShader_);
        }

        //-----------------------------------------------------
        void D3d9PixelShader::set()
        {
                if(d3dDevice_ != nullptr)
                        d3dDevice_->SetPixelShader(d3dShader_);
        }

}
