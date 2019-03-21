Texture2D txDiff : register(t0);
Texture2D norDiff : register(t1);
SamplerState Sampler : register(s0);

cbuffer MTL_BUFFER : register(b0)
{
    float4 Kd;
    float3 Ka;
    float Ns;
    //float4 Tf;
    //float4 Ks;
    //float4 NiNsIl; //x = Ni, y = Ns, z = illum, w = 1(for now)
};

struct GS_OUT
{
    float4 Pos : SV_POSITION;
    float4 wPos : POSITION;
    float2 TexCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3x3 TBN : TBNMATRIX; // tangent, bitangent, normal
    //float bfTest : BFTEST;
};

struct PS_OUT
{
    float4 normal       : SV_Target0;
    float4 diffuse      : SV_Target1;
    float4 pos          : SV_Target2;
    float4 material		: SV_Target3;
};

PS_OUT PS_main(GS_OUT input)
{
    
    PS_OUT output; 

    float3 normal = norDiff.Sample(Sampler, input.TexCoord).xyz;
    normal = normalize(normal * 2 - 1);
    normal = normalize(mul(normal, input.TBN));

    output.normal = float4(normal, 1.f);
    output.diffuse = float4(txDiff.Sample(Sampler, input.TexCoord).xyz * Kd.xyz, 1.0);
    output.pos = input.wPos;
    output.material = float4(Ka.rgb, Ns);
    
    return output;
};