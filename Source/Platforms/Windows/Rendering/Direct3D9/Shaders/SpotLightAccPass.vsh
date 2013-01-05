float4x4 mViewProjection: register(c0);
float4x4 mNormal: register(c4);
float4x4 mView: register(c8);
float4 LightPos[30]: register(c12);
float4 LightDir[30]: register(c42);
float4 LightColor[30]: register(c72);

struct VS_INPUT
{
        float4 position: POSITION0;
};

struct VS_OUTPUT
{
        float4 P: POSITION;
        float4 LightColor: TEXCOORD0;
        float4 LightPosition: TEXCOORD1;
        float4 LightDirection: TEXCOORD2;
        float4 textureCoords: TEXCOORD3;
};

VS_OUTPUT main(VS_INPUT In)
{
        VS_OUTPUT Out;
        float4 LP = LightPos[In.position.w];
        float4 LD = LightDir[In.position.w];
        float4 LC = LightColor[In.position.w];

        float3 ZNew = normalize(LD.xyz);
        float3 XNew, YNew;
        if(abs(ZNew.x) < 0.001 && abs(ZNew.y) < 0.001)
                XNew = cross(float3(0.0, 1.0, 0.0), ZNew);
        else
                XNew = cross(float3(0.0, 0.0, 1.0), ZNew);
        YNew = cross(ZNew, XNew);

        float Factor = (1.0 - LD.w);
        Factor = Factor * Factor;
        Factor = 1.0 / Factor;
        Out.LightDirection = float4(mul(mNormal, -ZNew).xyz, Factor);
        float3x3 ConversionMat = {XNew, YNew, ZNew};

        float3 p = float3(In.position.xy, 0.0);
        float3 pNew = mul(p, ConversionMat);

        p = LP.xyz + LD.xyz * In.position.z + pNew * LP.w * 1.268;

        Out.P = mul(mViewProjection, float4(p, 1.0));
        Out.LightColor = LC;
        Out.LightPosition = mul(mView, float4(LP.xyz, 1.0));
        Out.LightPosition.w = 1.0 / dot(LD.xyz, LD.xyz);
        Out.textureCoords = Out.P;

        return Out;
}
