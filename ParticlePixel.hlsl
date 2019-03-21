Texture2D particleTex : register(t0); // texture of chosen particle
SamplerState SamplerClamp : register(s0); // sample with clamp

struct GS_OUT
{
    float4 Pos : SV_POSITION;
    float4 wPos : POSITION;
    float2 TexCoord : TEXCOORD;
};

float4 PS_main(GS_OUT input) : SV_Target
{
    //float3 texture = particleTex.Sample(SamplerClamp, TexCoord).xyz;
    return float4(1.f, 1.f, 1.f, 1.f); // white for just rain
}