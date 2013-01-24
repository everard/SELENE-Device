// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "D3D9Shader.h"

#include "../../../../../Engine/Core/FileManager/FileManager.h"
#include "../../../../../Engine/Core/Helpers/Array.h"
#include "../D3D9Renderer.h"

namespace selene
{

        static const char* pixelShaderPositionDecodingFromR32f =
                "float4 EncodePos(float4 pos, float4 proj)"
                "{"
                "        return (pos.z / proj.w).xxxx;"
                "}"
                "float DecodeZEye(float4 ZEnc, float4 proj)"
                "{"
                "        return ZEnc.r * proj.w;"
                "}"
                "float3 DecodePos(float4 ZEnc, float2 XY, float4 proj, float4 unproj)"
                "{"
                "        float zv = DecodeZEye(ZEnc, proj);"
                "        float4 p = float4(XY, 1.0, 0.0);"
                "        float4 orig = p * unproj * zv.xxxx;"
                "        return orig.xyz;"
                "}";

        static const char* pixelShaderPositionDecodingFromRGBA8 =
                "float4 EncodePos(float4 pos, float4 proj)"
                "{"
                "        float4 enc = float4(16777216.0, 65536.0, 256.0, 1.0) * (pos.z / proj.w);"
                "        enc  = frac(enc);"
                "        enc -= enc.xxyz * float4(0.0, 1.0 / 256.0, 1.0 / 256.0, 1.0 / 256.0);"
                "        return enc;"
                "}"
                "float DecodeZEye(float4 ZEnc, float4 proj)"
                "{"
                "        return dot(ZEnc, float4(1.0 / (256.0 * 256.0 * 256.0), 1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0)) * proj.w;"
                "}"
                "float3 DecodePos(float4 ZEnc, float2 XY, float4 proj, float4 unproj)"
                "{"
                "        float zv = DecodeZEye(ZEnc, proj);"
                "        float4 p = float4(XY, 1.0, 0.0);"
                "        float4 orig = p * unproj * zv.xxxx;"
                "        return orig.xyz;"
                "}";

        D3d9Capabilities::D3d9Capabilities()
        {
                isR32fRenderTargetFormatSupported = false;
                isMultipleRenderTargetSupported   = false;
                isThirdShaderModelSupported       = false;
                maxTextureAnisotropy = 1;
        }
        D3d9Capabilities::~D3d9Capabilities() {}

        D3d9FrameParameters::D3d9FrameParameters()
        {
                resultRenderTarget = 0;
        }
        D3d9FrameParameters::~D3d9FrameParameters() {}

        D3d9Shader::D3d9Shader(const char* name, const std::string& version, DWORD flags,
                               uint8_t libraryType, const D3d9Capabilities& capabilities)
        {
                code_ = nullptr;

                HRSRC hResource = ::FindResource(GetModuleHandle(nullptr), name, RT_RCDATA);
                if(hResource == nullptr)
                        return;

                DWORD size = ::SizeofResource(GetModuleHandle(nullptr), hResource);
                if(size == 0)
                        return;

                HGLOBAL hResourceData = ::LoadResource(GetModuleHandle(nullptr), hResource);
                if(hResourceData == nullptr)
                        return;

                LPVOID buffer = ::LockResource(hResourceData);
                if(buffer == nullptr)
                        return;

                Array<uint8_t, uint32_t> fileContent;
                uint32_t length = static_cast<uint32_t>(size + 1);
                if(!fileContent.create(length))
                        return;

                memcpy(reinterpret_cast<char*>(&fileContent[0]), buffer, size);
                fileContent[length - 1] = 0;

                std::string sourceCode = reinterpret_cast<char*>(&fileContent[0]);

                std::string library;
                switch(libraryType)
                {
                        case LIBRARY_VERTEX_SHADER:
                                library =
                                        "float4 QuatMul(float4 q1, float4 q2)"
                                        "{"
                                        "        float3 im = q1.w * q2.xyz + q1.xyz * q2.w + cross(q1.xyz, q2.xyz);"
                                        "        float4 dt = q1 * q2;"
                                        "        float re = dot(dt, float4(-1.0, -1.0, -1.0, 1.0));"
                                        "        return float4(im, re);"
                                        "}"
                                        "float4 QuatRotate(float3 p, float4 q)"
                                        "{"
                                        "        float4 temp = QuatMul(q, float4(p, 0.0));"
                                        "        return QuatMul(temp, float4(-q.x, -q.y, -q.z, q.w));"
                                        "}"
                                        "float3 Transform(float4 offset, float4 rot, float3 pos)"
                                        "{"
                                        "        return offset.xyz + QuatRotate(pos, rot).xyz;"
                                        "}";
                                break;

                        case LIBRARY_PIXEL_SHADER:
                                library =
                                        "float3 EncodeNormal(float3 n)"
                                        "{"
                                        "        return 0.5 * n + 0.5;"
                                        "}"
                                        "half3 DecodeNormal(half3 enc)"
                                        "{"
                                        "        return 2.0 * enc - 1.0;"
                                        "}";
                                if(capabilities.isR32fRenderTargetFormatSupported)
                                        library += pixelShaderPositionDecodingFromR32f;
                                else
                                        library += pixelShaderPositionDecodingFromRGBA8;
                                break;
                }

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
                d3dDevice_ = D3d9Renderer::getDevice();
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
                d3dDevice_ = D3d9Renderer::getDevice();
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
