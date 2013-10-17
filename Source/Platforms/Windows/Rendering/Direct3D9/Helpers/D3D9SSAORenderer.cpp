// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "D3D9SSAORenderer.h"
#include "../D3D9Renderer.h"

namespace selene
{

        static float ssaoGeometryBuffer[] =
        {
                // radius 1.0
                -1.0f, -1.0f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, -0.5f, -0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, -0.5f, -0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 1.0f,
                 1.0f,  1.0f, 1.0f, 0.0f, -0.5f, -0.5f, -0.5f, 1.0f,

                -1.0f, -1.0f, 0.0f, 1.0f, 0.5f, -0.5f, -0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, -0.5f, -0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, -0.5f, -0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, -0.5f, -0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, -0.5f, -0.5f, 1.0f,
                 1.0f,  1.0f, 1.0f, 0.0f, 0.5f, -0.5f, -0.5f, 1.0f,

                -1.0f, -1.0f, 0.0f, 1.0f, -0.5f, 0.5f, -0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, 0.5f, -0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, 0.5f, -0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, 0.5f, -0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, 0.5f, -0.5f, 1.0f,
                 1.0f,  1.0f, 1.0f, 0.0f, -0.5f, 0.5f, -0.5f, 1.0f,

                -1.0f, -1.0f, 0.0f, 1.0f, 0.5f, 0.5f, -0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, 0.5f, -0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, 0.5f, -0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, 0.5f, -0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, 0.5f, -0.5f, 1.0f,
                 1.0f,  1.0f, 1.0f, 0.0f, 0.5f, 0.5f, -0.5f, 1.0f,

                -1.0f, -1.0f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, -0.5f, 0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, -0.5f, 0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, -0.5f, 0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, -0.5f, 0.5f, 1.0f,
                 1.0f,  1.0f, 1.0f, 0.0f, -0.5f, -0.5f, 0.5f, 1.0f,

                -1.0f, -1.0f, 0.0f, 1.0f, 0.5f, -0.5f, 0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, -0.5f, 0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, -0.5f, 0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f, 1.0f,
                 1.0f,  1.0f, 1.0f, 0.0f, 0.5f, -0.5f, 0.5f, 1.0f,

                -1.0f, -1.0f, 0.0f, 1.0f, -0.5f, 0.5f, 0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, 0.5f, 0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, 0.5f, 0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, 0.5f, 0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, 0.5f, 0.5f, 1.0f,
                 1.0f,  1.0f, 1.0f, 0.0f, -0.5f, 0.5f, 0.5f, 1.0f,

                -1.0f, -1.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f,
                 1.0f,  1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f,

                // radius 0.5
                -1.0f, -1.0f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.5f,
                 1.0f,  1.0f, 1.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.5f,

                -1.0f, -1.0f, 0.0f, 1.0f, 0.5f, -0.5f, -0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, -0.5f, -0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, -0.5f, -0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, -0.5f, -0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, -0.5f, -0.5f, 0.5f,
                 1.0f,  1.0f, 1.0f, 0.0f, 0.5f, -0.5f, -0.5f, 0.5f,

                -1.0f, -1.0f, 0.0f, 1.0f, -0.5f, 0.5f, -0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, 0.5f, -0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, 0.5f, -0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, 0.5f, -0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, 0.5f, -0.5f, 0.5f,
                 1.0f,  1.0f, 1.0f, 0.0f, -0.5f, 0.5f, -0.5f, 0.5f,

                -1.0f, -1.0f, 0.0f, 1.0f, 0.5f, 0.5f, -0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, 0.5f, -0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, 0.5f, -0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, 0.5f, -0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, 0.5f, -0.5f, 0.5f,
                 1.0f,  1.0f, 1.0f, 0.0f, 0.5f, 0.5f, -0.5f, 0.5f,

                -1.0f, -1.0f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, -0.5f, 0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, -0.5f, 0.5f, 0.5f,
                 1.0f,  1.0f, 1.0f, 0.0f, -0.5f, -0.5f, 0.5f, 0.5f,

                -1.0f, -1.0f, 0.0f, 1.0f, 0.5f, -0.5f, 0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, -0.5f, 0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, -0.5f, 0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f, 0.5f,
                 1.0f,  1.0f, 1.0f, 0.0f, 0.5f, -0.5f, 0.5f, 0.5f,

                -1.0f, -1.0f, 0.0f, 1.0f, -0.5f, 0.5f, 0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, 0.5f, 0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, 0.5f, 0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, -0.5f, 0.5f, 0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, -0.5f, 0.5f, 0.5f, 0.5f,
                 1.0f,  1.0f, 1.0f, 0.0f, -0.5f, 0.5f, 0.5f, 0.5f,

                -1.0f, -1.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f, 0.5f,
                 1.0f, -1.0f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f, 0.5f,
                -1.0f,  1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.5f,
                 1.0f,  1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.5f
        };

