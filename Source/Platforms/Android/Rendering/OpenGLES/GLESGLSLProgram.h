// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GLES_GLSL_PROGRAM_H
#define GLES_GLSL_PROGRAM_H

#include <GLES2/gl2.h>

namespace selene
{

        // Represents OpenGL ES GLSL program.
        class GlesGlslProgram
        {
        public:
                GlesGlslProgram();
                ~GlesGlslProgram();

                // Initializes program
                bool initialize(const char* vertexShaderSourceCode,
                                const char* fragmentShaderSourceCode);

                // Destroys program
                void destroy();

                // Uses program
                bool use();

                // Returns attribute location
                GLint getAttributeLocation(const GLchar* attributeName);

                // Returns uniform location
                GLint getUniformLocation(const GLchar* uniformName);

                // Loads shader from source code
                static GLuint loadShader(GLenum shaderType, const char* sourceCode);

        private:
                GLint program_;

        };

}

#endif
