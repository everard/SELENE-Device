// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "D3D9SSAOGeometry.h"

namespace selene
{

        float D3d9SsaoGeometry::vertices_[] =
        {
                // radius 1.0
                -1.0f, -1.0f, 0.0f, 1.0f,
                -0.5f, -0.5f, -0.5f, 1.0f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                -0.5f, -0.5f, -0.5f, 1.0f,
                1.0f, -1.0f, 1.0f, 1.0f,
                -0.5f, -0.5f, -0.5f, 1.0f,
                1.0f, -1.0f, 1.0f, 1.0f,
                -0.5f, -0.5f, -0.5f, 1.0f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                -0.5f, -0.5f, -0.5f, 1.0f,
                1.0f, 1.0f, 1.0f, 0.0f,
                -0.5f, -0.5f, -0.5f, 1.0f,

                -1.0f, -1.0f, 0.0f, 1.0f,
                0.5f, -0.5f, -0.5f, 1.0f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                0.5f, -0.5f, -0.5f, 1.0f,
                1.0f, -1.0f, 1.0f, 1.0f,
                0.5f, -0.5f, -0.5f, 1.0f,
                1.0f, -1.0f, 1.0f, 1.0f,
                0.5f, -0.5f, -0.5f, 1.0f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                0.5f, -0.5f, -0.5f, 1.0f,
                1.0f, 1.0f, 1.0f, 0.0f,
                0.5f, -0.5f, -0.5f, 1.0f,

                -1.0f, -1.0f, 0.0f, 1.0f,
                -0.5f, 0.5f, -0.5f, 1.0f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                -0.5f, 0.5f, -0.5f, 1.0f,
                1.0f, -1.0f, 1.0f, 1.0f,
                -0.5f, 0.5f, -0.5f, 1.0f,
                1.0f, -1.0f, 1.0f, 1.0f,
                -0.5f, 0.5f, -0.5f, 1.0f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                -0.5f, 0.5f, -0.5f, 1.0f,
                1.0f, 1.0f, 1.0f, 0.0f,
                -0.5f, 0.5f, -0.5f, 1.0f,

                -1.0f, -1.0f, 0.0f, 1.0f,
                0.5f, 0.5f, -0.5f, 1.0f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                0.5f, 0.5f, -0.5f, 1.0f,
                1.0f, -1.0f, 1.0f, 1.0f,
                0.5f, 0.5f, -0.5f, 1.0f,
                1.0f, -1.0f, 1.0f, 1.0f,
                0.5f, 0.5f, -0.5f, 1.0f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                0.5f, 0.5f, -0.5f, 1.0f,
                1.0f, 1.0f, 1.0f, 0.0f,
                0.5f, 0.5f, -0.5f, 1.0f,

                -1.0f, -1.0f, 0.0f, 1.0f,
                -0.5f, -0.5f, 0.5f, 1.0f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                -0.5f, -0.5f, 0.5f, 1.0f,
                1.0f, -1.0f, 1.0f, 1.0f,
                -0.5f, -0.5f, 0.5f, 1.0f,
                1.0f, -1.0f, 1.0f, 1.0f,
                -0.5f, -0.5f, 0.5f, 1.0f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                -0.5f, -0.5f, 0.5f, 1.0f,
                1.0f, 1.0f, 1.0f, 0.0f,
                -0.5f, -0.5f, 0.5f, 1.0f,

                -1.0f, -1.0f, 0.0f, 1.0f,
                0.5f, -0.5f, 0.5f, 1.0f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                0.5f, -0.5f, 0.5f, 1.0f,
                1.0f, -1.0f, 1.0f, 1.0f,
                0.5f, -0.5f, 0.5f, 1.0f,
                1.0f, -1.0f, 1.0f, 1.0f,
                0.5f, -0.5f, 0.5f, 1.0f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                0.5f, -0.5f, 0.5f, 1.0f,
                1.0f, 1.0f, 1.0f, 0.0f,
                0.5f, -0.5f, 0.5f, 1.0f,

                -1.0f, -1.0f, 0.0f, 1.0f,
                -0.5f, 0.5f, 0.5f, 1.0f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                -0.5f, 0.5f, 0.5f, 1.0f,
                1.0f, -1.0f, 1.0f, 1.0f,
                -0.5f, 0.5f, 0.5f, 1.0f,
                1.0f, -1.0f, 1.0f, 1.0f,
                -0.5f, 0.5f, 0.5f, 1.0f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                -0.5f, 0.5f, 0.5f, 1.0f,
                1.0f, 1.0f, 1.0f, 0.0f,
                -0.5f, 0.5f, 0.5f, 1.0f,

                -1.0f, -1.0f, 0.0f, 1.0f,
                0.5f, 0.5f, 0.5f, 1.0f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                0.5f, 0.5f, 0.5f, 1.0f,
                1.0f, -1.0f, 1.0f, 1.0f,
                0.5f, 0.5f, 0.5f, 1.0f,
                1.0f, -1.0f, 1.0f, 1.0f,
                0.5f, 0.5f, 0.5f, 1.0f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                0.5f, 0.5f, 0.5f, 1.0f,
                1.0f, 1.0f, 1.0f, 0.0f,
                0.5f, 0.5f, 0.5f, 1.0f,

                // radius 0.5
                -1.0f, -1.0f, 0.0f, 1.0f,
                -0.5f, -0.5f, -0.5f, 0.5f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                -0.5f, -0.5f, -0.5f, 0.5f,
                1.0f, -1.0f, 1.0f, 1.0f,
                -0.5f, -0.5f, -0.5f, 0.5f,
                1.0f, -1.0f, 1.0f, 1.0f,
                -0.5f, -0.5f, -0.5f, 0.5f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                -0.5f, -0.5f, -0.5f, 0.5f,
                1.0f, 1.0f, 1.0f, 0.0f,
                -0.5f, -0.5f, -0.5f, 0.5f,

                -1.0f, -1.0f, 0.0f, 1.0f,
                0.5f, -0.5f, -0.5f, 0.5f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                0.5f, -0.5f, -0.5f, 0.5f,
                1.0f, -1.0f, 1.0f, 1.0f,
                0.5f, -0.5f, -0.5f, 0.5f,
                1.0f, -1.0f, 1.0f, 1.0f,
                0.5f, -0.5f, -0.5f, 0.5f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                0.5f, -0.5f, -0.5f, 0.5f,
                1.0f, 1.0f, 1.0f, 0.0f,
                0.5f, -0.5f, -0.5f, 0.5f,

                -1.0f, -1.0f, 0.0f, 1.0f,
                -0.5f, 0.5f, -0.5f, 0.5f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                -0.5f, 0.5f, -0.5f, 0.5f,
                1.0f, -1.0f, 1.0f, 1.0f,
                -0.5f, 0.5f, -0.5f, 0.5f,
                1.0f, -1.0f, 1.0f, 1.0f,
                -0.5f, 0.5f, -0.5f, 0.5f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                -0.5f, 0.5f, -0.5f, 0.5f,
                1.0f, 1.0f, 1.0f, 0.0f,
                -0.5f, 0.5f, -0.5f, 0.5f,

                -1.0f, -1.0f, 0.0f, 1.0f,
                0.5f, 0.5f, -0.5f, 0.5f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                0.5f, 0.5f, -0.5f, 0.5f,
                1.0f, -1.0f, 1.0f, 1.0f,
                0.5f, 0.5f, -0.5f, 0.5f,
                1.0f, -1.0f, 1.0f, 1.0f,
                0.5f, 0.5f, -0.5f, 0.5f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                0.5f, 0.5f, -0.5f, 0.5f,
                1.0f, 1.0f, 1.0f, 0.0f,
                0.5f, 0.5f, -0.5f, 0.5f,

                -1.0f, -1.0f, 0.0f, 1.0f,
                -0.5f, -0.5f, 0.5f, 0.5f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                -0.5f, -0.5f, 0.5f, 0.5f,
                1.0f, -1.0f, 1.0f, 1.0f,
                -0.5f, -0.5f, 0.5f, 0.5f,
                1.0f, -1.0f, 1.0f, 1.0f,
                -0.5f, -0.5f, 0.5f, 0.5f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                -0.5f, -0.5f, 0.5f, 0.5f,
                1.0f, 1.0f, 1.0f, 0.0f,
                -0.5f, -0.5f, 0.5f, 0.5f,

                -1.0f, -1.0f, 0.0f, 1.0f,
                0.5f, -0.5f, 0.5f, 0.5f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                0.5f, -0.5f, 0.5f, 0.5f,
                1.0f, -1.0f, 1.0f, 1.0f,
                0.5f, -0.5f, 0.5f, 0.5f,
                1.0f, -1.0f, 1.0f, 1.0f,
                0.5f, -0.5f, 0.5f, 0.5f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                0.5f, -0.5f, 0.5f, 0.5f,
                1.0f, 1.0f, 1.0f, 0.0f,
                0.5f, -0.5f, 0.5f, 0.5f,

                -1.0f, -1.0f, 0.0f, 1.0f,
                -0.5f, 0.5f, 0.5f, 0.5f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                -0.5f, 0.5f, 0.5f, 0.5f,
                1.0f, -1.0f, 1.0f, 1.0f,
                -0.5f, 0.5f, 0.5f, 0.5f,
                1.0f, -1.0f, 1.0f, 1.0f,
                -0.5f, 0.5f, 0.5f, 0.5f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                -0.5f, 0.5f, 0.5f, 0.5f,
                1.0f, 1.0f, 1.0f, 0.0f,
                -0.5f, 0.5f, 0.5f, 0.5f,

                -1.0f, -1.0f, 0.0f, 1.0f,
                0.5f, 0.5f, 0.5f, 0.5f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                0.5f, 0.5f, 0.5f, 0.5f,
                1.0f, -1.0f, 1.0f, 1.0f,
                0.5f, 0.5f, 0.5f, 0.5f,
                1.0f, -1.0f, 1.0f, 1.0f,
                0.5f, 0.5f, 0.5f, 0.5f,
                -1.0f, 1.0f, 0.0f, 0.0f,
                0.5f, 0.5f, 0.5f, 0.5f,
                1.0f, 1.0f, 1.0f, 0.0f,
                0.5f, 0.5f, 0.5f, 0.5f
        };

