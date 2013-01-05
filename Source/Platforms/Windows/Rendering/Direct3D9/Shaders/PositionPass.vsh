float4x4 mWorldViewProjection: register(c0);
float4x4 mWorldView: register(c4);

struct VS_INPUT
{
        float3 position: POSITION0;
};

struct VS_OUTPUT
{
        float4 P: POSITION;
        float4 pos: TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT In)
{
        VS_OUTPUT Out;

        Out.P = mul(mWorldViewProjection, float4(In.position, 1.0));
        Out.pos = mul(mWorldView, float4(In.position, 1.0));

        return Out;
}
