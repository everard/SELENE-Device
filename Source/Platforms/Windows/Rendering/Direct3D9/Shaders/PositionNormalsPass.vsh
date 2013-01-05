float4x4 mWorldViewProjection: register(c0);
float4x4 mWorldView: register(c4);
float4x4 mNormal: register(c8);

struct VS_INPUT
{
        float3 position:      POSITION0;
        float3 normal:        NORMAL0;
        float4 tangent:       TANGENT0;
        float2 textureCoords: TEXCOORD0;
};

struct VS_OUTPUT
{
        float4 P: POSITION;
        float4 TCNXY: TEXCOORD0;
        float4 NZT:   TEXCOORD1;
        float3 B:     TEXCOORD2;
        float4 pos:   TEXCOORD3;
};

VS_OUTPUT main(VS_INPUT In)
{
        VS_OUTPUT Out;

        Out.P = mul(mWorldViewProjection, float4(In.position, 1.0));
        Out.pos = mul(mWorldView, float4(In.position, 1.0));

        float3 N = mul(mNormal, In.normal);
        float3 T = mul(mNormal, In.tangent.xyz);
        Out.B    = In.tangent.w * cross(T, N);

        Out.TCNXY = float4(In.textureCoords.xy, N.xy);
        Out.NZT   = float4(N.z, T.xyz);

        return Out;
}
