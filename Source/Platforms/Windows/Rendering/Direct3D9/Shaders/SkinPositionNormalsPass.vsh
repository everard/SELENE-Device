float4x4 mWorldViewProjection: register(c0);
float4x4 mWorldView: register(c4);
float4x4 mNormal: register(c8);
float4 Rotations[100]: register(c12);
float4 Positions[100]: register(c112);

struct VS_INPUT
{
        float3 position:      POSITION0;
        float3 normal:        NORMAL0;
        float4 tangent:       TANGENT0;
        float2 textureCoords: TEXCOORD0;
        float4 indices:       TEXCOORD1;
        float4 weights:       TEXCOORD2;
};

struct VS_OUTPUT
{
        float4 P:     POSITION;
        float4 TCNXY: TEXCOORD0;
        float4 NZT:   TEXCOORD1;
        float3 B:     TEXCOORD2;
        float4 pos:   TEXCOORD3;
};

VS_OUTPUT main(VS_INPUT In)
{
        VS_OUTPUT Out;

        float3 P = float3(0.0, 0.0, 0.0);
        float3 N = float3(0.0, 0.0, 0.0);
        float3 T = float3(0.0, 0.0, 0.0);

        P += In.weights.x * Transform(Positions[In.indices.x], Rotations[In.indices.x], In.position);
        P += In.weights.y * Transform(Positions[In.indices.y], Rotations[In.indices.y], In.position);
        P += In.weights.z * Transform(Positions[In.indices.z], Rotations[In.indices.z], In.position);
        P += In.weights.w * Transform(Positions[In.indices.w], Rotations[In.indices.w], In.position);

        N += In.weights.x * QuatRotate(In.normal, Rotations[In.indices.x]).xyz;
        T += In.weights.x * QuatRotate(In.tangent.xyz, Rotations[In.indices.x]).xyz;

        N += In.weights.y * QuatRotate(In.normal, Rotations[In.indices.y]).xyz;
        T += In.weights.y * QuatRotate(In.tangent.xyz, Rotations[In.indices.y]).xyz;

        N += In.weights.z * QuatRotate(In.normal, Rotations[In.indices.z]).xyz;
        T += In.weights.z * QuatRotate(In.tangent.xyz, Rotations[In.indices.z]).xyz;

        N += In.weights.w * QuatRotate(In.normal, Rotations[In.indices.w]).xyz;
        T += In.weights.w * QuatRotate(In.tangent.xyz, Rotations[In.indices.w]).xyz;

        Out.P = mul(mWorldViewProjection, float4(P, 1.0));
        Out.pos = mul(mWorldView, float4(P, 1.0));

        N = mul(mNormal, N);
        T = mul(mNormal, T);
        Out.B = In.tangent.w * cross(T, N);

        Out.TCNXY = float4(In.textureCoords.xy, N.xy);
        Out.NZT   = float4(N.z, T);

        return Out;
}
