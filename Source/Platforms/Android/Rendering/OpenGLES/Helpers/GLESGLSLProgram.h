// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GLES_GLSL_PROGRAM_H
#define GLES_GLSL_PROGRAM_H

#include "../../../../../Engine/Core/Entity/Entity.h"
#include <GLES2/gl2.h>
#include <string>

namespace selene
{

        /**
         * \addtogroup Android
         * @{
         */

        /**
         * Represents OpenGL ES GLSL program.
         */
        class GlesGlslProgram
        {
        public:
                /**
                 * Represents vertex attribute of GLSL program.
                 */
                class VertexAttribute
                {
                public:
                        /**
                         * \brief Constructs vertex attribute with given name and locaiton.
                         * \param[in] name name of the attribute
                         * \param[in] location location of the attribute
                         */
                        VertexAttribute(const char* name, GLuint location);
                        VertexAttribute(const VertexAttribute&) = default;
                        ~VertexAttribute();
                        VertexAttribute& operator =(const VertexAttribute&) = default;

                        /**
                         * \brief Returns location of the vertex attribute in GLSL program.
                         * \return location of the attribute
                         */
                        GLuint getLocation() const;

                        /**
                         * \brief Returns name of the vertex attribute.
                         * \return name of the vertex attribute
                         */
                        const char* getName() const;

                private:
                        std::string name_;
                        GLuint location_;

                };

                GlesGlslProgram();
                GlesGlslProgram(const GlesGlslProgram&) = delete;
                ~GlesGlslProgram();
                GlesGlslProgram& operator =(const GlesGlslProgram&) = delete;

                /**
                 * \brief Initializes program.
                 * \param[in] vertexShaderSourceCode source code of the vertex shader
                 * \param[in] fragmentShaderSourceCode source code of the fragment shader
                 * \param[in] vertexAttributes array of vertex attributes, which specifies their locations
                 * in GLSL program
                 * \param[in] numVertexAttributes number of vertex attributes
                 * \param[in] useVertexShaderLibrary flag, which shows the need to use vertex shader library
                 * \param[in] useFragmentShaderLibrary flag, which shows the need to use fragment shader library
                 * \return true if shaders has been successfully compiled and program linked
                 */
                bool initialize(const char* vertexShaderSourceCode,
                                const char* fragmentShaderSourceCode,
                                const VertexAttribute* vertexAttributes = nullptr,
                                uint8_t numVertexAttributes = 0,
                                bool useVertexShaderLibrary = false,
                                bool useFragmentShaderLibrary = false);

                /**
                 * \brief Destroys program.
                 */
                void destroy();

                /**
                 * \brief Sets GLSL program.
                 * \return true if program has been successfully set
                 */
                bool set();

                /**
                 * \brief Returns location of the attribute with given name.
                 * \param[in] attributeName name of the attribute
                 * \return location of the attribute, -1 if attribute with given name does not exist
                 */
                GLint getAttributeLocation(const GLchar* attributeName);

                /**
                 * \brief Returns location of the uniform with given name.
                 * \param[in] uniformName name of the uniform
                 * \return location of the uniform, -1 if uniform with given name does not exist
                 */
                GLint getUniformLocation(const GLchar* uniformName);

                /**
                 * \brief Loads shader from source code.
                 * \param[in] shaderType type of the shader (may be one of the following:
                 * GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
                 * \param[in] sourceCode source code of the shader
                 * \return ID of the loaded shader, -1 if shader could not be loaded
                 */
                static GLuint loadShader(GLenum shaderType, const char* sourceCode);

        private:
                GLint program_;

        };

        /**
         * @}
         */

}

#endif
