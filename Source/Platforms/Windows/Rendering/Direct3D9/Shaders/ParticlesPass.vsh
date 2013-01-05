float4x4 mProjection: register(c0);
float4x4 mView: register(c4);

struct VS_INPUT
{
        float4 position: POSITION0;
        float4 color:    TEXCOORD0;
        float4 offsets:  TEXCOORD1;
};

struct VS_OUTPUT
{
        float4 P: POSITION;
        float4 textureCoords: TEXCOORD0;
        float4 position: TEXCOORD1;
        float4 color: TEXCOORD2;
};

VS_OUTPUT main(VS_INPUT In)
{
        VS_OUTPUT Out;
        float4 p = mul(mView, float4(In.position.xyz, 1.0));
        p.xyz /= p.w;
        p.xy += In.offsets.zw * In.position.w;

        Out.P = mul(mProjection, float4(p.xyz, 1.0));
        Out.textureCoords = In.offsets;

        Out.position = float4(Out.P.xy, p.z, Out.P.w);
        Out.color = In.color;

        return Out;
}
