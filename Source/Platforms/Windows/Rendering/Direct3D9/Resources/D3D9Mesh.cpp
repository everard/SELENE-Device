// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "D3D9Mesh.h"
#include "../D3D9Renderer.h"

#include <new>

namespace selene
{

        D3d9Mesh::D3d9Mesh(const char* name): Mesh(name)
        {
                for(uint8_t i = 0; i < NUM_OF_VERTEX_STREAMS; ++i)
                        d3dVertexBuffers_[i] = nullptr;
                d3dIndexBuffer_  = nullptr;
        }
        D3d9Mesh::~D3d9Mesh()
        {
                destroy();
        }

        //----------------------
        bool D3d9Mesh::retain()
        {
                LPDIRECT3DDEVICE9 d3dDevice = D3d9Renderer::getDevice();
                if(d3dDevice == nullptr)
                        return false;

                // destroy D3D9 mesh data if any
                destroy();

                // compute size of vertex and index buffers
                uint32_t vertexBufferSizes[NUM_OF_VERTEX_STREAMS];
                for(uint8_t i = 0; i < NUM_OF_VERTEX_STREAMS; ++i)
                        vertexBufferSizes[i] = data_.vertices[i].getSize() * data_.vertices[i].getStride();

                uint32_t indexBufferSize = 3 * data_.faces.getSize() * data_.faces.getStride();

                // create vertex buffers
                for(uint8_t i = VERTEX_STREAM_POSITIONS; i <= VERTEX_STREAM_TEXTURE_COORDINATES; ++i)
                {
                        if(FAILED(d3dDevice->CreateVertexBuffer(vertexBufferSizes[i], 0, 0,
                                                                D3DPOOL_DEFAULT, &d3dVertexBuffers_[i], nullptr)))
                        {
                                destroy();
                                return false;
                        }
                }

                if(hasSkeleton())
                {
                        if(FAILED(d3dDevice->CreateVertexBuffer(vertexBufferSizes[VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS], 0, 0,
                                                                D3DPOOL_DEFAULT, &d3dVertexBuffers_[VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS], nullptr)))
                        {
                                destroy();
                                return false;
                        }
                }

                // create index buffer
                D3DFORMAT format = D3DFMT_INDEX16;
                if(data_.faces.getStride() == 4)
                        format = D3DFMT_INDEX32;

                if(FAILED(d3dDevice->CreateIndexBuffer(indexBufferSize, 0, format,
                                                       D3DPOOL_DEFAULT, &d3dIndexBuffer_, nullptr)))
                {
                        destroy();
                        return false;
                }

                // fill vertex buffers
                for(uint8_t i = VERTEX_STREAM_POSITIONS; i <= VERTEX_STREAM_TEXTURE_COORDINATES; ++i)
                {
                        void* destinationBuffer = nullptr;
                        if(FAILED(d3dVertexBuffers_[i]->Lock(0, vertexBufferSizes[i], &destinationBuffer, 0)))
                        {
                                destroy();
                                return false;
                        }
                        memcpy(destinationBuffer, &data_.vertices[i][0], vertexBufferSizes[i]);
                        d3dVertexBuffers_[i]->Unlock();
                }

                if(hasSkeleton())
                {
                        void* destinationBuffer = nullptr;
                        if(FAILED(d3dVertexBuffers_[VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS]->Lock(0,
                                  vertexBufferSizes[VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS], &destinationBuffer, 0)))
                        {
                                destroy();
                                return false;
                        }
                        memcpy(destinationBuffer, &data_.vertices[VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS][0],
                               vertexBufferSizes[VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS]);
                        d3dVertexBuffers_[VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS]->Unlock();
                }

                // fill index buffer
                void* destinationBuffer = nullptr;
                if(FAILED(d3dIndexBuffer_->Lock(0, indexBufferSize, &destinationBuffer, 0)))
                {
                        destroy();
                        return false;
                }
                memcpy(destinationBuffer, &data_.faces[0], indexBufferSize);
                d3dIndexBuffer_->Unlock();

                return true;
        }

        //----------------------
        void D3d9Mesh::discard()
        {
                destroy();
        }

        //----------------------
        void D3d9Mesh::destroy()
        {
                for(uint8_t i = 0; i < NUM_OF_VERTEX_STREAMS; ++i)
                {
                        SAFE_RELEASE(d3dVertexBuffers_[i]);
                }
                SAFE_RELEASE(d3dIndexBuffer_);
        }

}
