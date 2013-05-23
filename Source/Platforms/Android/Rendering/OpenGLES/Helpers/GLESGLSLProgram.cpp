// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLESGLSLProgram.h"

#include "../../../Platform.h"
#include "../GLESRenderer.h"

#include <new>

namespace selene
{

        GlesGlslProgram::VertexAttribute::VertexAttribute(const char* name, GLuint location): Entity(name)
        {
                location_ = location;
        }
        GlesGlslProgram::VertexAttribute::~VertexAttribute() {}

        //-------------------------------------------------------------------------------------------------------------
        GLuint GlesGlslProgram::VertexAttribute::getLocation() const
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

        //-------------------------------------------------------------------------------------------------------------
        bool GlesGlslProgram::initialize(const char* vertexShaderSourceCode,
                                         const char* fragmentShaderSourceCode,
                                         const VertexAttribute* vertexAttributes,
                                         uint8_t numVertexAttributes,
                                         bool useVertexShaderLibrary,
                                         bool useFragmentShaderLibrary)
        {
                static const char* vertexShaderLibrary =
                        "vec4 multiplyQuaternions(mediump vec4 quaternion0, vec4 quaternion1)\n"
                        "{\n"
                        "        vec3 im = quaternion0.w * quaternion1.xyz + quaternion0.xyz *\n"
                        "                  quaternion1.w + cross(quaternion0.xyz, quaternion1.xyz);\n"
                        "        vec4 dt = quaternion0 * quaternion1;\n"
                        "        float re = dot(dt, vec4(-1.0, -1.0, -1.0, 1.0));\n"
                        "        return vec4(im, re);\n"
                        "}\n"
                        "vec4 rotateVector(vec3 vector, vec4 quaternion)\n"
                        "{\n"
                        "        vec4 temp = multiplyQuaternions(quaternion, vec4(vector, 0.0));\n"
                        "        return multiplyQuaternions(temp, vec4(-quaternion.x, -quaternion.y,\n"
                        "                                              -quaternion.z, quaternion.w));\n"
                        "}\n"
                        "vec3 transformPosition(vec4 offset, vec4 rotation, vec3 position)\n"
                        "{\n"
                        "        return offset.xyz + rotateVector(position, rotation).xyz;\n"
                        "}\n";

                static const char* fragmentShaderLibrary =
                        "precision highp float;\n"
                        "precision highp sampler2D;\n"
                        /*"vec4 encodePosition(vec4 position, vec4 projectionParameters)\n"
                        "{\n"
                        "        vec4 enc = vec4(16777216.0, 65536.0, 256.0, 1.0) *\n"
                        "                   (position.z / projectionParameters.w);\n"
                        "        enc  = fract(enc);\n"
                        "        enc -= enc.xxyz * vec4(0.0, 1.0 / 256.0, 1.0 / 256.0, 1.0 / 256.0);\n"
                        "        return enc;\n"
                        "}\n"
                        "float decodeEyeZ(vec4 encodedEyeZ, vec4 projectionParameters)\n"
                        "{\n"
                        "        return dot(encodedEyeZ, vec4(1.0 / (256.0 * 256.0 * 256.0),\n"
                        "                                     1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0)) *\n"
                        "                                     projectionParameters.w;\n"
                        "}\n"*/
                        "float convertDepthToEyeZ(float depth, vec4 conversionParameters)\n"
                        "{\n"
                        "        return conversionParameters.x / (depth * conversionParameters.y +\n"
                        "                                                 conversionParameters.z);\n"
                        "}\n"
                        /*"vec3 decodePosition(vec4 encodedEyeZ, vec2 position, vec4 projectionParameters,\n"
                        "                      vec4 unprojectionVector)\n"
                        "{\n"
                        "        float zv = decodeEyeZ(encodedEyeZ, projectionParameters);\n"
                        "        vec4 p = vec4(position, 1.0, 0.0);\n"
                        "        vec4 orig = p * unprojectionVector * vec4(zv, zv, zv, zv);\n"
                        "        return orig.xyz;\n"
                        "}\n"*/
                        "vec3 decodePosition(vec2 position, float depth, vec4 conversionParameters,\n"
                        "                    vec4 unprojectionVector)\n"
                        //"vec3 decodePosition(vec4 position, mat4 projectionInvMatrix)\n"
                        "{\n"
                        "        float zv = convertDepthToEyeZ(depth, conversionParameters);\n"
                        "        vec4 p = vec4(position, 1.0, 0.0);\n"
                        "        vec4 orig = p * unprojectionVector * vec4(zv, zv, zv, zv);\n"
                        "        return orig.xyz;\n"
                        /*"        vec4 result = projectionInvMatrix * position;\n"
                        "        result.xyz /= result.w;\n"
                        "        return result.xyz;\n"*/
                        "}\n"
                        "vec3 encodeNormal(vec3 normal)\n"
                        "{\n"
                        "        return 0.5 * normal + vec3(0.5, 0.5, 0.5);\n"
                        "}\n"
                        "vec3 decodeNormal(vec3 encodedNormal)\n"
                        "{\n"
                        "        return 2.0 * encodedNormal - vec3(1.0, 1.0, 1.0);\n"
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
                        fragmentShader = loadShader(GL_FRAGMENT_SHADER, sourceCode.c_str());
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

                if(vertexAttributes != nullptr)
                {
                        for(uint32_t i = 0; i < numVertexAttributes; ++i)
                        {
                                glBindAttribLocation(program_, vertexAttributes[i].getLocation(),
                                                     vertexAttributes[i].getName());
                                CHECK_GLES_ERROR("GlesGlslProgram::initialize: glBindAttribLocation");
                        }
                }

                glAttachShader(program_, vertexShader);
                CHECK_GLES_ERROR("GlesGlslProgram::initialize: glAttachShader");

                glAttachShader(program_, fragmentShader);
                CHECK_GLES_ERROR("GlesGlslProgram::initialize: glAttachShader");

                glLinkProgram(program_);
                CHECK_GLES_ERROR("GlesGlslProgram::initialize: glLinkProgram");

                GLint isLinked = GL_FALSE;
                glGetProgramiv(program_, GL_LINK_STATUS, &isLinked);

                if(isLinked != GL_TRUE)
                {
                        GLint infoStringLength = 0;
                        glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &infoStringLength);
                        CHECK_GLES_ERROR("GlesGlslProgram::initialize: glGetProgramiv");

                        if(infoStringLength > 0)
                        {
                                char* buffer = new(std::nothrow) char[infoStringLength];
                                if(buffer != nullptr)
                                {
                                        glGetProgramInfoLog(program_, infoStringLength, nullptr, buffer);
                                        CHECK_GLES_ERROR("GlesGlslProgram::initialize: glGetProgramInfoLog");
                                        LOGI("****************************** Failed linking GLSL program: %s", buffer);
                                        SAFE_DELETE_ARRAY(buffer);
                                }
                        }

                        glDeleteProgram(program_);
                        CHECK_GLES_ERROR("GlesGlslProgram::initialize: glDeleteProgram");

                        program_ = 0;

                        return false;
                }

