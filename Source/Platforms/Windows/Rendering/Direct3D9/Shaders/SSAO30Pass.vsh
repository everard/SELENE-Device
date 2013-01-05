float4 screenSize: register(c0);

struct VS_INPUT
{
        float4 position: POSITION0;
};

struct VS_OUTPUT
{
        float4 P: POSITION;
        float4 textureCoords: TEXCOORD0;
        float2 RTexCoords: TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT In)
{
        VS_OUTPUT Out;
        Out.P = float4(In.position.xy, 0.0, 1.0);
        Out.textureCoords = In.position.zwxy;
        Out.RTexCoords = Out.textureCoords.xy * screenSize.xy / 64.0;
        return Out;
}
