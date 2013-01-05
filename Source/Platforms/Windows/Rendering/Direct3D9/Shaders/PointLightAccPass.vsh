float4x4 mViewProjection: register(c0);
float4x4 mView: register(c8);
float4 LightPos[30]: register(c12);
float4 LightColor[30]: register(c42);

struct VS_INPUT
{
        float4 position: POSITION0;
};

struct VS_OUTPUT
{
        float4 P: POSITION;
        float4 LightColor: TEXCOORD0;
        float4 LightPosition: TEXCOORD1;
        float4 textureCoords: TEXCOORD2;
};

VS_OUTPUT main(VS_INPUT In)
{
        VS_OUTPUT Out;
        float4 LP = LightPos[In.position.w];
        float4 LC = LightColor[In.position.w];
        float3 p = LP.xyz + In.position.xyz * LP.w * 1.268;

        Out.P = mul(mViewProjection, float4(p, 1.0));
        Out.LightColor = LC;
        Out.LightPosition = mul(mView, float4(LP.xyz, 1.0));
        Out.LightPosition.w = 1.0 / (LP.w * LP.w);
        Out.textureCoords = Out.P;

        return Out;
}
