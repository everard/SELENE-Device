// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLESGLSLProgram.h"

#include "../../Platform.h"
#include "GLESRenderer.h"

namespace selene
{

        GlesGlslProgram::GlesGlslProgram()
        {
                program_ = 0;
        }
        GlesGlslProgram::~GlesGlslProgram()
        {
                destroy();
        }

        // Initializes program
        bool GlesGlslProgram::initialize(const char* vertexShaderSourceCode,
                                         const char* fragmentShaderSourceCode)
        {
                GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderSourceCode);
                if(vertexShader == 0)
                        return false;

                GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderSourceCode);
                if(fragmentShader == 0)
                        return false;

                program_ = glCreateProgram();
                if(program_ == 0)
                        return false;

                glAttachShader(program_, vertexShader);
                GlesRenderer::checkGlesError("glAttachShader");

                glAttachShader(program_, fragmentShader);
                GlesRenderer::checkGlesError("glAttachShader");

                glLinkProgram(program_);

                GLint isLinked = GL_FALSE;
                glGetProgramiv(program_, GL_LINK_STATUS, &isLinked);

                if(isLinked != GL_TRUE)
                {
                        GLint infoStringLength = 0;
                        glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &infoStringLength);

                        if(infoStringLength > 0)
                        {
                                char* buffer = new(std::nothrow) char[infoStringLength];
                                if(buffer != nullptr)
                                {
                                        glGetProgramInfoLog(program_, infoStringLength, NULL, buffer);
                                        LOGI("-------------------- Failed linking GLSL program: %s", buffer);
                                        SAFE_DELETE_ARRAY(buffer);
                                }
                        }

                        glDeleteProgram(program_);
                        program_ = 0;

                        return false;
                }

                return true;
        }

        // Destroys program
        void GlesGlslProgram::destroy()
        {
                if(program_ != 0)
                {
                        glDeleteProgram(program_);
                        program_ = 0;
                }
        }

        // Uses program
        bool GlesGlslProgram::use()
        {
                if(program_ == 0)
                        return false;

                glUseProgram(program_);
                return true;
        }

        // Returns attribute location
        GLint GlesGlslProgram::getAttributeLocation(const GLchar* attributeName)
        {
                if(program_ == 0)
                        return -1;

                return glGetAttribLocation(program_, attributeName);
        }

        // Returns uniform location
        GLint GlesGlslProgram::getUniformLocation(const GLchar* uniformName)
        {
                if(program_ == 0)
                        return -1;

                return glGetUniformLocation(program_, uniformName);
        }

        // Loads shader from source code
        GLuint GlesGlslProgram::loadShader(GLenum shaderType, const char* sourceCode)
        {
                GLuint shader = glCreateShader(shaderType);

                if(shader == 0)
                        return 0;

                glShaderSource(shader, 1, &sourceCode, nullptr);
                glCompileShader(shader);

                GLint isCompiled = 0;
                glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

                if(isCompiled == 0)
                {
                        GLint infoStringLength = 0;
                        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoStringLength);

                        if(infoStringLength > 0)
                        {
                                char* buffer = new(std::nothrow) char[infoStringLength];
                                if(buffer != nullptr)
                                {
                                        glGetShaderInfoLog(shader, infoStringLength, nullptr, buffer);
                                        LOGI("-------------------- Failed loading shader: %s", buffer);
                                        SAFE_DELETE_ARRAY(buffer);
                                }
                        }

                        glDeleteShader(shader);
                        shader = 0;
                }

                return shader;
        }

}