                return true;
        }

        //-------------------------------------------------------------------------------------------------------------
        void GlesGlslProgram::destroy()
        {
                if(program_ != 0)
                {
                        glDeleteProgram(program_);
                        program_ = 0;
                }
        }

        //-------------------------------------------------------------------------------------------------------------
        bool GlesGlslProgram::set()
        {
                if(program_ == 0)
                        return false;

                glUseProgram(program_);
                return true;
        }

        //-------------------------------------------------------------------------------------------------------------
        GLint GlesGlslProgram::getAttributeLocation(const GLchar* attributeName)
        {
                if(program_ == 0)
                        return -1;

                return glGetAttribLocation(program_, attributeName);
        }

        //-------------------------------------------------------------------------------------------------------------
        GLint GlesGlslProgram::getUniformLocation(const GLchar* uniformName)
        {
                if(program_ == 0)
                        return -1;

                return glGetUniformLocation(program_, uniformName);
        }

        //-------------------------------------------------------------------------------------------------------------
        GLuint GlesGlslProgram::loadShader(GLenum shaderType, const char* sourceCode)
        {
                GLuint shader = glCreateShader(shaderType);

                if(shader == 0)
                        return 0;

                glShaderSource(shader, 1, &sourceCode, nullptr);
                CHECK_GLES_ERROR("GlesGlslProgram::loadShader: glShaderSource");

                glCompileShader(shader);
                CHECK_GLES_ERROR("GlesGlslProgram::loadShader: glCompileShader");

                GLint isCompiled = 0;
                glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
                CHECK_GLES_ERROR("GlesGlslProgram::loadShader: glGetShaderiv");

                if(isCompiled == 0)
                {
                        GLint infoStringLength = 0;
                        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoStringLength);
                        CHECK_GLES_ERROR("GlesGlslProgram::loadShader: glGetShaderiv");

                        if(infoStringLength > 0)
                        {
                                char* buffer = new(std::nothrow) char[infoStringLength];
                                if(buffer != nullptr)
                                {
                                        glGetShaderInfoLog(shader, infoStringLength, nullptr, buffer);
                                        CHECK_GLES_ERROR("GlesGlslProgram::loadShader: glGetShaderInfoLog");
                                        LOGI("****************************** Failed loading shader: %s", buffer);
                                        SAFE_DELETE_ARRAY(buffer);
                                }
                        }

                        glDeleteShader(shader);
                        CHECK_GLES_ERROR("GlesGlslProgram::loadShader: glDeleteShader");

                        shader = 0;
                }

                return shader;
        }

}
