float4 cursorPositionAndSize: register(c0);

struct VS_INPUT
{
        float4 position: POSITION0;
};

struct VS_OUTPUT
{
        float4 P: POSITION;
        float2 textureCoords: TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT In)
{
        VS_OUTPUT Out;
        float2 offset = float2(cursorPositionAndSize.z, -cursorPositionAndSize.z);
        Out.P = float4(In.position.xy * cursorPositionAndSize.z + cursorPositionAndSize.xy + offset, 0.0, 1.0);
        Out.textureCoords = In.position.zw;
        return Out;
}