        static const uint32_t ssaoGeometryVertexStride = 2 * sizeof(Vector4d);
        static const uint32_t ssaoGeometryNumVertices = 96;
        static const uint32_t ssaoGeometryNumFaces = 32;
        static const uint32_t ssaoGeometryBufferSize = ssaoGeometryVertexStride * ssaoGeometryNumVertices;

        // Fills random texture
        VOID WINAPI fillRandomTexture(D3DXVECTOR4* output, const D3DXVECTOR2*, const D3DXVECTOR2*, LPVOID)
        {
                static D3DXVECTOR4 colors[16] =
                {
                        D3DXVECTOR4(138.0f / 255.0f, 106.0f / 255.0f, 241.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(119.0f / 255.0f, 37.0f / 255.0f, 111.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(149.0f / 255.0f, 208.0f / 255.0f, 90.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(138.0f / 255.0f, 184.0f / 255.0f, 23.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(57.0f / 255.0f, 105.0f / 255.0f, 205.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(20.0f / 255.0f, 134.0f / 255.0f, 144.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(46.0f / 255.0f, 87.0f / 255.0f, 83.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(75.0f / 255.0f, 160.0f / 255.0f, 33.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(81.0f / 255.0f, 208.0f / 255.0f, 127.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(156.0f / 255.0f, 202.0f / 255.0f, 199.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(90.0f / 255.0f, 174.0f / 255.0f, 200.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(208.0f / 255.0f, 155.0f / 255.0f, 77.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(215.0f / 255.0f, 72.0f / 255.0f, 92.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(229.0f / 255.0f, 89.0f / 255.0f, 196.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(139.0f / 255.0f, 43.0f / 255.0f, 194.0f / 255.0f, 1.0f),
                        D3DXVECTOR4(129.0f / 255.0f, 75.0f / 255.0f, 28.0f / 255.0f, 1.0f)
                };

                *output = colors[rand() % 16];
        }

        D3d9SsaoRenderer::D3d9SsaoRenderer():
                d3dVertexDeclaration_(nullptr), d3dVertexBuffer_(nullptr), d3dRandomTexture_(nullptr),
                d3dDevice_(nullptr), renderTargetContainer_(nullptr), frameParameters_(nullptr),
                fullScreenQuad_(nullptr), textureHandler_(nullptr), capabilities_(nullptr) {}
        D3d9SsaoRenderer::~D3d9SsaoRenderer()
        {
                destroy();
        }

        //-------------------------------------------------------------------------------------------------------------
        bool D3d9SsaoRenderer::initialize(D3d9RenderTargetContainer& renderTargetContainer,
                                          D3d9FrameParameters& frameParameters,
                                          D3d9FullScreenQuad& fullScreenQuad,
                                          D3d9TextureHandler& textureHandler,
                                          D3d9Capabilities& capabilities)
        {
                destroy();

                renderTargetContainer_ = &renderTargetContainer;
                frameParameters_ = &frameParameters;
                fullScreenQuad_ = &fullScreenQuad;
                textureHandler_ = &textureHandler;
                capabilities_ = &capabilities;

                d3dDevice_ = D3d9Renderer::getDevice();
                if(d3dDevice_ == nullptr)
                        return false;

                static const char* vertexShaderSsaoPass =
                        "float4 screenSize: register(c0);"
                        ""
                        "struct VS_INPUT"
                        "{"
                        "        float4 position: POSITION0;"
                        "        float4 TraceDirection: TEXCOORD0;"
                        "};"
                        ""
                        "struct VS_OUTPUT"
                        "{"
                        "        float4 P: POSITION;"
                        "        float4 textureCoords: TEXCOORD0;"
                        "        float4 TraceDirection: TEXCOORD1;"
                        "        float2 RTexCoords: TEXCOORD2;"
                        "};"
                        ""
                        "VS_OUTPUT main(VS_INPUT In)"
                        "{"
                        "        VS_OUTPUT Out;"
                        "        Out.P = float4(In.position.xy, 0.0, 1.0);"
                        "        Out.textureCoords = In.position.zwxy;"
                        "        Out.TraceDirection = In.TraceDirection;"
                        "        Out.RTexCoords = Out.textureCoords.xy * screenSize.xy / 64.0;"
                        "        return Out;"
                        "}";

                static const char* pixelShaderSsaoPass =
                        "sampler2D GBufferPosition: register(s0);"
                        "sampler2D GBufferNormals: register(s1);"
                        "sampler2D RandomDirections: register(s2);"
                        "float4 unprojectionVector: register(c0);"
                        "float4 projectionParameters: register(c1);"
                        "float4 ssaoParams: register(c2);"
                        ""
                        "struct PS_INPUT"
                        "{"
                        "        float4 textureCoords: TEXCOORD0;"
                        "        float4 TraceDirection: TEXCOORD1;"
                        "        float2 RTexCoords: TEXCOORD2;"
                        "};"
                        ""
                        "float4 main(PS_INPUT In): COLOR0"
                        "{"
                        "        float3 p = decodePosition(tex2D(GBufferPosition, In.textureCoords.xy),"
                        "                                  In.textureCoords.zw,"
                        "                                  projectionParameters,"
                        "                                  unprojectionVector);"
                        "        float3 n = decodeNormal(tex2D(GBufferNormals, In.textureCoords.xy).xyz);"
                        "        float3 plane = normalize(2.0 * tex2D(RandomDirections, In.RTexCoords).xyz -"
                        "                                 float3(1.0, 1.0, 1.0));"
                        "        float3 sample = reflect(In.TraceDirection, plane);"
                        ""
                        "        float4 textureCoords;"
                        "        textureCoords.xy = In.textureCoords.xy +"
                        "                           (In.TraceDirection.w * ssaoParams.x * sample.xy / p.z);"
                        "        textureCoords.zw = 2.0 * float2( textureCoords.x,"
                        "                                        -textureCoords.y) - float2(1.0, -1.0);"
                        ""
                        "        float3 p1 = decodePosition(tex2D(GBufferPosition, textureCoords.xy),"
                        "                                   textureCoords.zw,"
                        "                                   projectionParameters,"
                        "                                   unprojectionVector);"
                        "        float3 dir = p1 - p;"
                        "        float DirLen = length(dir);"
                        "        dir /= DirLen;"
                        ""
                        "        float norm = clamp(dot(dir, n) + ssaoParams.y, 0.0, 1.0);"
                        "        float range = 1.0 - clamp(DirLen / ssaoParams.x, 0.0, 1.0);"
                        ""
                        "        float ATT = norm * range * 0.06875;"
                        "        return ATT.xxxx;"
                        "}";

                static const char* vertexShaderBlurPass =
                        "struct VS_INPUT"
                        "{"
                        "        float4 position: POSITION0;"
                        "};"
                        ""
                        "struct VS_OUTPUT"
                        "{"
                        "        float4 P: POSITION;"
                        "        float4 textureCoords: TEXCOORD0;"
                        "};"
                        ""
                        "VS_OUTPUT main(VS_INPUT In)"
                        "{"
                        "        VS_OUTPUT Out;"
                        "        Out.P = float4(In.position.xy, 0.0, 1.0);"
                        "        Out.textureCoords = In.position.zwxy;"
                        "        return Out;"
                        "}";

                static const char* pixelShaderHorizontalBlurPass =
                        "sampler2D GBufferPosition: register(s0);"
                        "sampler2D GBufferNormals: register(s1);"
                        "sampler2D Texture: register(s2);"
                        "float4 unprojectionVector: register(c0);"
                        "float4 projectionParameters: register(c1);"
                        "float4 edgeDetectionParams: register(c2);"
                        "float4 screenSize: register(c3);"
                        "float4 texCoordsAdjustment: register(c4);"
                        ""
                        "struct PS_INPUT"
                        "{"
                        "        float4 textureCoords: TEXCOORD0;"
                        "};"
                        ""
                        "float4 main(PS_INPUT In):COLOR0"
                        "{"
                        "        In.textureCoords.xy += texCoordsAdjustment.zw;"
                        "        float2 k = float2(edgeDetectionParams.z, 0.0);"
                        "        float4 sum = tex2D(Texture, In.textureCoords.xy);"
                        "        float p = decodeEyeZ(tex2D(GBufferPosition, In.textureCoords.xy),"
                        "                             projectionParameters);"
                        "        float3 n = decodeNormal(tex2D(GBufferNormals, In.textureCoords.xy).xyz);"
                        "        float num = 1.0;"
                        ""
                        "        float2 textureCoords;"
                        "        textureCoords.xy = In.textureCoords.xy;"
                        "        textureCoords.xy += k;"
                        "        float p1 = decodeEyeZ(tex2D(GBufferPosition, textureCoords.xy),"
                        "                              projectionParameters);"
                        "        float3 n1 = decodeNormal(tex2D(GBufferNormals, textureCoords.xy).xyz);"
                        ""
                        "        if(abs(p - p1) <= edgeDetectionParams.x && dot(n, n1) >= edgeDetectionParams.y)"
                        "        {"
                        "                num += 1.0;"
                        "                sum += tex2D(Texture, textureCoords.xy);"
                        "        }"
                        ""
                        "        textureCoords.xy = In.textureCoords.xy;"
                        "        textureCoords.xy -= k;"
                        "        float p2 = decodeEyeZ(tex2D(GBufferPosition, textureCoords.xy),"
                        "                              projectionParameters);"
                        "        float3 n2 = decodeNormal(tex2D(GBufferNormals, textureCoords.xy).xyz);"
                        ""
                        "        if(abs(p - p2) <= edgeDetectionParams.x && dot(n, n2) >= edgeDetectionParams.y)"
                        "        {"
                        "                num += 1.0;"
                        "                sum += tex2D(Texture, textureCoords.xy);"
                        "        }"
                        "        sum /= num;"
                        "        return sum;"
                        "}";

                static const char* pixelShaderVerticalBlurPass =
                        "sampler2D GBufferPosition: register(s0);"
                        "sampler2D GBufferNormals: register(s1);"
                        "sampler2D Texture: register(s2);"
                        "float4 unprojectionVector: register(c0);"
                        "float4 projectionParameters: register(c1);"
                        "float4 edgeDetectionParams: register(c2);"
                        "float4 screenSize: register(c3);"
                        "float4 texCoordsAdjustment: register(c4);"
                        ""
                        "struct PS_INPUT"
                        "{"
                        "        float4 textureCoords: TEXCOORD0;"
                        "};"
                        ""
                        "float4 main(PS_INPUT In):COLOR0"
                        "{"
                        "        In.textureCoords.xy += texCoordsAdjustment.zw;"
                        "        float2 k = float2(0.0, edgeDetectionParams.w);"
                        "        float4 sum = tex2D(Texture, In.textureCoords.xy);"
                        "        float p = decodeEyeZ(tex2D(GBufferPosition, In.textureCoords.xy),"
                        "                             projectionParameters);"
                        "        float3 n = decodeNormal(tex2D(GBufferNormals, In.textureCoords.xy).xyz);"
                        "        float num = 1.0;"
                        ""
                        "        float4 textureCoords;"
                        "        textureCoords.xy = In.textureCoords.xy;"
                        "        textureCoords.xy += k;"
                        "        float p1 = decodeEyeZ(tex2D(GBufferPosition, textureCoords.xy),"
                        "                              projectionParameters);"
                        "        float3 n1 = decodeNormal(tex2D(GBufferNormals, textureCoords.xy).xyz);"
                        ""
                        "        if(abs(p - p1) <= edgeDetectionParams.x && dot(n, n1) >= edgeDetectionParams.y)"
                        "        {"
                        "                num += 1.0;"
                        "                sum += tex2D(Texture, textureCoords.xy);"
                        "        }"
                        ""
                        "        textureCoords.xy = In.textureCoords.xy;"
                        "        textureCoords.xy -= k;"
                        "        float p2 = decodeEyeZ(tex2D(GBufferPosition, textureCoords.xy),"
                        "                              projectionParameters);"
                        "        float3 n2 = decodeNormal(tex2D(GBufferNormals, textureCoords.xy).xyz);"
                        ""
                        "        if(abs(p - p2) <= edgeDetectionParams.x && dot(n, n2) >= edgeDetectionParams.y)"
                        "        {"
                        "                num += 1.0;"
                        "                sum += tex2D(Texture, textureCoords.xy);"
                        "        }"
                        "        sum /= num;"
                        "        return sum;"
                        "}";

                // load shaders
                D3d9Shader d3dVertexShaders[NUM_OF_VERTEX_SHADERS] =
                {
                        D3d9Shader(vertexShaderSsaoPass, "vs_1_1", 0,
                                   D3d9Shader::LIBRARY_EMPTY, *capabilities_),
                        D3d9Shader(vertexShaderBlurPass, "vs_1_1", 0,
                                   D3d9Shader::LIBRARY_EMPTY, *capabilities_)
                };

                D3d9Shader d3dPixelShaders[NUM_OF_PIXEL_SHADERS] =
                {
                        D3d9Shader(pixelShaderSsaoPass,           "ps_2_0", 0,
                                   D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_),
                        D3d9Shader(pixelShaderHorizontalBlurPass, "ps_2_0", 0,
                                   D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_),
                        D3d9Shader(pixelShaderVerticalBlurPass,   "ps_2_0", 0,
                                   D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_)
                };

                for(uint32_t i = 0; i < NUM_OF_VERTEX_SHADERS; ++i)
                {
                        if(!vertexShaders_[i].create(d3dVertexShaders[i]))
                        {
                                destroy();
                                return false;
                        }
                }

                for(uint32_t i = 0; i < NUM_OF_PIXEL_SHADERS; ++i)
                {
                        if(!pixelShaders_[i].create(d3dPixelShaders[i]))
                        {
                                destroy();
                                return false;
                        }
                }

                static const char* optionalVertexShaderSsaoPass =
                        "float4 screenSize: register(c0);"
                        ""
                        "struct Input"
                        "{"
                        "        float4 position: POSITION0;"
                        "};"
                        ""
                        "struct Output"
                        "{"
                        "        float4 position:            POSITION;"
                        "        float4 textureCoordinates0: TEXCOORD0;"
                        "        float2 textureCoordinates1: TEXCOORD1;"
                        "};"
                        ""
                        "Output main(Input input)"
                        "{"
                        "        Output output;"
                        ""
                        "        output.position = float4(input.position.xy, 0.0, 1.0);"
                        "        output.textureCoordinates0 = input.position.zwxy;"
                        "        output.textureCoordinates1 = output.textureCoordinates0.xy *"
                        "                                     screenSize.xy / 64.0;"
                        ""
                        "        return output;"
                        "}";

                static const char* optionalPixelShaderSsaoPass =
                        "float4 unprojectionVector:   register(c0);"
                        "float4 projectionParameters: register(c1);"
                        ""
                        "float4 ssaoParameters: register(c2);"
                        ""
                        "sampler2D positionsBuffer: register(s0);"
                        "sampler2D normalsBuffer:   register(s1);"
                        ""
                        "sampler2D randomDirectionsMap: register(s2);"
                        ""
                        "struct Input"
                        "{"
                        "        float4 textureCoordinates0: TEXCOORD0;"
                        "        float2 textureCoordinates1: TEXCOORD1;"
                        "};"
                        ""
                        "float4 main(Input input): COLOR0"
                        "{"
                        "        float4 traceDirections[] ="
                        "        {"
                        "                float4(-0.5, -0.5, -0.5, 1.0),"
                        "                float4( 0.5, -0.5, -0.5, 1.0),"
                        "                float4(-0.5,  0.5, -0.5, 1.0),"
                        "                float4( 0.5,  0.5, -0.5, 1.0),"
                        "                float4(-0.5, -0.5,  0.5, 1.0),"
                        "                float4( 0.5, -0.5,  0.5, 1.0),"
                        "                float4(-0.5,  0.5,  0.5, 1.0),"
                        "                float4( 0.5,  0.5,  0.5, 1.0),"
                        ""
                        "                float4(-0.5, -0.5, -0.5, 0.5),"
                        "                float4( 0.5, -0.5, -0.5, 0.5),"
                        "                float4(-0.5,  0.5, -0.5, 0.5),"
                        "                float4( 0.5,  0.5, -0.5, 0.5),"
                        "                float4(-0.5, -0.5,  0.5, 0.5),"
                        "                float4( 0.5, -0.5,  0.5, 0.5),"
                        "                float4(-0.5,  0.5,  0.5, 0.5),"
                        "                float4( 0.5,  0.5,  0.5, 0.5)"
                        "        };"
                        ""
                        "        float attenuation = 0.0;"
                        ""
                        "        float3 position = decodePosition(tex2D(positionsBuffer,"
                        "                                               input.textureCoordinates0.xy),"
                        "                                         input.textureCoordinates0.zw,"
                        "                                         projectionParameters,"
                        "                                         unprojectionVector);"
                        "        float3 normal = decodeNormal(tex2D(normalsBuffer,"
                        "                                           input.textureCoordinates0.xy).xyz);"
                        "        float3 plane = normalize(2.0 * tex2D(randomDirectionsMap,"
                        "                                             input.textureCoordinates1).xyz -"
                        "                                 float3(1.0, 1.0, 1.0));"
                        ""
                        "        float ssaoRadiusInv = 1.0 / ssaoParameters.x;"
                        "        float traceDirectionScaling = ssaoParameters.x / position.z;"
                        ""
                        "        for(int i = 0; i < 16; ++i)"
                        "        {"
                        "                float4 traceDirection  = traceDirections[i];"
                        "                float3 sampleDirection = reflect(traceDirection, plane);"
                        ""
                        "                float4 textureCoordinates;"
                        "                textureCoordinates.xy = input.textureCoordinates0.xy +"
                        "                                        (traceDirection.w * sampleDirection.xy *"
                        "                                         traceDirectionScaling);"
                        "                textureCoordinates.zw = 2.0 * float2( textureCoordinates.x,"
                        "                                                     -textureCoordinates.y) -"
                        "                                        float2(1.0, -1.0);"
                        ""
                        "                float3 newPosition = decodePosition(tex2D(positionsBuffer,"
                        "                                                          textureCoordinates.xy),"
                        "                                                    textureCoordinates.zw,"
                        "                                                    projectionParameters,"
                        "                                                    unprojectionVector);"
                        ""
                        "                float3 direction = newPosition - position;"
                        "                float distance = length(direction);"
                        "                direction /= distance;"
                        ""
                        "                float factor0 = 1.0 - clamp(distance * ssaoRadiusInv, 0.0, 1.0);"
                        "                float factor1 = clamp(dot(direction, normal) + ssaoParameters.y, 0.0, 1.0);"
                        ""
                        "                attenuation += factor0 * factor1;"
                        "        }"
                        ""
                        "        return attenuation.xxxx * 0.06875;"
                        "}";

                if(capabilities_->isThirdShaderModelSupported())
                {
                        D3d9Shader d3dOptionalVertexShader(optionalVertexShaderSsaoPass, "vs_1_1", 0,
                                                           D3d9Shader::LIBRARY_VERTEX_SHADER, *capabilities_);
                        D3d9Shader d3dOptionalPixelShader(optionalPixelShaderSsaoPass,   "ps_3_0", 0,
                                                          D3d9Shader::LIBRARY_PIXEL_SHADER, *capabilities_);

                        if(!optionalVertexShaders_[OPTIONAL_VERTEX_SHADER_SSAO_PASS].create(d3dOptionalVertexShader) ||
                           !optionalPixelShaders_[OPTIONAL_PIXEL_SHADER_SSAO_PASS].create(d3dOptionalPixelShader))
                        {
                                destroy();
                                return false;
                        }
                }

                // create vertex declaration and vertex buffer
                static D3DVERTEXELEMENT9 d3dVertexElements[] =
                {
                        {0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
                        {0, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
                        D3DDECL_END()
                };

                if(FAILED(d3dDevice_->CreateVertexDeclaration(d3dVertexElements, &d3dVertexDeclaration_)))
                {
                        destroy();
                        return false;
                }

                if(FAILED(d3dDevice_->CreateVertexBuffer(ssaoGeometryBufferSize,
                                                         0, 0, D3DPOOL_DEFAULT, &d3dVertexBuffer_, nullptr)))
                {
                        destroy();
                        return false;
                }

                void* destinationBuffer = nullptr;
                if(FAILED(d3dVertexBuffer_->Lock(0, ssaoGeometryBufferSize, &destinationBuffer, 0)))
                {
                        destroy();
                        return false;
                }
                memcpy(destinationBuffer, static_cast<void*>(ssaoGeometryBuffer), ssaoGeometryBufferSize);
                d3dVertexBuffer_->Unlock();

                // create random texture
                if(FAILED(d3dDevice_->CreateTexture(64, 64, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
                                                    &d3dRandomTexture_, nullptr)))
                {
                        destroy();
                        return false;
                }

                srand(0);
                if(FAILED(D3DXFillTexture(d3dRandomTexture_, fillRandomTexture, nullptr)))
                {
                        destroy();
                        return false;
                }

                return true;
        }

        //-------------------------------------------------------------------------------------------------------------
        void D3d9SsaoRenderer::destroy()
        {
                for(uint32_t i = 0; i < NUM_OF_OPTIONAL_VERTEX_SHADERS; ++i)
                        optionalVertexShaders_[i].destroy();

                for(uint32_t i = 0; i < NUM_OF_OPTIONAL_PIXEL_SHADERS; ++i)
                        optionalPixelShaders_[i].destroy();

                for(uint32_t i = 0; i < NUM_OF_VERTEX_SHADERS; ++i)
                        vertexShaders_[i].destroy();

                for(uint32_t i = 0; i < NUM_OF_PIXEL_SHADERS; ++i)
                        pixelShaders_[i].destroy();

                SAFE_RELEASE(d3dVertexDeclaration_);
                SAFE_RELEASE(d3dVertexBuffer_);
                SAFE_RELEASE(d3dRandomTexture_);

                d3dDevice_ = nullptr;

                renderTargetContainer_ = nullptr;
                frameParameters_ = nullptr;
                fullScreenQuad_ = nullptr;
                textureHandler_ = nullptr;
                capabilities_ = nullptr;
        }

        //-------------------------------------------------------------------------------------------------------------
        void D3d9SsaoRenderer::renderSsao()
        {
                if(d3dDevice_ == nullptr)
                        return;

                d3dDevice_->SetStreamSource(1, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(2, nullptr, 0, 0);
                d3dDevice_->SetStreamSource(3, nullptr, 0, 0);

                if(capabilities_->isThirdShaderModelSupported())
                {
                        const auto& renderTarget = renderTargetContainer_->getRenderTarget(RENDER_TARGET_HELPER_0);
                        d3dDevice_->SetRenderTarget(0, renderTarget.getSurface());
                        d3dDevice_->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

                        optionalVertexShaders_[OPTIONAL_VERTEX_SHADER_SSAO_PASS].set();
                        optionalPixelShaders_[OPTIONAL_PIXEL_SHADER_SSAO_PASS].set();
                }
                else
                {
                        const auto& renderTarget =
                                renderTargetContainer_->getRenderTarget(RENDER_TARGET_HALF_SIZE_HELPER);
                        d3dDevice_->SetRenderTarget(0, renderTarget.getSurface());
                        d3dDevice_->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

                        vertexShaders_[VERTEX_SHADER_SSAO_PASS].set();
                        pixelShaders_[PIXEL_SHADER_SSAO_PASS].set();
                }

                d3dDevice_->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
                d3dDevice_->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

                d3dDevice_->SetPixelShaderConstantF(LOCATION_UNPROJECTION_VECTOR,
                                                    static_cast<const float*>(frameParameters_->unprojectionVector),
                                                    1);
                d3dDevice_->SetPixelShaderConstantF(LOCATION_PROJECTION_PARAMETERS,
                                                    static_cast<const float*>(frameParameters_->projectionParameters),
                                                    1);
                d3dDevice_->SetPixelShaderConstantF(LOCATION_SSAO_PARAMETERS,
                                                    static_cast<const float*>(frameParameters_->ssaoParameters),
                                                    1);

                d3dDevice_->SetVertexShaderConstantF(LOCATION_SCREEN_SIZE_SSAO_PASS,
                                                     static_cast<const float*>(frameParameters_->screenSize), 1);

                textureHandler_->setStageState(LOCATION_POSITIONS_MAP,
                                               D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE,
                                               D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(LOCATION_POSITIONS_MAP,
                                       renderTargetContainer_->getRenderTarget(RENDER_TARGET_POSITIONS).getTexture());

                textureHandler_->setStageState(LOCATION_NORMALS_MAP,
                                               D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE,
                                               D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
                d3dDevice_->SetTexture(LOCATION_NORMALS_MAP,
                                       renderTargetContainer_->getRenderTarget(RENDER_TARGET_NORMALS).getTexture());

                textureHandler_->setStageState(LOCATION_HELPER_TEXTURE, D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE);
                d3dDevice_->SetTexture(LOCATION_HELPER_TEXTURE, d3dRandomTexture_);

                if(capabilities_->isThirdShaderModelSupported())
                {
                        fullScreenQuad_->render();
                }
                else
                {
                        d3dDevice_->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ONE);
                        d3dDevice_->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
                        d3dDevice_->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

                        d3dDevice_->SetVertexDeclaration(d3dVertexDeclaration_);
                        d3dDevice_->SetStreamSource(0, d3dVertexBuffer_, 0, ssaoGeometryVertexStride);
                        d3dDevice_->DrawPrimitive(D3DPT_TRIANGLELIST, 0, ssaoGeometryNumFaces);

                        d3dDevice_->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
                }

                // blur SSAO
                Vector4d edgeDetectionParameters(frameParameters_->ssaoParameters.x,
                                                 frameParameters_->ssaoParameters.z, 0.0f, 0.0f);
                edgeDetectionParameters.z = 4.0f / frameParameters_->screenSize.x;
                edgeDetectionParameters.w = 4.0f / frameParameters_->screenSize.y;

                blurSsao(edgeDetectionParameters, !capabilities_->isThirdShaderModelSupported());
                edgeDetectionParameters.z = 2.0f / frameParameters_->screenSize.x;
                edgeDetectionParameters.w = 2.0f / frameParameters_->screenSize.y;
                blurSsao(edgeDetectionParameters);
                edgeDetectionParameters.z = 1.0f / frameParameters_->screenSize.x;
                edgeDetectionParameters.w = 1.0f / frameParameters_->screenSize.y;
                blurSsao(edgeDetectionParameters);
        }

        //-------------------------------------------------------------------------------------------------------------
        void D3d9SsaoRenderer::blurSsao(const Vector4d& edgeDetectionParameters,
                                        bool shouldUpscale)
        {
                LPDIRECT3DSURFACE9 renderTargets[] =
                {
                        renderTargetContainer_->getRenderTarget(RENDER_TARGET_HELPER_1).getSurface(),
                        renderTargetContainer_->getRenderTarget(RENDER_TARGET_HELPER_0).getSurface()
                };

                LPDIRECT3DTEXTURE9 textures[] =
                {
                        renderTargetContainer_->getRenderTarget(RENDER_TARGET_HELPER_0).getTexture(),
                        renderTargetContainer_->getRenderTarget(RENDER_TARGET_HELPER_1).getTexture()
                };

                for(uint8_t i = 0; i < 2; ++i)
                {
                        d3dDevice_->SetRenderTarget(0, renderTargets[i]);
                        d3dDevice_->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

                        vertexShaders_[VERTEX_SHADER_BLUR_PASS].set();
                        pixelShaders_[PIXEL_SHADER_HORIZONTAL_BLUR_PASS + i].set();

                        d3dDevice_->SetPixelShaderConstantF(LOCATION_UNPROJECTION_VECTOR,
                                                            frameParameters_->unprojectionVector,
                                                            1);
                        d3dDevice_->SetPixelShaderConstantF(LOCATION_PROJECTION_PARAMETERS,
                                                            frameParameters_->projectionParameters,
                                                            1);
                        d3dDevice_->SetPixelShaderConstantF(LOCATION_EDGE_DETECTION_PARAMETERS,
                                                            edgeDetectionParameters,
                                                            1);
                        d3dDevice_->SetPixelShaderConstantF(LOCATION_SCREEN_SIZE_BLUR_PASS,
                                                            frameParameters_->screenSize,
                                                            1);
                        d3dDevice_->SetPixelShaderConstantF(LOCATION_TEXTURE_COORDINATES_ADJUSTMENT,
                                                            frameParameters_->textureCoordinatesAdjustment,
                                                            1);

                        const auto& positionsMap = renderTargetContainer_->getRenderTarget(RENDER_TARGET_POSITIONS);
                        textureHandler_->setStageState(LOCATION_POSITIONS_MAP,
                                                       D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE,
                                                       D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
                        d3dDevice_->SetTexture(LOCATION_POSITIONS_MAP, positionsMap.getTexture());

                        const auto& normalsMap = renderTargetContainer_->getRenderTarget(RENDER_TARGET_NORMALS);
                        textureHandler_->setStageState(LOCATION_NORMALS_MAP,
                                                       D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE,
                                                       D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
                        d3dDevice_->SetTexture(LOCATION_NORMALS_MAP, normalsMap.getTexture());

                        if(shouldUpscale)
                        {
                                const auto& ssaoBuffer =
                                        renderTargetContainer_->getRenderTarget(RENDER_TARGET_HALF_SIZE_HELPER);
                                textureHandler_->setStageState(LOCATION_SSAO_BUFFER,
                                                               D3DTEXF_LINEAR, D3DTEXF_POINT, D3DTEXF_LINEAR,
                                                               D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
                                d3dDevice_->SetTexture(LOCATION_SSAO_BUFFER, ssaoBuffer.getTexture());
                                shouldUpscale = false;
                        }
                        else
                        {
                                textureHandler_->setStageState(LOCATION_SSAO_BUFFER,
                                                               D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_NONE,
                                                               D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
                                d3dDevice_->SetTexture(LOCATION_SSAO_BUFFER, textures[i]);
                        }

                        fullScreenQuad_->render();
                }
        }

}
