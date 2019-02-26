Texture2D texture0 : register(t0);//normals
Texture2D texture1 : register(t1);//diffuse
Texture2D texture2 : register(t2);//pos
Texture2D texture3 : register(t3);//ljussak

SamplerState Sampler : register (s0);

cbuffer MTL_BUFFER : register(b0)
{
	float4 Kd;
	float4 Ka;
	float4 Tf;
	float4 Ks;
	float4 NiNsIl; //x = Ni, y = Ns, z = illum, w = 1(for now)
};

cbuffer Camera_Buffer : register(b1)
{
    float3 camPos;
};

cbuffer Light_Pos_Buffer : register(b2)
{
    float4 lightPos[6];
};

cbuffer Light_Color_Buffer : register(b3)
{
    float4 lightRGBA[6];
};

struct VS_OUT
{
    float4 Pos : SV_POSITION;
    //float4 wPos : POSITION;
   // float3 camPos : CAMPOSITION;
    float2 TexCoord : TEXCOORD;
};

float4 PS_main(VS_OUT input) : SV_Target
{
    float3 normal = texture0.Sample(Sampler, input.TexCoord).xyz;
    float3 color = texture1.Sample(Sampler, input.TexCoord).xyz;
    float3 position = texture2.Sample(Sampler, input.TexCoord).xyz;
    float3 retard = texture3.Sample(Sampler, input.TexCoord).xyz; // till shadowmapping

    if ((normal == NULL) || (normal == NULL) || (normal == NULL) || (normal == NULL))
    {

    }

    if (color.x == 0.2 && color.y == 0.3 && color.z == 0.3)
    {
        return float4(color, 1.0);
    }
    
    float3 vecToLight;
    float3 VecToCam = normalize(camPos.xyz - position.xyz);

    float3 Ia = { 0, 0, 0 };
    float3 lightColor;
    float lightIntensity;
    float3 diffuse;
    float3 ambient = Ka.xyz * Ia;
    float3 specular = { 0, 0, 0 };
    float3 finalColor = color * Ia;
    float3 Rm;
    for (int i = 0; i < 6; i++)
    {
        vecToLight = normalize(lightPos[i].xyz - position.xyz);
        lightColor = lightRGBA[i].rgb;
        lightIntensity = lightRGBA[i].a;
        diffuse = color * Kd.xyz * lightColor * lightIntensity * max(dot(normal, vecToLight), 0);
        Rm = 2 * max(dot(vecToLight, normal), 0) * (normal - vecToLight);
        specular = color * lightColor * pow(max(dot(Rm, VecToCam), 0), NiNsIl.y);
        finalColor += diffuse + specular;
    }

    return float4(finalColor, 1.0);
};
  