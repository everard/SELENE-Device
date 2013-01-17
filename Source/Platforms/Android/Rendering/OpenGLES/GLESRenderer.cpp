// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "GLESRenderer.h"

#include "../../../../Engine/Scene/Nodes/ParticleSystem.h"
#include "../../../../Engine/Scene/Nodes/Camera.h"
#include "../../../../Engine/Scene/Nodes/Actor.h"
#include "../../Application/AndroidApplication.h"
#include "../../../../Engine/GUI/GUI.h"
#include "../../Platform.h"

#include "Resources/GLESTexture.h"
#include "Resources/GLESMesh.h"

namespace selene
{

        //--------------------------------------------------------------------------------------------
        bool GlesRenderer::initialize(const Renderer::Parameters& parameters)
        {
                parameters_ = parameters;
                writeLogEntry("--- Initializing OpenGL ES renderer ---");

                // check capabilities
                GLint maxNumVertexAttributes = 0;
                glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxNumVertexAttributes);
                if(maxNumVertexAttributes < NUM_OF_VERTEX_ATTRIBUTES_REQUIRED)
                {
                        writeLogEntry("ERROR: Not enough vertex attributes.");
                        return false;
                }

                GlesRenderer::checkGlesError("glGetIntegerv");

                GLint maxNumUniforms = 0;
                glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxNumUniforms);
                if(maxNumUniforms < NUM_OF_VERTEX_SHADER_UNIFORMS_REQUIRED)
                {
                        writeLogEntry("ERROR: Not enough vertex uniforms.");
                        return false;
                }

                GlesRenderer::checkGlesError("glGetIntegerv");

                return initializeHelpers();
        }

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::destroy()
        {
                destroyHelpers();
                destroyGlesContext();
        }

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::render(const Camera& camera)
        {
                // get rendering data
                auto& renderingData = const_cast<Renderer::Data&>(camera.getRenderingData());

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

                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // render some
                glEnable(GL_DEPTH_TEST);
                glDisable(GL_CULL_FACE);
                renderActors(renderingData.getActorNode(), programs_, nullptr, 0, RENDERING_PASS_SHADING);

                // render GUI
                Gui* gui = camera.getGui();
                guiRenderer_.render(gui);

                eglSwapBuffers(display_, surface_);
        }

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::checkGlesError(const char* operation)
        {
                for(GLint error = glGetError(); error; error = glGetError())
                {
                        LOGI("=========================================== after %s() glError (0x%x)\n", operation, error);
                }
        }

        GlesRenderer::GlesRenderer(): parameters_(nullptr, nullptr, 0, 0, nullptr, 0)
        {
                surface_ = EGL_NO_SURFACE;
                context_ = EGL_NO_CONTEXT;
                display_ = EGL_NO_DISPLAY;
        }
        GlesRenderer::GlesRenderer(const GlesRenderer&): Renderer(), Status(),
                                                         parameters_(nullptr, nullptr, 0, 0, nullptr, 0)
        {
                surface_ = EGL_NO_SURFACE;
                context_ = EGL_NO_CONTEXT;
                display_ = EGL_NO_DISPLAY;
        }
        GlesRenderer::~GlesRenderer()
        {
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
        void GlesRenderer::setTexture(const Resource::Instance<Texture>& texture, uint8_t sampler,
                                      uint8_t dummyTextureIndex)
        {
                setTexture(*texture, sampler, dummyTextureIndex);
        }

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::setTexture(Texture* texture, uint8_t sampler, uint8_t)
        {
                glActiveTexture(GL_TEXTURE0 + sampler);
                GlesRenderer::checkGlesError("setTexture: glActiveTexture");

                if(texture == nullptr)
                        glBindTexture(GL_TEXTURE_2D, dummyTexture_.texture_);
                else
                {
                        GlesTexture* glesTexture = static_cast<GlesTexture*>(texture);
                        glBindTexture(GL_TEXTURE_2D, glesTexture->texture_);
                }

                GlesRenderer::checkGlesError("setTexture: glBindTexture");
        }

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::setSkeletonPose(const Array<Skeleton::Transform, uint16_t>& boneTransforms,
                                           GLint bonePositionsLocation, GLint boneRotationsLocation)
        {
                if(boneTransforms.isEmpty())
                        return;

                static Quaternion rotations[MAX_NUM_OF_BONES];
                static Vector4d   positions[MAX_NUM_OF_BONES];

                uint16_t numBoneTransforms = (boneTransforms.getSize() > static_cast<uint16_t>(MAX_NUM_OF_BONES)) ?
                                             static_cast<uint16_t>(MAX_NUM_OF_BONES) : boneTransforms.getSize();

                for(uint16_t i = 0; i < numBoneTransforms; ++i)
                {
                        positions[i].define(boneTransforms[i].position, 0.0f);
                        rotations[i] = boneTransforms[i].rotation;
                }

                glUniform4fv(bonePositionsLocation, numBoneTransforms, reinterpret_cast<const float*>(positions));
                glUniform4fv(boneRotationsLocation, numBoneTransforms, reinterpret_cast<const float*>(rotations));
                checkGlesError("setSkeletonPose: glUniform4fv");
        }

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::renderActors(Renderer::Data::ActorNode& actorNode,
                                        GlesGlslProgram* programs,
                                        uint8_t*,
                                        uint8_t,
                                        uint8_t pass)
        {
                //const DWORD cullModes[] = {D3DCULL_CCW, D3DCULL_NONE};

                // walk through all mesh units
                for(uint8_t meshUnit = 0; meshUnit < Renderer::Data::NUM_OF_MESH_UNITS; ++meshUnit)
                {
                        auto& materialNode = actorNode.getMaterialNode(meshUnit);
                        programs[meshUnit].use();

                        GLint worldViewProjectionMatrixLocation  = programs[meshUnit].getUniformLocation("worldViewProjectionMatrix");
                        GLint bonePositionsLocation = programs[meshUnit].getUniformLocation("bonePositions");
                        GLint boneRotationsLocation = programs[meshUnit].getUniformLocation("boneRotations");

                        GLint textureLocation = programs[meshUnit].getUniformLocation("texture");
                        GLint positionsLocation          = programs[meshUnit].getAttributeLocation("position");
                        GLint textureCoordinatesLocation = programs[meshUnit].getAttributeLocation("textureCoordinates");

                        GLint weightsLocation = programs[meshUnit].getAttributeLocation("weights");
                        GLint indicesLocation = programs[meshUnit].getAttributeLocation("indices");

                        glUniform1i(textureLocation, 0);

                        // walk through all material units
                        for(uint8_t materialUnit = 0; materialUnit < Renderer::Data::NUM_OF_MATERIAL_UNITS; ++materialUnit)
                        {
                                //d3dDevice_->SetRenderState(D3DRS_CULLMODE, cullModes[materialUnit]);

                                // walk through all materials
                                for(bool materialResult = materialNode.readFirstElement(materialUnit); materialResult;
                                         materialResult = materialNode.readNextElement())
                                {
                                        auto material = materialNode.getCurrentKey();
                                        if(material == nullptr)
                                                break;

                                        auto meshNode = materialNode.getCurrentData();
                                        if(meshNode == nullptr)
                                                break;

                                        // set material here
                                        setTexture(material->getTextureMap(TEXTURE_MAP_DIFFUSE), 0, 0);
                                        /*switch(pass)
                                        {
                                                case RENDERING_PASS_POSITIONS_AND_NORMALS:
                                                case RENDERING_PASS_NORMALS:
                                                        setupNormalsPass(*material);
                                                        break;

                                                case RENDERING_PASS_SHADING:
                                                        setupShadingPass(*material);
                                                        break;
                                        }*/

                                        // walk through all meshes
                                        for(bool resultMesh = meshNode->readFirstElement(); resultMesh;
                                                 resultMesh = meshNode->readNextElement())
                                        {
                                                GlesMesh* glesMesh = static_cast<GlesMesh*>(meshNode->getCurrentKey());
                                                if(glesMesh == nullptr)
                                                        break;

                                                auto meshSubsetNode = meshNode->getCurrentData();
                                                if(meshSubsetNode == nullptr)
                                                        break;

                                                const auto& meshData = glesMesh->getData();

                                                // setup vertex streams
                                                glBindBuffer(GL_ARRAY_BUFFER, glesMesh->vertexBuffers_[Mesh::VERTEX_STREAM_POSITIONS]);
                                                GlesRenderer::checkGlesError("glBindBuffer");

                                                glEnableVertexAttribArray(positionsLocation);
                                                GlesRenderer::checkGlesError("glEnableVertexAttribArray");

                                                glVertexAttribPointer(positionsLocation, 3, GL_FLOAT, GL_FALSE,
                                                                      meshData.vertices[Mesh::VERTEX_STREAM_POSITIONS].getStride(),
                                                                      nullptr);
                                                GlesRenderer::checkGlesError("glVertexAttribPointer");

                                                // tex coords
                                                glBindBuffer(GL_ARRAY_BUFFER, glesMesh->vertexBuffers_[Mesh::VERTEX_STREAM_TEXTURE_COORDINATES]);
                                                GlesRenderer::checkGlesError("glBindBuffer");

                                                glEnableVertexAttribArray(textureCoordinatesLocation);
                                                GlesRenderer::checkGlesError("glEnableVertexAttribArray");

                                                glVertexAttribPointer(textureCoordinatesLocation, 2, GL_FLOAT, GL_FALSE,
                                                                      meshData.vertices[Mesh::VERTEX_STREAM_TEXTURE_COORDINATES].getStride(),
                                                                      nullptr);
                                                GlesRenderer::checkGlesError("glVertexAttribPointer");

                                                // skin
                                                if(glesMesh->hasSkeleton())
                                                {
                                                        glBindBuffer(GL_ARRAY_BUFFER, glesMesh->vertexBuffers_[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS]);
                                                        GlesRenderer::checkGlesError("skin: glBindBuffer");

                                                        glEnableVertexAttribArray(indicesLocation);
                                                        GlesRenderer::checkGlesError("skin i: glEnableVertexAttribArray");

                                                        glVertexAttribPointer(indicesLocation, 4, GL_FLOAT, GL_FALSE,
                                                                              meshData.vertices[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS].getStride(),
                                                                              nullptr);
                                                        GlesRenderer::checkGlesError("skin i: glVertexAttribPointer");

                                                        glEnableVertexAttribArray(weightsLocation);
                                                        GlesRenderer::checkGlesError("skin w: glEnableVertexAttribArray");

                                                        glVertexAttribPointer(weightsLocation, 4, GL_FLOAT, GL_FALSE,
                                                                              meshData.vertices[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS].getStride(),
                                                                              reinterpret_cast<uint8_t*>(sizeof(Vector4d)));
                                                        GlesRenderer::checkGlesError("skin1 w: glVertexAttribPointer");
                                                }

                                                glBindBuffer(GL_ARRAY_BUFFER, 0);

                                                // indices
                                                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glesMesh->indexBuffer_);
                                                GlesRenderer::checkGlesError("glBindBuffer");

                                                /*for(uint8_t vertexStream = 0; vertexStream < numVertexStreams; ++vertexStream)
                                                {
                                                        UINT streamNo = vertexStreamIndices[vertexStream];
                                                        d3dDevice_->SetStreamSource(streamNo, d3dMesh->d3dVertexBuffers_[streamNo], 0,
                                                                                    meshData.vertices[streamNo].getStride());
                                                }

                                                if(meshUnit == Renderer::Data::UNIT_MESH_SKIN)
                                                        d3dDevice_->SetStreamSource(3, d3dMesh->d3dVertexBuffers_[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS], 0,
                                                                                    meshData.vertices[Mesh::VERTEX_STREAM_BONE_INDICES_AND_WEIGHTS].getStride());

                                                d3dDevice_->SetIndices(d3dMesh->d3dIndexBuffer_);*/

                                                // walk through all mesh subsets
                                                for(bool resultMeshSubset = meshSubsetNode->readFirstElement(); resultMeshSubset;
                                                         resultMeshSubset = meshSubsetNode->readNextElement())
                                                {
                                                        auto meshSubset = meshSubsetNode->getCurrentKey();
                                                        auto instances  = meshSubsetNode->getCurrentData();

                                                        if(meshSubset == nullptr || instances == nullptr)
                                                                break;

                                                        renderActors(*meshSubset, *instances,
                                                                     worldViewProjectionMatrixLocation,
                                                                     -1, -1,
                                                                     bonePositionsLocation,
                                                                     boneRotationsLocation,
                                                                     meshUnit, pass);
                                                }
                                        }
                                }
                        }

                        glDisableVertexAttribArray(positionsLocation);
                        glDisableVertexAttribArray(textureCoordinatesLocation);

                        if(weightsLocation >= 0)
                                glDisableVertexAttribArray(weightsLocation);

                        if(indicesLocation >= 0)
                                glDisableVertexAttribArray(indicesLocation);
                }

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::renderActors(const Mesh::Subset& meshSubset,
                                        const std::vector<Renderer::Data::Instance>& instances,
                                        GLint worldViewProjectionMatrixLocation,
                                        GLint worldViewMatrixLocation,
                                        GLint normalsMatrixLocation,
                                        GLint bonePositionsLocation,
                                        GLint boneRotationsLocation,
                                        uint8_t meshRenderingUnit,
                                        uint8_t pass)
        {
                for(auto it = instances.begin(); it != instances.end(); ++it)
                {
                        const auto& transform = (*it).second;

                        glUniformMatrix4fv(worldViewProjectionMatrixLocation, 1, GL_FALSE, static_cast<const float*>(transform.getWorldViewProjectionMatrix()));
                        GlesRenderer::checkGlesError("renderActors: glUniformMatrix4fv");

                        switch(pass)
                        {
                                case RENDERING_PASS_POSITIONS:
                                        glUniform4fv(worldViewMatrixLocation, 4, static_cast<const float*>(transform.getWorldViewMatrix()));
                                        break;

                                case RENDERING_PASS_NORMALS:
                                        glUniform4fv(normalsMatrixLocation, 4, static_cast<const float*>(transform.getNormalsMatrix()));
                                        break;
                        }

                        if(meshRenderingUnit == Renderer::Data::UNIT_MESH_SKIN)
                                setSkeletonPose((*it).first->getFinalBoneTransforms(),
                                                bonePositionsLocation,
                                                boneRotationsLocation);

                        glDrawElements(GL_TRIANGLES, 3 * meshSubset.numFaces, GL_UNSIGNED_SHORT, reinterpret_cast<uint8_t*>(3 * meshSubset.faceIndex * sizeof(uint16_t)));
                        GlesRenderer::checkGlesError("renderActors: glDrawElements");
                }
        }

        //--------------------------------------------------------------------------------------------
        bool GlesRenderer::retain()
        {
                if(!initializeGlesContext())
                        return false;

                if(!retainHelpers())
                        return false;

                if(!ResourceManager::retainResources())
                        return false;

                return true;
        }

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::discard()
        {
                ResourceManager::discardResources();
                discardHelpers();
                destroyGlesContext();
        }

        //--------------------------------------------------------------------------------------------
        bool GlesRenderer::retainHelpers()
        {
                if(!guiRenderer_.retain())
                        return false;

                if(!dummyTexture_.retain())
                        return false;

                return true;
        }

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::discardHelpers()
        {
                programs_[0].destroy();
                programs_[1].destroy();
                guiRenderer_.discard();
                dummyTexture_.discard();
        }

        //--------------------------------------------------------------------------------------------
        bool GlesRenderer::initializeHelpers()
        {
                destroyHelpers();

                if(!guiRenderer_.initialize(parameters_.getFileManager()))
                {
                        writeLogEntry("ERROR: Could not initialize GUI renderer.");
                        return false;
                }

                static const char vertexShader[] =
                "attribute vec4 position;"
                "attribute vec4 textureCoordinates;"
                "uniform mat4 worldViewProjectionMatrix;"
                "varying vec4 vTextureCoordinates;"
                "void main()"
                "{"
                "        gl_Position = worldViewProjectionMatrix * position;"
                "        vTextureCoordinates  = textureCoordinates;"
                "}\n";

                static const char fragmentShader[] =
                "precision mediump float;"
                "varying vec4 vTextureCoordinates;"
                "uniform sampler2D texture;"
                "void main()"
                "{"
                "        gl_FragColor = vec4(0.1, 0.1, 0.1, 0.1) + texture2D(texture, vTextureCoordinates.xy);"
                "}\n";

                static const char skinVertexShader[] =
                "attribute vec4 position;\n"
                "attribute vec4 textureCoordinates;\n"
                "attribute vec4 weights;\n"
                "attribute vec4 indices;\n"
                "uniform mat4 worldViewProjectionMatrix;\n"
                "uniform vec4 bonePositions[50];\n"
                "uniform vec4 boneRotations[50];\n"
                "varying vec4 vTextureCoordinates;\n"
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
                "}\n"
                "void main()\n"
                "{\n"
                "        vec3 P = vec3(0.0, 0.0, 0.0);\n"
                "        P += weights.x * Transform(bonePositions[int(indices.x)], boneRotations[int(indices.x)], position.xyz);\n"
                "        P += weights.y * Transform(bonePositions[int(indices.y)], boneRotations[int(indices.y)], position.xyz);\n"
                "        P += weights.z * Transform(bonePositions[int(indices.z)], boneRotations[int(indices.z)], position.xyz);\n"
                "        P += weights.w * Transform(bonePositions[int(indices.w)], boneRotations[int(indices.w)], position.xyz);\n"
                "        gl_Position = worldViewProjectionMatrix * vec4(P, 1.0);\n"
                "        vTextureCoordinates  = textureCoordinates;\n"
                "}\n";

                dummyTexture_.getData().bpp = 4;
                dummyTexture_.getData().format = TEXTURE_FORMAT_NOT_COMPRESSED;
                dummyTexture_.getData().width  = 1;
                dummyTexture_.getData().height = 1;
                dummyTexture_.getData().numMipMaps = 0;
                if(!dummyTexture_.getData().pixels.create(4))
                {
                        writeLogEntry("ERROR: Could not create dummy texture.");
                        return false;
                }

                dummyTexture_.getData().pixels[0] = 255;
                dummyTexture_.getData().pixels[1] = 255;
                dummyTexture_.getData().pixels[2] = 255;
                dummyTexture_.getData().pixels[3] = 255;

                if(!dummyTexture_.retain())
                {
                        writeLogEntry("ERROR: Could not retain dummy texture.");
                        return false;
                }

                // initialize tempopary programs
                if(!programs_[0].initialize(vertexShader, fragmentShader) ||
                   !programs_[1].initialize(skinVertexShader, fragmentShader))
                {
                        writeLogEntry("ERROR: Could not initialize shaders.");
                        return false;
                }

                // prepare OpenGL ES
                glEnable(GL_CULL_FACE);
                glEnable(GL_DEPTH_TEST);
                GlesRenderer::checkGlesError("glEnable");

                return true;
        }

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::destroyHelpers()
        {
                programs_[0].destroy();
                programs_[1].destroy();
                guiRenderer_.destroy();
        }

        //--------------------------------------------------------------------------------------------
        bool GlesRenderer::initializeGlesContext()
        {
                const EGLint attributes[] =
                {
                        EGL_BLUE_SIZE, 8,
                        EGL_GREEN_SIZE, 8,
                        EGL_RED_SIZE, 8,
                        EGL_ALPHA_SIZE, 8,
                        EGL_DEPTH_SIZE, 24,
                        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                        EGL_NONE
                };

                const EGLint attributesList[] =
                {
                        EGL_CONTEXT_CLIENT_VERSION, 2,
                        EGL_NONE
                };

                android_app* state = Platform::state_;
                if(state == nullptr)
                        return false;

                EGLConfig config;
                EGLint numConfigs;

                display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
                if(display_ == EGL_NO_DISPLAY)
                        return false;

                if(!eglInitialize(display_, 0, 0))
                        return false;

                if(!eglChooseConfig(display_, attributes, &config, 1, &numConfigs))
                        return false;

                EGLint format;
                if(!eglGetConfigAttrib(display_, config, EGL_NATIVE_VISUAL_ID, &format))
                        return false;

                ANativeWindow_setBuffersGeometry(state->window, 0, 0, format);

                surface_ = eglCreateWindowSurface(display_, config, state->window, nullptr);
                if(surface_ == EGL_NO_SURFACE)
                        return false;

                context_ = eglCreateContext(display_, config, nullptr, attributesList);
                if(context_ == EGL_NO_CONTEXT)
                        return false;

                if(eglMakeCurrent(display_, surface_, surface_, context_) == EGL_FALSE)
                        return false;

                return true;
        }

        //--------------------------------------------------------------------------------------------
        void GlesRenderer::destroyGlesContext()
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

}
