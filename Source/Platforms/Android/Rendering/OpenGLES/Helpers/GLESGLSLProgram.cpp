// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLESGLSLProgram.h"

#include "../../../Platform.h"
#include "../GLESRenderer.h"

#include <new>

namespace selene
{

        GlesGlslProgram::Attribute::Attribute(const char* name, GLuint location): Entity(name)
        {
                location_ = location;
        }
        GlesGlslProgram::Attribute::~Attribute() {}

        //---------------------------------------------------------------------------
        GLuint GlesGlslProgram::Attribute::getLocation() const
        {
                return location_;
        }

        GlesGlslProgram::GlesGlslProgram()
        {
                program_ = 0;
        }
        GlesGlslProgram::~GlesGlslProgram()
        {
                destroy();
        }

        //---------------------------------------------------------------------------
        bool GlesGlslProgram::initialize(const char* vertexShaderSourceCode,
                                         const char* fragmentShaderSourceCode,
                                         const Attribute* attributes,
                                         uint8_t numAttributes,
                                         bool useVertexShaderLibrary,
                                         bool useFragmentShaderLibrary)
        {
                static const char* vertexShaderLibrary =
                        "vec4 QuatMul(vec4 q1, vec4 q2)\n"
                        "{"
                        "        vec3 im = q1.w * q2.xyz + q1.xyz * q2.w + cross(q1.xyz, q2.xyz);\n"
                        "        vec4 dt = q1 * q2;\n"
                        "        float re = dot(dt, vec4(-1.0, -1.0, -1.0, 1.0));\n"
                        "        return vec4(im, re);\n"
                        "}\n"
                        "vec4 QuatRotate(vec3 p, vec4 q)\n"
                        "{\n"
                        "        vec4 temp = QuatMul(q, vec4(p, 0.0));\n"
                        "        return QuatMul(temp, vec4(-q.x, -q.y, -q.z, q.w));\n"
                        "}\n"
                        "vec3 Transform(vec4 offset, vec4 rot, vec3 pos)\n"
                        "{\n"
                        "        return offset.xyz + QuatRotate(pos, rot).xyz;\n"
                        "}\n";

                static const char* fragmentShaderLibrary =
                        "vec4 EncodePos(vec4 pos, vec4 proj)\n"
                        "{\n"
                        "        float4 enc = vec4(16777216.0, 65536.0, 256.0, 1.0) * (pos.z / proj.w);\n"
                        "        enc  = frac(enc);\n"
                        "        enc -= enc.xxyz * vec4(0.0, 1.0 / 256.0, 1.0 / 256.0, 1.0 / 256.0);\n"
                        "        return enc;\n"
                        "}"
                        "float DecodeZEye(vec4 ZEnc, vec4 proj)\n"
                        "{\n"
                        "        return dot(ZEnc, float4(1.0 / (256.0 * 256.0 * 256.0), 1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0)) * proj.w;\n"
                        "}\n"
                        "vec3 DecodePos(vec4 ZEnc, vec2 XY, vec4 proj, vec4 unproj)\n"
                        "{\n"
                        "        float zv = DecodeZEye(ZEnc, proj);\n"
                        "        float4 p = float4(XY, 1.0, 0.0);\n"
                        "        float4 orig = p * unproj * zv.xxxx;\n"
                        "        return orig.xyz;\n"
                        "}\n"
                        "vec3 EncodeNormal(vec3 n)\n"
                        "{\n"
                        "        return 0.5 * n + vec3(0.5, 0.5, 0.5);\n"
                        "}\n"
                        "vec3 DecodeNormal(vec3 enc)\n"
                        "{\n"
                        "        return 2.0 * enc - vec3(1.0, 1.0, 1.0);\n"
                        "}\n";

                // compile vertex shader
                GLuint vertexShader = 0;
                if(useVertexShaderLibrary)
                {
                        std::string sourceCode = vertexShaderLibrary;
                        sourceCode += vertexShaderSourceCode;
                        vertexShader = loadShader(GL_VERTEX_SHADER, sourceCode.c_str());
                }
                else
                {
                        vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderSourceCode);
                }

                if(vertexShader == 0)
                        return false;

                // compile fragment shader
                GLuint fragmentShader = 0;
                if(useFragmentShaderLibrary)
                {
                        std::string sourceCode = fragmentShaderLibrary;
                        sourceCode += fragmentShaderSourceCode;
                        fragmentShader = vertexShader = loadShader(GL_FRAGMENT_SHADER, sourceCode.c_str());
                }
                else
                {
                        fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderSourceCode);
                }

                if(fragmentShader == 0)
                        return false;

                program_ = glCreateProgram();
                if(program_ == 0)
                        return false;

                if(attributes != nullptr)
                {
                        LOGI("****************************** GLSL program has %i attribute locations specified", numAttributes);

                        for(uint32_t i = 0; i < numAttributes; ++i)
                        {
                                glBindAttribLocation(program_, attributes[i].getLocation(), attributes[i].getName());
                                CHECK_GLES_ERROR("GLSL Program: initialize: glBindAttribLocation");
                        }
                }

                glAttachShader(program_, vertexShader);
                CHECK_GLES_ERROR("GLSL Program: initialize: glAttachShader");

                glAttachShader(program_, fragmentShader);
                CHECK_GLES_ERROR("GLSL Program: initialize: glAttachShader");

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
                                        LOGI("****************************** Failed linking GLSL program: %s", buffer);
                                        SAFE_DELETE_ARRAY(buffer);
                                }
                        }

                        glDeleteProgram(program_);
                        program_ = 0;

                        return false;
                }

                return true;
        }

        //---------------------------------------------------------------------------
        void GlesGlslProgram::destroy()
        {
                if(program_ != 0)
                {
                        glDeleteProgram(program_);
                        program_ = 0;
                }
        }

        //---------------------------------------------------------------------------
        bool GlesGlslProgram::use()
        {
                if(program_ == 0)
                        return false;

                glUseProgram(program_);
                return true;
        }

        //---------------------------------------------------------------------------
        GLint GlesGlslProgram::getAttributeLocation(const GLchar* attributeName)
        {
                if(program_ == 0)
                        return -1;

                return glGetAttribLocation(program_, attributeName);
        }

        //---------------------------------------------------------------------------
        GLint GlesGlslProgram::getUniformLocation(const GLchar* uniformName)
        {
                if(program_ == 0)
                        return -1;

                return glGetUniformLocation(program_, uniformName);
        }

        //---------------------------------------------------------------------------
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
                                        LOGI("****************************** Failed loading shader: %s", buffer);
                                        SAFE_DELETE_ARRAY(buffer);
                                }
                        }

                        glDeleteShader(shader);
                        shader = 0;
                }

                return shader;
        }

}
