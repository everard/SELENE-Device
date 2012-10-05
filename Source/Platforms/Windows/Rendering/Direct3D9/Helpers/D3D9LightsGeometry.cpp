// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "D3D9LightsGeometry.h"

namespace selene
{

        float D3d9LightsGeometry::vertices_[] =
        {
                // Directional light
                -1.0f, -1.0f, 1.0f, 0.0f,
                -1.0f, 1.0f, 1.0f, 0.0f,
                1.0f, -1.0f, 1.0f, 0.0f,
                1.0f, 1.0f, 1.0f, 0.0f,

                // Point light
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.707107f, 0.0f, 0.707107f, 0.0f,
                0.0f, 0.707107f, 0.707107f, 0.0f,
                0.707107f, 0.707107f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.707107f, -0.707107f, 0.0f,
                -0.707107f, 0.707107f, 0.0f, 0.0f,
                -0.707107f, 0.0f, -0.707107f, 0.0f,
                -1.0f, 0.0f, 0.0f, 0.0f,
                -0.707107f, -0.707107f, 0.0f, 0.0f,
                -0.707107f, 0.0f, 0.707107f, 0.0f,
                0.0f, -0.707107f, 0.707107f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, -0.707107f, 0.707107f, 0.0f,
                0.707107f, 0.0f, 0.707107f, 0.0f,
                0.707107f, -0.707107f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f, 0.0f,
                0.707107f, 0.0f, -0.707107f, 0.0f,
                0.707107f, 0.707107f, 0.0f, 0.0f,
                0.707107f, 0.0f, -0.707107f, 0.0f,
                0.0f, 0.707107f, -0.707107f, 0.0f,
                0.0f, 0.0f, -1.0f, 0.0f,
                0.0f, 0.0f, -1.0f, 0.0f,
                0.707107f, 0.0f, 0.707107f, 0.0f,
                0.707107f, 0.0f, 0.707107f, 0.0f,
                1.0f, 0.0f, 0.0f, 0.0f,
                0.707107f, 0.707107f, 0.0f, 0.0f,
                0.707107f, 0.707107f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.707107f, 0.707107f, 0.0f,
                -0.707107f, 0.0f, 0.707107f, 0.0f,
                -0.707107f, 0.707107f, 0.0f, 0.0f,
                -1.0f, 0.0f, 0.0f, 0.0f,
                -1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.707107f, 0.707107f, 0.0f,
                0.0f, 0.707107f, 0.707107f, 0.0f,
                0.0f, 0.707107f, 0.707107f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                -0.707107f, 0.707107f, 0.0f, 0.0f,
                -0.707107f, 0.707107f, 0.0f, 0.0f,
                0.0f, 0.707107f, -0.707107f, 0.0f,
                0.0f, 0.707107f, -0.707107f, 0.0f,
                0.0f, 0.707107f, -0.707107f, 0.0f,
                0.0f, 0.0f, -1.0f, 0.0f,
                -0.707107f, 0.0f, -0.707107f, 0.0f,
                0.0f, -0.707107f, -0.707107f, 0.0f,
                -0.707107f, -0.707107f, 0.0f, 0.0f,
                0.0f, -1.0f, 0.0f, 0.0f,
                0.0f, -0.707107f, 0.707107f, 0.0f,
                0.0f, -1.0f, 0.0f, 0.0f,
                0.707107f, -0.707107f, 0.0f, 0.0f,
                0.0f, -0.707107f, -0.707107f, 0.0f,
                0.707107f, 0.0f, -0.707107f, 0.0f,
                0.0f, 0.0f, -1.0f, 0.0f,
                0.0f, 0.0f, -1.0f, 0.0f,

                // Spot light
                0.707107f, 0.707107f, 1.0f, 0.0f,
                0.707107f, 0.707107f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                1.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.707107f, -0.707107f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, -1.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                -0.707107f, -0.707107f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                -1.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                -0.707107f, 0.707107f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 1.0f, 1.0f, 0.0f,
                0.707107f, 0.707107f, 1.0f, 0.0f,
                0.707107f, 0.707107f, 1.0f, 0.0f,
                1.0f, 0.0f, 1.0f, 0.0f,
                1.0f, 0.0f, 1.0f, 0.0f,
                1.0f, 0.0f, 1.0f, 0.0f,
                1.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.707107f, 0.707107f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                -0.707107f, 0.707107f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                -1.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                -0.707107f, -0.707107f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, -1.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.707107f, -0.707107f, 1.0f, 0.0f,
                1.0f, 0.0f, 1.0f, 0.0f,
                1.0f, 0.0f, 1.0f, 0.0f
        };

        const uint32_t D3d9LightsGeometry::batchSize_ = 30;

        const uint32_t D3d9LightsGeometry::offsets_[Renderer::NUM_OF_LIGHT_UNITS] =
        {
                // Directional light
                0,

                // Point light
                4 * D3d9LightsGeometry::numVertices_[Renderer::UNIT_LIGHT_DIRECTIONAL],

                // Spot light
                4 * D3d9LightsGeometry::numVertices_[Renderer::UNIT_LIGHT_DIRECTIONAL] +
                4 * D3d9LightsGeometry::numVertices_[Renderer::UNIT_LIGHT_POINT]
        };

        const uint32_t D3d9LightsGeometry::numVertices_[Renderer::NUM_OF_LIGHT_UNITS] =
        {
                // Directional light
                4,

                // Point light
                58,

                // Spot light
                38
        };

        const uint32_t D3d9LightsGeometry::vertexStrides_[Renderer::NUM_OF_LIGHT_UNITS] =
        {
                // Directional light
                4 * sizeof(float),

                // Point light
                4 * sizeof(float),

                // Spot light
                4 * sizeof(float)
        };

