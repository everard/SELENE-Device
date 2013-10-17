// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "D3D9Shader.h"

#include "../../../../../Engine/Core/FileManager/FileManager.h"
#include "../../../../../Engine/Core/Helpers/Array.h"
#include "../D3D9Renderer.h"

namespace selene
{

        D3d9Shader::D3d9Shader(const char* sourceCode, const char* version, DWORD flags, uint8_t libraryType,
                               const D3d9Capabilities& capabilities): code_(nullptr)
        {
                static const char* pixelShaderPositionDecodingFromR32f =
                        "float4 encodePosition(float4 position, float4 projectionParameters)"
                        "{"
                        "        return (position.z / projectionParameters.w).xxxx;"
                        "}"
                        "float decodeEyeZ(float4 encodedEyeZ, float4 projectionParameters)"
                        "{"
                        "        return encodedEyeZ.r * projectionParameters.w;"
                        "}";

                static const char* pixelShaderPositionDecodingFromRGBA8 =
                        "float4 encodePosition(float4 position, float4 projectionParameters)"
                        "{"
                        "        float4 result = float4(16777216.0, 65536.0, 256.0, 1.0) *"
                        "                        position.z / projectionParameters.w;"
                        "        result  = frac(result);"
                        "        result -= result.xxyz * float4(0.0, 1.0 / 256.0, 1.0 / 256.0, 1.0 / 256.0);"
                        "        return result;"
                        "}"
                        "float decodeEyeZ(float4 encodedEyeZ, float4 projectionParameters)"
                        "{"
                        "        return dot(encodedEyeZ, float4(1.0 / (256.0 * 256.0 * 256.0),"
                        "                                       1.0 / (256.0 * 256.0),"
                        "                                       1.0 /  256.0, 1.0)) * projectionParameters.w;"
                        "}";

                std::string library;
                switch(libraryType)
                {
                        case LIBRARY_VERTEX_SHADER:
                                library =
                                        "float4 multiplyQuaternions(float4 quaternion0, float4 quaternion1)"
                                        "{"
                                        "        float3 v = quaternion0.w   * quaternion1.xyz +"
                                        "                   quaternion0.xyz * quaternion1.w   +"
                                        "                   cross(quaternion0.xyz, quaternion1.xyz);"
                                        "        float4 b = quaternion0 * quaternion1;"
                                        "        float  w = dot(b, float4(-1.0, -1.0, -1.0, 1.0));"
                                        "        return float4(v, w);"
                                        "}"
                                        "float4 rotateVector(float3 targetVector, float4 quaternion)"
                                        "{"
                                        "        float4 q = multiplyQuaternions(quaternion,"
                                        "                                       float4(targetVector, 0.0));"
                                        "        return multiplyQuaternions(q, float4(-quaternion.x, -quaternion.y,"
                                        "                                             -quaternion.z,  quaternion.w));"
                                        "}"
                                        "float3 transformPosition(float4 offset, float4 rotation, float3 position)"
                                        "{"
                                        "        return offset.xyz + rotateVector(position, rotation).xyz;"
                                        "}";
                                break;

                        case LIBRARY_PIXEL_SHADER:
                                if(capabilities.isR32fRenderTargetFormatSupported())
                                        library = pixelShaderPositionDecodingFromR32f;
                                else
                                        library = pixelShaderPositionDecodingFromRGBA8;

                                library +=
                                        "float3 decodePosition(float4 z, float2 xy, float4 projectionParameters,"
                                        "                      float4 unprojectionVector)"
                                        "{"
                                        "        float decodedEyeZ = decodeEyeZ(z, projectionParameters);"
                                        "        float4 p = float4(xy, 1.0, 0.0);"
                                        "        float4 result = p * unprojectionVector * decodedEyeZ.xxxx;"
                                        "        return result.xyz;"
                                        "}"
                                        "float3 encodeNormal(float3 normal)"
                                        "{"
                                        "        return 0.5 * normal + 0.5;"
                                        "}"
                                        "half3 decodeNormal(half3 encodedNormal)"
                                        "{"
                                        "        return 2.0 * encodedNormal - 1.0;"
                                        "}";
                                break;
                }

                std::string sourceCodeWithLibrary = library + sourceCode;

                if(FAILED(D3DXCompileShader(sourceCodeWithLibrary.c_str(),
                                            sourceCodeWithLibrary.length(),
                                            nullptr, nullptr, "main", version,
                                            flags, &code_, nullptr, nullptr)))
                        code_ = nullptr;
        }
        D3d9Shader::D3d9Shader(const D3d9Shader& other): code_(nullptr)
        {
                code_ = other.code_;

                if(code_ != nullptr)
                        code_->AddRef();
        }
        D3d9Shader::~D3d9Shader()
        {
                SAFE_RELEASE(code_);
        }
        D3d9Shader& D3d9Shader::operator =(const D3d9Shader& other)
        {
                SAFE_RELEASE(code_);
                code_ = other.code_;

                if(code_ != nullptr)
                        code_->AddRef();

                return *this;
        }

