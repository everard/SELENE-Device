float4 imageScale: register(c0);

struct VS_INPUT
{
        float4 position: POSITION0;
};

struct VS_OUTPUT
{
        float4 P: POSITION;
        float2 textureCoords: TEXCOORD0;
        float2 originalTextureCoords: TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT In)
{
        VS_OUTPUT Out;
        Out.P = float4(In.position.xy, 0.0, 1.0);
        Out.textureCoords = In.position.zw * imageScale.x + float2(0.0, 1.0 - imageScale.x);
        Out.originalTextureCoords = In.position.zw;
        return Out;
}
