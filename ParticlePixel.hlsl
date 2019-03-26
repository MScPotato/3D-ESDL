// Following code provided from Practical Rendering and Computation with Direct3d 11
Texture2D ParticleTexture : register(t0);
SamplerState LinearSampler : register(s0);

struct PS_INPUT
{
    int pos;
};

float4 PS_main(in PS_INPUT input) : SV_Target
{
    //float4 color = ParticleTexture.Sample(LinearSampler, input.texcoords);
    //color = color * input.color;

    //return (color);
    return float4(1, 1, 1, 1);
}