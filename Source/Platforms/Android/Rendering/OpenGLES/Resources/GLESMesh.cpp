// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLESMesh.h"

#include "../../../Platform.h"
#include "../GLESRenderer.h"

namespace selene
{

        GlesMesh::GlesMesh(const char* name): Mesh(name)
        {
                memset(vertexBuffers_, 0, sizeof(vertexBuffers_));
                indexBuffer_ = 0;
        }
        GlesMesh::~GlesMesh()
        {
                destroy();
        }

        //----------------------
        bool GlesMesh::retain()
        {
                destroy();

                LOGI("----------------------- Retaining Mesh '%s'", getName());

                // OpenGL ES does not allow 4-byte indices
                if(data_.faces.getStride() == 4)
                        return false;

                // compute sizes of the buffers
                uint32_t vertexBufferSizes[NUM_OF_VERTEX_STREAMS];
                for(uint8_t i = 0; i < NUM_OF_VERTEX_STREAMS; ++i)
                        vertexBufferSizes[i] = data_.vertices[i].getSize() * data_.vertices[i].getStride();

                uint32_t indexBufferSize = 3 * data_.faces.getSize() * data_.faces.getStride();

                // create vertex buffer objects
                if(hasSkeleton())
                        glGenBuffers(NUM_OF_VERTEX_STREAMS, vertexBuffers_);
                else
                        glGenBuffers(NUM_OF_VERTEX_STREAMS - 1, vertexBuffers_);

                GlesRenderer::checkGlesError("glGenBuffers");

                for(uint8_t i = VERTEX_STREAM_POSITIONS; i <= VERTEX_STREAM_TEXTURE_COORDINATES; ++i)
                {
                        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers_[i]);
                        GlesRenderer::checkGlesError("glBindBuffer");

                        glBufferData(GL_ARRAY_BUFFER, vertexBufferSizes[i], &data_.vertices[i][0], GL_STATIC_DRAW);
                        GlesRenderer::checkGlesError("glBufferData");
                }

                if(hasSkeleton())
                {
                        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers_[VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS]);
                        GlesRenderer::checkGlesError("glGenBuffers");

                        glBufferData(GL_ARRAY_BUFFER, vertexBufferSizes[VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS],
                                     &data_.vertices[VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS][0], GL_STATIC_DRAW);
                        GlesRenderer::checkGlesError("glBufferData");
                }

                glBindBuffer(GL_ARRAY_BUFFER, 0);

                // create index buffer
                glGenBuffers(1, &indexBuffer_);
                GlesRenderer::checkGlesError("glGenBuffers");

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);
                GlesRenderer::checkGlesError("glBindBuffer");

                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, &data_.faces[0], GL_STATIC_DRAW);
                GlesRenderer::checkGlesError("glBufferData");

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

                LOGI("----------------------- Retained Mesh '%s'", getName());

                return true;
        }

        //----------------------
        void GlesMesh::discard()
        {
                destroy();
        }

        //----------------------
        void GlesMesh::destroy()
        {
                for(uint8_t i = 0; i < NUM_OF_VERTEX_STREAMS; ++i)
                {
                        if(vertexBuffers_[i] != 0)
                                glDeleteBuffers(1, &vertexBuffers_[i]);
                }

                if(indexBuffer_ != 0)
                        glDeleteBuffers(1, &indexBuffer_);

                memset(vertexBuffers_, 0, sizeof(vertexBuffers_));
                indexBuffer_ = 0;
        }

}
