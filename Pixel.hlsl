Texture2D texture0 : register(t0); // Normals
Texture2D texture1 : register(t1); // Diffuse/color
Texture2D texture2 : register(t2); // Position
Texture2D texture3 : register(t3); // material
Texture2D texture4 : register(t4); // shadowmap camera
Texture2D texture5 : register(t5); // shadowmap sun

SamplerState SamplerWrap : register (s0);
SamplerState SamplerClamp : register(s1);

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

cbuffer Shadow_buffer : register(b4)
{
    float3 shadowPos;
};

struct VS_OUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

float4 PS_main(VS_OUT input) : SV_Target
{
    float3 normal = texture0.Sample(SamplerWrap, input.TexCoord).xyz;
    float3 color = texture1.Sample(SamplerWrap, input.TexCoord).xyz;
    float4 position = texture2.Sample(SamplerWrap, input.TexCoord);
    float4 material = texture3.Sample(SamplerWrap, input.TexCoord); // specular
    float4 camera = texture4.Sample(SamplerWrap, input.TexCoord); // shadowmap cam

    if (color.x == 0.2 && color.y == 0.3 && color.z == 0.3)
    {
        return float4(color, 1.0);
    }

    float3 ambient = material.xyz /* * 0.5*/;
    float3 finalColor = color * ambient;
    float3 diffuse = { 0, 0, 0 };
    float3 specular = { 0, 0, 0 };
    float3 Rm;
    
    float3 VecToCam = normalize(camPos.xyz - position.xyz);

    float3 lightColor = { 1.f, 0.f, 0.f };

    float2 projCoords;
    projCoords.x = (camera.x / camera.w) * 0.5f + 0.5f;
    projCoords.y = (-camera.y / camera.w) * 0.5f + 0.5f;

    if (saturate(projCoords.x) == projCoords.x && saturate(projCoords.y) == projCoords.y)
    {
        float shadow = texture5.Sample(SamplerClamp, projCoords).x; // shadowmap sun

        float lightDV = camera.z / camera.w;
        lightDV -= 0.1f;

        if (lightDV < shadow)
        {
            float3 vecToLight = normalize(shadowPos - position.xyz);
            float lightIntensity = 0.f;

            if (dot(normalize(normal), normalize(vecToLight)) > 0)
            {
                lightIntensity = 1.f;
            }

            diffuse = color * lightColor * lightIntensity;// * max(dot(normal, vecToLight), 0);
            //Rm = 2 * max(dot(vecToLight, normal), 0) * (normal - vecToLight);
            //specular = color * pow(max(dot(Rm, VecToCam), 0), material.w);

            finalColor += diffuse + specular;
        }
    }
       
    return float4(finalColor, 1.f);
};
  