        const uint32_t D3d9SsaoGeometry::vertexStride_ = 8 * sizeof(float);

        const uint32_t D3d9SsaoGeometry::numVertices_ = 96;

        const uint32_t D3d9SsaoGeometry::numFaces_ = 32;

        D3d9SsaoGeometry::D3d9SsaoGeometry()
        {
                d3dVertexDeclaration_ = nullptr;
                d3dVertexBuffer_ = nullptr;
                d3dDevice_ = nullptr;
        }
        D3d9SsaoGeometry::~D3d9SsaoGeometry()
        {
                destroy();
        }

        //---------------------------------
        bool D3d9SsaoGeometry::initialize()
        {
                d3dDevice_ = D3d9Device::getInterface();
                if(d3dDevice_ == nullptr)
                        return false;

                static D3DVERTEXELEMENT9 d3dVertexElements[] =
                {
                        {0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
                        {0, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
                        D3DDECL_END()
                };

                destroy();

                if(FAILED(d3dDevice_->CreateVertexDeclaration(d3dVertexElements,
                                                              &d3dVertexDeclaration_)))
                {
                        destroy();
                        return false;
                }

                if(FAILED(d3dDevice_->CreateVertexBuffer(numVertices_ * vertexStride_, 0, 0, D3DPOOL_DEFAULT,
                                                         &d3dVertexBuffer_, nullptr)))
                {
                        destroy();
                        return false;
                }

                void* destinationBuffer = nullptr;
                if(FAILED(d3dVertexBuffer_->Lock(0, numVertices_ * vertexStride_, &destinationBuffer, 0)))
                {
                        destroy();
                        return false;
                }
                memcpy(destinationBuffer, (void*)vertices_, numVertices_ * vertexStride_);
                d3dVertexBuffer_->Unlock();

                return true;
        }

        //---------------------------------
        void D3d9SsaoGeometry::destroy()
        {
                SAFE_RELEASE(d3dVertexDeclaration_);
                SAFE_RELEASE(d3dVertexBuffer_);
        }

        //---------------------------------
        bool D3d9SsaoGeometry::render()
        {
                if(d3dDevice_ == nullptr || d3dVertexBuffer_ == nullptr ||
                   d3dVertexDeclaration_ == nullptr)
                        return false;

                d3dDevice_->SetVertexDeclaration(d3dVertexDeclaration_);
                d3dDevice_->SetStreamSource(0, d3dVertexBuffer_, 0, vertexStride_);
                d3dDevice_->DrawPrimitive(D3DPT_TRIANGLELIST, 0, numFaces_);
                return true;
        }

}
