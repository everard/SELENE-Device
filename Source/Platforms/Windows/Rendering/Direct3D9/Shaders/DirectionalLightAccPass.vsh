float4x4 mNormal: register(c4);
float4 LightDir[30]: register(c12);
float4 LightColor[30]: register(c42);

struct VS_INPUT
{
        float4 position: POSITION0;
};

struct VS_OUTPUT
{
        float4 P: POSITION;
        float4 LightColor: TEXCOORD0;
        float4 LightDirection: TEXCOORD1;
        float4 textureCoords: TEXCOORD2;
};

VS_OUTPUT main(VS_INPUT In)
{
        VS_OUTPUT Out;
        float4 LD = LightDir[In.position.w];
        float4 LC = LightColor[In.position.w];

        Out.P = float4(In.position.xyz, 1.0);
        Out.LightColor = LC;
        Out.LightDirection = mul(mNormal, LD);
        Out.textureCoords = Out.P;

        return Out;
}
