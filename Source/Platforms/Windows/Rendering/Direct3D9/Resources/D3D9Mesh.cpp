// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "D3D9Mesh.h"
#include <new>

namespace selene
{

        D3d9Mesh::D3d9Mesh(const char* name): Mesh(name)
        {
                d3dVertexDeclaration_ = nullptr;
                d3dVertexBuffer_ = nullptr;
                d3dIndexBuffer_  = nullptr;
                d3dVertexElements_ = nullptr;
        }
        D3d9Mesh::~D3d9Mesh()
        {
                destroy();
        }

        //----------------------
        bool D3d9Mesh::prepare()
        {
                LPDIRECT3DDEVICE9 d3dDevice = D3d9Device::getInterface();
                if(d3dDevice == nullptr)
                        return false;

                // destroy D3D9 mesh data if any
                destroy();

                // validate
                if(data_.vertexElements.isEmpty())
                        return false;

                static Mesh::VertexElement staticMeshVertexElements[] =
                {
                        Mesh::VertexElement(VERTEX_ELEMENT_POSITION,
                                            VERTEX_ELEMENT_SIZE_FLOAT3,
                                            0, 0),
                        Mesh::VertexElement(VERTEX_ELEMENT_NORMAL,
                                            VERTEX_ELEMENT_SIZE_FLOAT3,
                                            12, 0),
                        Mesh::VertexElement(VERTEX_ELEMENT_TEXCOORD,
                                            VERTEX_ELEMENT_SIZE_FLOAT2,
                                            24, 0),
                        Mesh::VertexElement(VERTEX_ELEMENT_TANGENT,
                                            VERTEX_ELEMENT_SIZE_FLOAT4,
                                            32, 0)
                };
                static const uint32_t numStaticMeshVertexElements = sizeof(staticMeshVertexElements) / sizeof(staticMeshVertexElements[0]);

                static Mesh::VertexElement skinMeshVertexElements[] =
                {
                        Mesh::VertexElement(VERTEX_ELEMENT_POSITION,
                                            VERTEX_ELEMENT_SIZE_FLOAT3,
                                            0, 0),
                        Mesh::VertexElement(VERTEX_ELEMENT_NORMAL,
                                            VERTEX_ELEMENT_SIZE_FLOAT3,
                                            12, 0),
                        Mesh::VertexElement(VERTEX_ELEMENT_TEXCOORD,
                                            VERTEX_ELEMENT_SIZE_FLOAT2,
                                            24, 0),
                        Mesh::VertexElement(VERTEX_ELEMENT_TANGENT,
                                            VERTEX_ELEMENT_SIZE_FLOAT4,
                                            32, 0),
                        Mesh::VertexElement(VERTEX_ELEMENT_TEXCOORD,
                                            VERTEX_ELEMENT_SIZE_FLOAT4,
                                            48, 1),
                        Mesh::VertexElement(VERTEX_ELEMENT_TEXCOORD,
                                            VERTEX_ELEMENT_SIZE_FLOAT4,
                                            64, 2)
                };
                static const uint32_t numSkinMeshVertexElements = sizeof(skinMeshVertexElements) / sizeof(skinMeshVertexElements[0]);

                Mesh::VertexElement* referenceVertexElements = nullptr;
                if(data_.vertexElements.getSize() == numStaticMeshVertexElements)
                        referenceVertexElements = staticMeshVertexElements;
                else if(data_.vertexElements.getSize() == numSkinMeshVertexElements)
                        referenceVertexElements = skinMeshVertexElements;
                else
                        return false;

                for(register uint8_t i = 0; i < data_.vertexElements.getSize(); ++i)
                {
                        if(data_.vertexElements[i].type != referenceVertexElements[i].type ||
                           data_.vertexElements[i].size != referenceVertexElements[i].size ||
                           data_.vertexElements[i].offset != referenceVertexElements[i].offset ||
                           data_.vertexElements[i].index  != referenceVertexElements[i].index)
                                return false;
                }

                // allocate memory for vertex elements
                d3dVertexElements_ = new(std::nothrow) D3DVERTEXELEMENT9[data_.vertexElements.getSize() + 1];
                if(d3dVertexElements_ == nullptr)
                        return false;

                // fill vertex elements
                for(register uint8_t i = 0; i < data_.vertexElements.getSize(); ++i)
                {
                        d3dVertexElements_[i].UsageIndex = data_.vertexElements[i].index;
                        d3dVertexElements_[i].Offset = data_.vertexElements[i].offset;
                        d3dVertexElements_[i].Method = D3DDECLMETHOD_DEFAULT;
                        d3dVertexElements_[i].Stream = 0;

                        // set type
                        switch(data_.vertexElements[i].type)
                        {
                                case VERTEX_ELEMENT_POSITION:
                                        d3dVertexElements_[i].Usage = D3DDECLUSAGE_POSITION;
                                        break;

                                case VERTEX_ELEMENT_TEXCOORD:
                                        d3dVertexElements_[i].Usage = D3DDECLUSAGE_TEXCOORD;
                                        break;

                                case VERTEX_ELEMENT_BINORMAL:
                                        d3dVertexElements_[i].Usage = D3DDECLUSAGE_BINORMAL;
                                        break;

                                case VERTEX_ELEMENT_TANGENT:
                                        d3dVertexElements_[i].Usage = D3DDECLUSAGE_TANGENT;
                                        break;

                                case VERTEX_ELEMENT_NORMAL:
                                        d3dVertexElements_[i].Usage = D3DDECLUSAGE_NORMAL;
                                        break;

                                default:
                                        destroy();
                                        return false;
                        }

                        // set size
                        switch(data_.vertexElements[i].size)
                        {
                                case VERTEX_ELEMENT_SIZE_FLOAT1:
                                        d3dVertexElements_[i].Type = D3DDECLTYPE_FLOAT1;
                                        break;

                                case VERTEX_ELEMENT_SIZE_FLOAT2:
                                        d3dVertexElements_[i].Type = D3DDECLTYPE_FLOAT2;
                                        break;

                                case VERTEX_ELEMENT_SIZE_FLOAT3:
                                        d3dVertexElements_[i].Type = D3DDECLTYPE_FLOAT3;
                                        break;

                                case VERTEX_ELEMENT_SIZE_FLOAT4:
                                        d3dVertexElements_[i].Type = D3DDECLTYPE_FLOAT4;
                                        break;

                                default:
                                        destroy();
                                        return false;
                        }

                }

                D3DVERTEXELEMENT9 lastD3dVertexElement = D3DDECL_END();
                d3dVertexElements_[data_.vertexElements.getSize()] = lastD3dVertexElement;

                // create vertex declaration
                if(FAILED(d3dDevice->CreateVertexDeclaration(d3dVertexElements_, &d3dVertexDeclaration_)))
                {
                        destroy();
                        return false;
                }

                // compute size of vertex and index buffers
                uint32_t vertexBufferSize = data_.vertices.getSize() * data_.vertices.getStride();
                uint32_t indexBufferSize  = 3 * data_.faces.getSize() * data_.faces.getStride();

                // create vertex buffer
                if(FAILED(d3dDevice->CreateVertexBuffer(vertexBufferSize, 0, 0,
                                                        D3DPOOL_DEFAULT, &d3dVertexBuffer_, nullptr)))
                {
                        destroy();
                        return false;
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

                // fill vertex buffer
                void* destinationBuffer = nullptr;
                if(FAILED(d3dVertexBuffer_->Lock(0, vertexBufferSize, &destinationBuffer, 0)))
                {
                        destroy();
                        return false;
                }
                memcpy(destinationBuffer, &data_.vertices[0], vertexBufferSize);
                d3dVertexBuffer_->Unlock();

                // fill index buffer
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
        void D3d9Mesh::destroy()
        {
                SAFE_RELEASE(d3dVertexDeclaration_);
                SAFE_RELEASE(d3dVertexBuffer_);
                SAFE_RELEASE(d3dIndexBuffer_);
                SAFE_DELETE_ARRAY(d3dVertexElements_);
        }

}
