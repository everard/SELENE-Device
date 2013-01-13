// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLESRenderer.h"

#include "../../../../Engine/Scene/Nodes/ParticleSystem.h"
#include "../../../../Engine/Scene/Nodes/Camera.h"
#include "../../../../Engine/Scene/Nodes/Actor.h"
#include "../../Application/AndroidApplication.h"
#include "../../../../Engine/GUI/GUI.h"

#include "Resources/GLESTexture.h"
#include "Resources/GLESMesh.h"

#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "SELENE-Device", __VA_ARGS__))

namespace selene
{

        //--------------------------------------------------------------------------------------------
        static void checkGlError(const char* operation)
        {
                for(GLint error = glGetError(); error; error = glGetError())
                {
                        LOGI("after %s() glError (0x%x)\n", operation, error);
                }
        }

        //--------------------------------------------------------------------------------------------
        GLuint loadShader(GLenum shaderType, const char* sourceCode)
        {
                GLuint shader = glCreateShader(shaderType);
                if(shader != 0)
                {
                        glShaderSource(shader, 1, &sourceCode, NULL);
                        glCompileShader(shader);

                        GLint compiled = 0;
                        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

                        if(compiled == 0)
                        {
                                GLint infoLen = 0;
                                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
                                if(infoLen > 0)
                                {
                                        char* buf = new(std::nothrow) char[infoLen];
                                        if(buf != nullptr)
                                        {
                                                glGetShaderInfoLog(shader, infoLen, NULL, buf);
                                                LOGI("Could not compile shader %d:\n%s\n", shaderType, buf);
                                                SAFE_DELETE_ARRAY(buf);
                                        }

                                        glDeleteShader(shader);
                                        shader = 0;
                                }
                        }
                }

                return shader;
        }

        //--------------------------------------------------------------------------------------------
        GLuint createProgram(const char* vertexShaderSourceCode, const char* fragmentShaderSourceCode)
        {
                GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderSourceCode);
                if(vertexShader == 0)
                {
                        return 0;
                }

                GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderSourceCode);
                if(pixelShader == 0)
                {
                        return 0;
                }

                GLuint program = glCreateProgram();
                if(program != 0)
                {
                        glAttachShader(program, vertexShader);
                        checkGlError("glAttachShader");

                        glAttachShader(program, pixelShader);
                        checkGlError("glAttachShader");

                        glLinkProgram(program);
                        GLint linkStatus = GL_FALSE;
                        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

                        if(linkStatus != GL_TRUE)
                        {
                                GLint bufLength = 0;
                                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
                                if(bufLength > 0)
                                {
                                        char* buf = new(std::nothrow) char[bufLength];
                                        if(buf != nullptr)
                                        {
                                                glGetProgramInfoLog(program, bufLength, NULL, buf);
                                                LOGI("Could not link program:\n%s\n", buf);
                                                SAFE_DELETE_ARRAY(buf);
                                        }
                                }

                                glDeleteProgram(program);
                                program = 0;
                        }
                }

                return program;
        }

        //--------------------------------------------------------------------------------------------
        bool GlesRenderer::initialize(const Renderer::Parameters& parameters)
        {
                destroy();

                parameters_ = parameters;

                writeLogEntry("--- Initializing OpenGL ES renderer ---");

                if(!initializeContext())
                {
                        destroy();
                        writeLogEntry("ERROR: Context initialization failed.");
                        return false;
                }

                // check capabilities
                GLint maxNumVertexAttributes = 0;
                glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxNumVertexAttributes);
                if(maxNumVertexAttributes < NUM_OF_VERTEX_ATTRIBUTES_REQUIRED)
                {
                        destroy();
                        writeLogEntry("ERROR: Not enough vertex attributes.");
                        return false;
                }

                // load shaders, etc.
                static const char vertexShader[] =
                        "attribute vec4 vPosition;"
                        "void main()"
                        "{"
                        "        gl_Position = vec4(vPosition.x - 1.0, 1.0 - vPosition.y, 0.0, 1.0);"
                        "}\n";

                static const char fragmentShader[] =
                        "precision mediump float;"
                        "uniform vec4 color;"
                        "void main()"
                        "{"
                        "        gl_FragColor = color;"
                        "}\n";

                program_ = createProgram(vertexShader, fragmentShader);
                if(program_ == 0)
                {
                        writeLogEntry("ERROR: Could not create GLSL program.");
                        return false;
                }

                positionHandle_ = glGetAttribLocation(program_, "vPosition");
                checkGlError("glGetAttribLocation");

                colorLocation_ = glGetUniformLocation(program_, "color");

                // prepare OpenGL ES
                glEnable(GL_CULL_FACE);
                glEnable(GL_DEPTH_TEST);

                return true;
        }

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::destroy()
        {
                if(display_ != EGL_NO_DISPLAY)
                {
                        eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

                        if(context_ != EGL_NO_CONTEXT)
                        {
                                eglDestroyContext(display_, context_);
                                context_ = EGL_NO_CONTEXT;
                        }

                        if(surface_ != EGL_NO_SURFACE)
                        {
                                eglDestroySurface(display_, surface_);
                                surface_ = EGL_NO_SURFACE;
                        }

                        eglTerminate(display_);
                        display_ = EGL_NO_DISPLAY;
                }
        }

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::render(const Camera& camera)
        {
                // get matrices
                viewProjectionMatrix_ = camera.getViewProjectionMatrix();
                projectionMatrix_ = camera.getProjectionMatrix();
                viewMatrix_ = camera.getViewMatrix();

                normalsMatrix_ = viewMatrix_;
                normalsMatrix_.invert();
                viewInvMatrix_ = normalsMatrix_;
                normalsMatrix_.transpose();

                // get vectors
                const Matrix& projectionInvMatrix = camera.getProjectionInvMatrix();
                projectionParameters_ = camera.getProjectionParameters();
                unprojectionVector_.define(projectionInvMatrix.a[0][0],
                                           projectionInvMatrix.a[1][1],
                                           1.0, 0.0);

                glViewport(0, 0, parameters_.getWidth(), parameters_.getHeight());

                glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glUseProgram(program_);
                checkGlError("glUseProgram");

                glEnableVertexAttribArray(positionHandle_);
                checkGlError("glEnableVertexAttribArray");

                // render GUI
                Gui* gui = camera.getGui();
                if(gui != nullptr)
                {
                        const auto& elements = gui->getElements();

                        for(auto it = elements.begin(); it != elements.end(); ++it)
                        {
                                if(!it->second)
                                        continue;

                                auto element = it->second.get();
                                if(element->is(GUI_ELEMENT_HIDDEN))
                                        continue;

                                Vector4d color = element->getBackgroundColor(GUI_ELEMENT_COLOR_DEFAULT);

                                if(element->is(GUI_ELEMENT_SELECTED))
                                        color = element->getBackgroundColor(GUI_ELEMENT_COLOR_SELECTED);
                                else if(element->is(GUI_ELEMENT_TOUCHED))
                                        color = element->getBackgroundColor(GUI_ELEMENT_COLOR_TOUCHED);

                                const auto& size = element->getSize();
                                const auto& position = element->getPosition();

                                static Vector2d triangleVertices[] =
                                {
                                        Vector2d(),
                                        Vector2d(),
                                        Vector2d(),

                                        Vector2d(),
                                        Vector2d(),
                                        Vector2d()
                                };
                                static Vector2d vertexOffsets[] =
                                {
                                        Vector2d(),
                                        Vector2d(),
                                        Vector2d(),
                                        Vector2d()
                                };
                                const uint32_t numTriangleVertices = sizeof(triangleVertices) / sizeof(triangleVertices[0]);
                                uint32_t vertexIndices[numTriangleVertices] = {0, 2, 1, 2, 3, 1};

                                vertexOffsets[1] = Vector2d(size.x, 0.0f);
                                vertexOffsets[2] = Vector2d(0.0f, size.y);
                                vertexOffsets[3] = Vector2d(size.x, size.y);

                                for(uint32_t i = 0; i < numTriangleVertices; ++i)
                                        triangleVertices[i] = position + vertexOffsets[vertexIndices[i]];

                                glUniform4fv(colorLocation_, 1, static_cast<const float*>(color));

                                glVertexAttribPointer(positionHandle_, 2, GL_FLOAT, GL_FALSE, 0, triangleVertices);
                                checkGlError("glVertexAttribPointer");

                                glDrawArrays(GL_TRIANGLES, 0, 6);
                                checkGlError("glDrawArrays");
                        }
                }

                eglSwapBuffers(display_, surface_);
        }

        GlesRenderer::GlesRenderer(): parameters_(nullptr, nullptr, 0, 0, nullptr, 0)
        {
                surface_ = EGL_NO_SURFACE;
                context_ = EGL_NO_CONTEXT;
                display_ = EGL_NO_DISPLAY;

                program_ = positionHandle_ = colorLocation_ = 0;
        }
        GlesRenderer::GlesRenderer(const GlesRenderer&): parameters_(nullptr, nullptr, 0, 0, nullptr, 0)
        {
                surface_ = EGL_NO_SURFACE;
                context_ = EGL_NO_CONTEXT;
                display_ = EGL_NO_DISPLAY;

                program_ = positionHandle_ = colorLocation_ = 0;
        }
        GlesRenderer::~GlesRenderer()
        {
                destroy();
        }

        //--------------------------------------------------------------------------------------------
        bool GlesRenderer::createContext()
        {
                if(!initialize(parameters_))
                        return false;

                if(!ResourceManager::retainResources())
                        return false;

                return true;
        }

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::discardContext()
        {
                ResourceManager::discardResources();
                destroy();
        }

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::writeLogEntry(const char* entry)
        {
                LOGI("%s", entry);
                if(parameters_.getLog() != nullptr)
                        (*parameters_.getLog()) << entry << std::endl;
        }

        //--------------------------------------------------------------------------------------------
        bool GlesRenderer::initializeContext()
        {
                AndroidApplication* application = dynamic_cast<AndroidApplication*>(parameters_.getApplication());
                if(application == nullptr)
                        return false;

                android_app* state = application->getHandle();
                if(state == nullptr)
                        return false;

                const EGLint attribs[] =
                {
                        EGL_BLUE_SIZE, 5,
                        EGL_GREEN_SIZE, 6,
                        EGL_RED_SIZE, 5,
                        EGL_DEPTH_SIZE, 24,
                        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                        EGL_NONE
                };

                EGLConfig config;
                EGLint numConfigs;

                display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
                if(display_ == EGL_NO_DISPLAY)
                        return false;

                if(!eglInitialize(display_, 0, 0))
                        return false;

                if(!eglChooseConfig(display_, attribs, &config, 1, &numConfigs))
                        return false;

                EGLint format;
                if(!eglGetConfigAttrib(display_, config, EGL_NATIVE_VISUAL_ID, &format))
                        return false;

                ANativeWindow_setBuffersGeometry(state->window, 0, 0, format);

                surface_ = eglCreateWindowSurface(display_, config, state->window, nullptr);
                if(!surface_)
                        return false;

                const EGLint attribsList[] =
                {
                        EGL_CONTEXT_CLIENT_VERSION, 2,
                        EGL_NONE
                };

                context_ = eglCreateContext(display_, config, nullptr, attribsList);
                if(!context_)
                        return false;

                if(eglMakeCurrent(display_, surface_, surface_, context_) == EGL_FALSE)
                        return false;

                return true;
        }

}
