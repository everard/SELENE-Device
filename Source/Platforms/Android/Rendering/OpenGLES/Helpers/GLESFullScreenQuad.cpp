// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLESFullScreenQuad.h"

#include "../../../Platform.h"

namespace selene
{

        GlesFullScreenQuad::GlesFullScreenQuad(): vertexBuffer_(0) {}
        GlesFullScreenQuad::~GlesFullScreenQuad()
        {
                destroy();
        }

        //---------------------------------------------------------------------------------
        bool GlesFullScreenQuad::initialize()
        {
                destroy();

                static Vector4d vertices[] =
                {
                        Vector4d(-1.0f, -1.0f, 0.0f, 0.0f),
                        Vector4d(-1.0f,  1.0f, 0.0f, 1.0f),
                        Vector4d( 1.0f, -1.0f, 1.0f, 0.0f),
                        Vector4d( 1.0f,  1.0f, 1.0f, 1.0f)
                };
                const uint32_t vertexBufferSize = sizeof(vertices);

                glGenBuffers(1, &vertexBuffer_);

                glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
                CHECK_GLES_ERROR("GlesFullScreenQuad::initialize: glBindBuffer");

                glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, vertices, GL_STATIC_DRAW);
                CHECK_GLES_ERROR("GlesFullScreenQuad::initialize: glBufferData");

                glBindBuffer(GL_ARRAY_BUFFER, 0);

                return true;
        }

        //---------------------------------------------------------------------------------
        void GlesFullScreenQuad::destroy()
        {
                if(vertexBuffer_ != 0)
                {
                        glDeleteBuffers(1, &vertexBuffer_);
                        vertexBuffer_ = 0;
                }
        }

        //---------------------------------------------------------------------------------
        void GlesFullScreenQuad::render()
        {
                if(vertexBuffer_ == 0)
                        return;

                glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
                CHECK_GLES_ERROR("GlesFullScreenQuad::render: glBindBuffer");

                glEnableVertexAttribArray(0);
                CHECK_GLES_ERROR("GlesFullScreenQuad::render: glEnableVertexAttribArray");

                glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vector4d), nullptr);
                CHECK_GLES_ERROR("GlesFullScreenQuad::render: glVertexAttribPointer");

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                CHECK_GLES_ERROR("GlesFullScreenQuad::render: glBindBuffer");

                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                CHECK_GLES_ERROR("GlesFullScreenQuad::render: glDrawArrays");
        }

}
