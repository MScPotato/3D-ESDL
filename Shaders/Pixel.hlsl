Texture2D texture0 : register(t0); // Normals
Texture2D texture1 : register(t1); // Diffuse/color
Texture2D texture2 : register(t2); // Position
Texture2D texture3 : register(t3); // material
Texture2D texture4 : register(t4); // shadowmap camera
Texture2D texture5 : register(t5); // shadowmap sun

SamplerState Sampler : register (s0);

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
    float3 normal = texture0.Sample(Sampler, input.TexCoord).xyz;
    float3 color = texture1.Sample(Sampler, input.TexCoord).xyz;
    float4 position = texture2.Sample(Sampler, input.TexCoord);
    float4 material = texture3.Sample(Sampler, input.TexCoord); // specular
    float camera = texture4.Sample(Sampler, input.TexCoord).x; // shadowmap cam
    float shadow = texture5.Sample(Sampler, input.TexCoord).x; // shadowmap sun

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

    for (int i = 0; i < 1; i++)
    {
        float3 vecToLight = shadowPos - position.xyz;

        float lenghtToLight = length(vecToLight);
        float3 lightColor = { 1.f, 1.f, 1.f };
        float lightIntensity = 1;
        lenghtToLight = lenghtToLight;//fixa
        //if (lenghtToLight <= shadow)
        //{
        
        //    lightIntensity = 1;

        //}
        //else
        //{
        //    lightIntensity = 0;

        //}
        //diffuse = color * lightColor * lightIntensity * max(dot(normal, vecToLight), 0);
        //Rm = 2 * max(dot(vecToLight, normal), 0) * (normal - vecToLight);
        //specular = color * lightColor * pow(max(dot(Rm, VecToCam), 0), material.w);

        //finalColor += diffuse + specular;
    }

    //float3 vecToLight;
    //float3 VecToCam = normalize(camPos.xyz - position.xyz);
    //float3 Ia = { 0, 0, 0 };
    //float3 lightColor;
    //float lightIntensity;
    //float3 diffuse;
    //float3 ambient = Ka.xyz * Ia;
    //float3 specular = { 0, 0, 0 };
    //float3 finalColor = color * Ia;
    //float3 Rm;
    //for (int i = 0; i < 6; i++)
    //{
    //    vecToLight = normalize(lightPos[i].xyz - position.xyz);
    //    lightColor = lightRGBA[i].rgb;
    //    lightIntensity = lightRGBA[i].a;
    //    diffuse = color * Kd.xyz * lightColor * lightIntensity * max(dot(normal, vecToLight), 0);
    //    Rm = 2 * max(dot(vecToLight, normal), 0) * (normal - vecToLight);
    //    specular = color * lightColor * pow(max(dot(Rm, VecToCam), 0), NiNsIl.y);
    //    finalColor += diffuse + specular;
    //}

    //return float4(finalColor, 1.0);
    return float4(finalColor, 1.f);
};
  
