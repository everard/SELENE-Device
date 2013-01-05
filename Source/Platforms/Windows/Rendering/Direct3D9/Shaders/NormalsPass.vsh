float4x4 mWorldViewProjection: register(c0);
float4x4 mNormal: register(c8);

struct VS_INPUT
{
        float3 position: POSITION0;
        float3 normal: NORMAL0;
        float4 tangent: TANGENT0;
        float2 textureCoords: TEXCOORD0;
};

struct VS_OUTPUT
{
        float4 P: POSITION;
        float2 TC: TEXCOORD0;
        float3 N: TEXCOORD1;
        float3 T: TEXCOORD2;
        float3 B: TEXCOORD3;
};

VS_OUTPUT main(VS_INPUT In)
{
        VS_OUTPUT Out;

        Out.P = mul(mWorldViewProjection, float4(In.position, 1.0));

        Out.N = mul(mNormal, In.normal);
        Out.T = mul(mNormal, In.tangent.xyz);
        Out.B = In.tangent.w * cross(Out.T, Out.N);

        Out.TC = In.textureCoords;

        return Out;
}