        //------------------------------------------------------------------------------------------------------------
        LPD3DXBUFFER D3d9Shader::getCode() const
        {
                return code_;
        }

        D3d9VertexShader::D3d9VertexShader(): d3dShader_(nullptr), d3dDevice_(nullptr) {}
        D3d9VertexShader::~D3d9VertexShader()
        {
                destroy();
        }

        //------------------------------------------------------------------------------------------------------------
        bool D3d9VertexShader::create(const D3d9Shader& shader)
        {
                d3dDevice_ = D3d9Renderer::getDevice();
                if(d3dDevice_ == nullptr)
                        return false;

                SAFE_RELEASE(d3dShader_);
                LPD3DXBUFFER code = shader.getCode();
                if(code == nullptr)
                        return false;

                if(FAILED(d3dDevice_->CreateVertexShader(reinterpret_cast<DWORD*>(code->GetBufferPointer()),
                                                         &d3dShader_)))
                {
                        d3dShader_ = nullptr;
                        return false;
                }

                return true;
        }

        //------------------------------------------------------------------------------------------------------------
        void D3d9VertexShader::destroy()
        {
                SAFE_RELEASE(d3dShader_);
                d3dDevice_ = nullptr;
        }

        //------------------------------------------------------------------------------------------------------------
        void D3d9VertexShader::set()
        {
                if(d3dDevice_ != nullptr)
                        d3dDevice_->SetVertexShader(d3dShader_);
        }

        D3d9PixelShader::D3d9PixelShader(): d3dShader_(nullptr), d3dDevice_(nullptr) {}
        D3d9PixelShader::~D3d9PixelShader()
        {
                destroy();
        }

        //------------------------------------------------------------------------------------------------------------
        bool D3d9PixelShader::create(const D3d9Shader& shader)
        {
                d3dDevice_ = D3d9Renderer::getDevice();
                if(d3dDevice_ == nullptr)
                        return false;

                SAFE_RELEASE(d3dShader_);
                LPD3DXBUFFER code = shader.getCode();
                if(code == nullptr)
                        return false;

                if(FAILED(d3dDevice_->CreatePixelShader(reinterpret_cast<DWORD*>(code->GetBufferPointer()),
                                                        &d3dShader_)))
                {
                        d3dShader_ = nullptr;
                        return false;
                }

                return true;
        }

        //------------------------------------------------------------------------------------------------------------
        void D3d9PixelShader::destroy()
        {
                SAFE_RELEASE(d3dShader_);
                d3dDevice_ = nullptr;
        }

        //------------------------------------------------------------------------------------------------------------
        void D3d9PixelShader::set()
        {
                if(d3dDevice_ != nullptr)
                        d3dDevice_->SetPixelShader(d3dShader_);
        }

}
