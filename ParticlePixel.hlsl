// Following code provided from Practical Rendering and Computation with Direct3d 11
Texture2D ParticleTexture : register(t0);
SamplerState Sampler : register(s0);

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 wPos : POSITION;
    float2 texCoord : texCoord;
};

float4 PS_main(in PS_INPUT input) : SV_Target
{
    float4 finalColor = float4(ParticleTexture.Sample(Sampler, input.texCoord));
    if (finalColor.w == 0.f)
        discard; // we discard the color for the transparent pixels
    return finalColor;
}