        const uint32_t D3d9LightsGeometry::vertexBufferSizes_[Renderer::NUM_OF_LIGHT_UNITS] =
        {
                // Directional light
                D3d9LightsGeometry::vertexStrides_[Renderer::UNIT_LIGHT_DIRECTIONAL] *
                D3d9LightsGeometry::numVertices_[Renderer::UNIT_LIGHT_DIRECTIONAL] *
                D3d9LightsGeometry::batchSize_,

                // Point light
                D3d9LightsGeometry::vertexStrides_[Renderer::UNIT_LIGHT_POINT] *
                D3d9LightsGeometry::numVertices_[Renderer::UNIT_LIGHT_POINT] *
                D3d9LightsGeometry::batchSize_,

                // Spot light
                D3d9LightsGeometry::vertexStrides_[Renderer::UNIT_LIGHT_SPOT] *
                D3d9LightsGeometry::numVertices_[Renderer::UNIT_LIGHT_SPOT] *
                D3d9LightsGeometry::batchSize_
        };

        D3d9LightsGeometry::D3d9LightsGeometry()
        {
                memset(d3dVertexDeclarations_, 0, sizeof(d3dVertexDeclarations_));
                memset(d3dVertexBuffers_, 0, sizeof(d3dVertexBuffers_));
                d3dDevice_ = nullptr;

                currentLightRenderingUnit_ = -1;
        }
        D3d9LightsGeometry::~D3d9LightsGeometry()
        {
                destroy();
        }

        //-----------------------------------------------------------------
        bool D3d9LightsGeometry::initialize()
        {
                d3dDevice_ = D3d9Device::getInterface();
                if(d3dDevice_ == nullptr)
                        return false;

                destroy();

                static D3DVERTEXELEMENT9 d3dVertexElements[] =
                {
                        {0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
                        D3DDECL_END()
                };

                for(uint8_t l = 0; l < Renderer::NUM_OF_LIGHT_UNITS; ++l)
                {
                        if(FAILED(d3dDevice_->CreateVertexDeclaration(d3dVertexElements,
                                                                      &d3dVertexDeclarations_[l])))
                        {
                                destroy();
                                return false;
                        }

                        if(FAILED(d3dDevice_->CreateVertexBuffer(vertexBufferSizes_[l], 0, 0, D3DPOOL_DEFAULT,
                                                                 &d3dVertexBuffers_[l], nullptr)))
                        {
                                destroy();
                                return false;
                        }

                        uint8_t* destinationBuffer = nullptr;
                        if(FAILED(d3dVertexBuffers_[l]->Lock(0, vertexBufferSizes_[l],
                                                             (void**)&destinationBuffer, 0)))
                        {
                                destroy();
                                return false;
                        }

                        float* vertices = vertices_ + offsets_[l];
                        for(uint32_t i = 0; i < batchSize_; ++i)
                        {
                                float index = (float)i;
                                for(uint32_t j = 0; j < numVertices_[l]; ++j)
                                        vertices[4 * j + 3] = index;

                                memcpy((void*)destinationBuffer, (void*)vertices,
                                       vertexStrides_[l] * numVertices_[l]);

                                destinationBuffer += vertexStrides_[l] * numVertices_[l];
                        }

                        d3dVertexBuffers_[l]->Unlock();
                }

                return true;
        }

        //-----------------------------------------------------------------
        void D3d9LightsGeometry::destroy()
        {
                for(uint8_t i = 0; i < Renderer::NUM_OF_LIGHT_UNITS; ++i)
                {
                        SAFE_RELEASE(d3dVertexDeclarations_[i]);
                        SAFE_RELEASE(d3dVertexBuffers_[i]);
                }

                currentLightRenderingUnit_ = -1;
        }

        //-----------------------------------------------------------------
        uint32_t D3d9LightsGeometry::getBatchSize() const
        {
                return batchSize_;
        }

        //-----------------------------------------------------------------
        bool D3d9LightsGeometry::beginRendering(int16_t lightRenderingUnit)
        {
                if(d3dDevice_ == nullptr || lightRenderingUnit < 0 ||
                   lightRenderingUnit >= Renderer::NUM_OF_LIGHT_UNITS)
                {
                        currentLightRenderingUnit_ = -1;
                        return false;
                }

                if(d3dVertexDeclarations_[lightRenderingUnit] == nullptr ||
                   d3dVertexBuffers_[lightRenderingUnit] == nullptr)
                {
                        currentLightRenderingUnit_ = -1;
                        return false;
                }

                currentLightRenderingUnit_ = lightRenderingUnit;

                d3dDevice_->SetVertexDeclaration(d3dVertexDeclarations_[lightRenderingUnit]);
                d3dDevice_->SetStreamSource(0, d3dVertexBuffers_[lightRenderingUnit],
                                            0, vertexStrides_[lightRenderingUnit]);
                d3dDevice_->SetIndices(nullptr);

                return true;
        }

        //-----------------------------------------------------------------
        void D3d9LightsGeometry::endRendering()
        {
                currentLightRenderingUnit_ = -1;
        }

        //-----------------------------------------------------------------
        bool D3d9LightsGeometry::render(uint32_t numLights)
        {
                if(d3dDevice_ == nullptr || currentLightRenderingUnit_ < 0 ||
                   numLights == 0 || numLights > batchSize_)
                        return false;

                d3dDevice_->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0,
                                          numLights * numVertices_[currentLightRenderingUnit_] - 2);

                return true;
        }

}
