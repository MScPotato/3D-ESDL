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
    float3 camPos; //camerans pos
};

//cbuffer Light_Pos_Buffer : register(b2)
//{
//    float4 lightPos[6]; //not in use
//};

//cbuffer Light_Color_Buffer : register(b3)
//{
//    float4 lightRGBA[6]; //not in use
//};

cbuffer Shadow_buffer : register(b4)
{
    float3 shadowPos; //ljusets pos
};

cbuffer lightconst : register(b5)
{
    float4x4 lightView;
    float4x4 lightProjection;
};

struct VS_OUT
{
    float4 Pos : SV_POSITION;   // Pos in camera pov
    float4 LightPos : POSITION; // Pos in light's pov
    float2 TexCoord : TEXCOORD;
};

float ShadowCalc(float4 lightPos)
{
    float3 projCoord = lightPos.xyz / lightPos.w; //[-1, 1]

    projCoord.x = projCoord.x * 0.5f + 0.5f; //[0, 1]
    projCoord.y = -projCoord.y * 0.5f + 0.5f; //[0, 1]
   
    if (projCoord.x < 0.0f || projCoord.x > 1.0f || projCoord.y < 0.0f || projCoord.y > 1.0f)
    {
        return 0.0f;
    }

    float closestDepth = texture5.Sample(SamplerClamp, projCoord.xy).x;
    
    float currentDepth = projCoord.z;
    float bias = 0.00005f;
    if (currentDepth - bias  > closestDepth)
    {
        return 1.0f;
    }
    else
    {
        return 0.0f;
    }
}

float4 PS_main(VS_OUT input) : SV_Target
{
    float3 normal = texture0.Sample(SamplerWrap, input.TexCoord).xyz;
    float3 color = texture1.Sample(SamplerWrap, input.TexCoord).xyz;
    float4 position = texture2.Sample(SamplerWrap, input.TexCoord);
    float4 material = texture3.Sample(SamplerWrap, input.TexCoord); // specular
    float4 camera = texture4.Sample(SamplerWrap, input.TexCoord); // shadowmap cam
    float4 shadowMap = texture5.Sample(SamplerClamp, input.TexCoord); // shadowmap cam

    float4 currPos;
    currPos = mul(lightView, position);
    currPos = mul(lightProjection, currPos);

    if (color.x == 0.2 && color.y == 0.3 && color.z == 0.3)
    {
        return float4(color, 1.0);
    }

    float3 ambient = color.xyz * material.xyz * 0.5f;
    float3 finalColor = { 0, 0, 0 };
    float3 diffuse = { 0, 0, 0 };
    float3 specular = { 0, 0, 0 };
    float3 Rm;
    
    float3 VecToCam = normalize(camPos.xyz - position.xyz);
    float3 lightColor = float3(1.f, 1.f, 1.f);

    float3 vecToLight = shadowPos - position.xyz;
    
    float lengthToLight = length(vecToLight);

    vecToLight = normalize(vecToLight);
    
    diffuse = lightColor / lengthToLight * max(dot(normal, vecToLight), 0);
         
    float3 halfwayDir = normalize(vecToLight + VecToCam);
    specular = lightColor * pow(max(dot(normal, halfwayDir), 0), material.w);
    
    float shadow = 1.f - ShadowCalc(currPos);
    
    finalColor = (ambient + (shadow) * (diffuse + specular)) * color;
    
    return float4(finalColor, 1.f);

    
    //float3 normal = texture0.Sample(Sampler, input.TexCoord).xyz;
    //float3 color = texture1.Sample(Sampler, input.TexCoord).xyz;
    //float3 position = texture2.Sample(Sampler, input.TexCoord).xyz;
    //float4 material = texture3.Sample(Sampler, input.TexCoord); // specular

    //if (color.x == 0.2 && color.y == 0.3 && color.z == 0.3)
    //{
    //    return float4(color, 1.0);
    //}
    
    //float3 ambient = material.xyz * float3(0.3, 0.3, 0.3);
    //float3 finalColor = color * ambient;
    //float3 diffuse = { 0, 0, 0 };
    //float3 specular = { 0, 0, 0 };
    //float3 Rm;
    
    //float3 VecToCam = normalize(camPos.xyz - position.xyz);

    //for (int i = 0; i < 6; i++)
    //{
    //    float3 vecToLight = normalize(lightPos[i].xyz - position.xyz);
    //    float3 lightColor = lightRGBA[i].rgb;
    //    float lightIntensity = lightRGBA[i].a;

    //    diffuse = color * lightColor * lightIntensity * max(dot(normal, vecToLight), 0);
    //    Rm = 2 * max(dot(vecToLight, normal), 0) * (normal - vecToLight);
    //    specular = color * lightColor * pow(max(dot(Rm, VecToCam), 0), material.w);

    //    finalColor += diffuse + specular;
    //}
};

  
