// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "D3D9FullScreenQuad.h"
#include "../D3D9Renderer.h"

namespace selene
{

        const uint32_t D3d9FullScreenQuad::vertexStride_ = 4 * sizeof(float);

        D3d9FullScreenQuad::D3d9FullScreenQuad()
        {
                d3dVertexDeclaration_ = nullptr;
                d3dVertexBuffer_ = nullptr;
                d3dDevice_ = nullptr;
        }
        D3d9FullScreenQuad::~D3d9FullScreenQuad()
        {
                destroy();
        }

        //-----------------------------------
        bool D3d9FullScreenQuad::initialize()
        {
                d3dDevice_ = D3d9Renderer::getDevice();
                if(d3dDevice_ == nullptr)
                        return false;

                static D3DVERTEXELEMENT9 d3dVertexElements[] =
                {
                        {0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
                        D3DDECL_END()
                };

                static float vertices[] =
                {
                        -1.0f, -1.0f, 0.0f, 1.0f,
                        -1.0f, 1.0f, 0.0f, 0.0f,
                        1.0f, -1.0f, 1.0f, 1.0f,
                        1.0f, 1.0f, 1.0f, 0.0f,
                };

                destroy();

                if(FAILED(d3dDevice_->CreateVertexDeclaration(d3dVertexElements,
                                                              &d3dVertexDeclaration_)))
                {
                        destroy();
                        return false;
                }

                if(FAILED(d3dDevice_->CreateVertexBuffer(4 * vertexStride_, 0, 0, D3DPOOL_DEFAULT,
                                                         &d3dVertexBuffer_, nullptr)))
                {
                        destroy();
                        return false;
                }

                void* destinationBuffer = nullptr;
                if(FAILED(d3dVertexBuffer_->Lock(0, 4 * vertexStride_, &destinationBuffer, 0)))
                {
                        destroy();
                        return false;
                }
                memcpy(destinationBuffer, (void*)vertices, 4 * vertexStride_);
                d3dVertexBuffer_->Unlock();

                return true;
        }

        //-----------------------------------
        void D3d9FullScreenQuad::destroy()
        {
                SAFE_RELEASE(d3dVertexDeclaration_);
                SAFE_RELEASE(d3dVertexBuffer_);
        }

        //-----------------------------------
        bool D3d9FullScreenQuad::render()
        {
                if(d3dDevice_ == nullptr || d3dVertexBuffer_ == nullptr ||
                   d3dVertexDeclaration_ == nullptr)
                        return false;

                d3dDevice_->SetVertexDeclaration(d3dVertexDeclaration_);
                d3dDevice_->SetStreamSource(0, d3dVertexBuffer_, 0, vertexStride_);
                d3dDevice_->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
                return true;
        }

}
