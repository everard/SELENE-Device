// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef GLES_GLSL_PROGRAM_H
#define GLES_GLSL_PROGRAM_H

#include "../../../../../Engine/Core/Entity/Entity.h"
#include <GLES2/gl2.h>

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
                 * Represents attribute of GLSL program.
                 */
                class Attribute: public Entity
                {
                public:
                        /**
                         * \brief Constructs attribute with given name and locaiton.
                         * \param[in] name name of the attribute
                         * \param[in] location location of the attribute
                         */
                        Attribute(const char* name, GLuint location);
                        ~Attribute();

                        /**
                         * \brief Returns location of the attribute.
                         * \return location of the attribute
                         */
                        GLuint getLocation() const;

                private:
                        GLuint location_;

                };

                GlesGlslProgram();
                ~GlesGlslProgram();

                /**
                 * \brief Initializes program.
                 * \param[in] vertexShaderSourceCode source code of the vertex shader
                 * \param[in] fragmentShaderSourceCode source code of the fragment shader
                 * \param[in] useVertexShaderLibrary flag, which shows the need to use vertex shader library
                 * \param[in] useFragmentShaderLibrary flag, which shows the need to use fragment shader library
                 * \return true if shaders has been successfully compiled and program linked
                 */
                bool initialize(const char* vertexShaderSourceCode,
                                const char* fragmentShaderSourceCode,
                                const Attribute* attributes = nullptr,
                                uint8_t numAttributes = 0,
                                bool useVertexShaderLibrary = false,
                                bool useFragmentShaderLibrary = false);

                /**
                 * \brief Destroys program.
                 */
                void destroy();

                /**
                 * \brief Uses program.
                 * \return true if program has been successfully used
                 */
                bool use();

                /**
                 * \brief Returns attribute location.
                 * \param[in] attributeName name of the attribute
                 * \return location of the attribute, -1 if attribute with given name does not exist
                 */
                GLint getAttributeLocation(const GLchar* attributeName);

                /**
                 * \brief Returns uniform location.
                 * \param[in] uniformName name of the uniform
                 * \return location of the uniform, -1 if uniform with given name does not exist
                 */
                GLint getUniformLocation(const GLchar* uniformName);

                /**
                 * \brief Loads shader from source code.
                 * \param[in] shaderType type of the shader (vertex or fragment shader)
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
