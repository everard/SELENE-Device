float4x4 mWorldViewProjection: register(c0);
float4 Rotations[100]: register(c12);
float4 Positions[100]: register(c112);

struct VS_INPUT
{
        float4 position: POSITION0;
        float2 textureCoords: TEXCOORD0;
        float4 indices: TEXCOORD1;
        float4 weights: TEXCOORD2;
};

struct VS_OUTPUT
{
        float4 P: POSITION;
        float4 pos : TEXCOORD0;
        float2 textureCoords: TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT In)
{
        VS_OUTPUT Out;
        float3 P = float3(0.0, 0.0, 0.0);

        P += In.weights.x * Transform(Positions[In.indices.x], Rotations[In.indices.x], In.position);
        P += In.weights.y * Transform(Positions[In.indices.y], Rotations[In.indices.y], In.position);
        P += In.weights.z * Transform(Positions[In.indices.z], Rotations[In.indices.z], In.position);
        P += In.weights.w * Transform(Positions[In.indices.w], Rotations[In.indices.w], In.position);

        Out.P = mul(mWorldViewProjection, float4(P, 1.0));
        Out.pos = Out.P;
        Out.textureCoords = In.textureCoords;

        return Out;
}
