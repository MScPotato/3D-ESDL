// Following code provided from Practical Rendering and Computation with Direct3d 11
Texture2D ParticleTexture : register(t0);
SamplerState Sampler : register(s0);

struct PS_INPUT
{
    float3 position : Position;
    float2 texCoord : texCoord;
};

float4 PS_main(in PS_INPUT input) : SV_Target
{
    float4 finalColor = float4(1.f, 0.f, 0.f, 1.f);
    return finalColor;
}