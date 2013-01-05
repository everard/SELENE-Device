float4x4 mWorldViewProjection: register(c0);

struct VS_INPUT
{
        float4 position: POSITION0;
        float2 textureCoords: TEXCOORD0;
};

struct VS_OUTPUT
{
        float4 P: POSITION;
        float4 pos : TEXCOORD0;
        float2 textureCoords: TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT In)
{
        VS_OUTPUT Out;

        Out.P = mul(mWorldViewProjection, float4(In.position.xyz, 1.0));
        Out.pos = Out.P;
        Out.textureCoords = In.textureCoords;

        return Out;
}
