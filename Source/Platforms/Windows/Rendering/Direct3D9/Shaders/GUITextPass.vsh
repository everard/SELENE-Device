struct VS_INPUT
{
        float4 position: POSITION0;
        float4 Color: TEXCOORD0;
};

struct VS_OUTPUT
{
        float4 P: POSITION;
        float4 Color: TEXCOORD0;
        float2 textureCoords: TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT In)
{
        VS_OUTPUT Out;
        Out.P = float4(float2(In.position.x, 2.0 - In.position.y) - float2(1.0, 1.0), 0.0, 1.0);
        Out.Color = In.Color;
        Out.textureCoords = In.position.zw;
        return Out;
}
