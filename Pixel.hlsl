Texture2D txDiff : register(t0);
SamplerState Sampler : register (s0);

cbuffer MTL_BUFFER : register(b0)
{
	float4 Kd;
	float4 Ka;
	float4 Tf;
	float4 Ks;
	float4 NiNsIl; //x = Ni, y = Ns, z = illum, w = 1(for now)
	
	//float Ni;
	//float Ns;
	//float illum;
	//float tmp;
};


cbuffer Light_Pos_Buffer : register(b2)
{
    float4 lightPos[6];
    //float4 lightA;
    //float4 lightB;
    //float4 lightC;
};

cbuffer Light_Color_Buffer : register(b3)
{
    float4 lightRGBA[6];
};

struct GS_OUT
{
    float4 Pos : SV_POSITION;
    float4 wPos : POSITION;
   // float3 camPos : CAMPOSITION;
    float2 TexCoord : TEXCOORD;
    float3 normal : NORMAL;
};

float4 PS_main(GS_OUT input) : SV_Target
{
	float3 texColor = txDiff.Sample(Sampler, input.TexCoord).xyz;
    float3 vecToLight;// = normalize(lightPos.xyz - input.wPos.xyz);
    //float3 VecToCam = normalize(input.camPos.xyz - input.wPos.xyz);

    float3 Ia = { 1, 1, 1 };
	//float3 lightColor = lightRGBA.rgb;
    //float lightIntesity = lightRGBA.a;
    float3 lightColor;
    float lightIntensity;
	float3 ambient = texColor * Ka.xyz * Ia;
	float3 diffuse;
    float3 specular = { 0, 0, 0 };
    float3 finalColor = texColor * Ia;
    float3 Rm;
	for (int i = 0; i < 6; i++)
	{
        vecToLight = normalize(lightPos[i].xyz - input.wPos.xyz);
        lightColor = lightRGBA[i].rgb;
        lightIntensity = lightRGBA[i].a;
        diffuse = texColor * Kd.xyz * lightColor * lightIntensity * max(dot(input.normal, vecToLight), 0);
        Rm = 2 * max(dot(vecToLight, input.normal), 0) * (input.normal - vecToLight);
      //  specular = texColor * lightColor * pow(max(dot(Rm, VecToCam), 0), NiNsIl.y);
		finalColor += diffuse + specular;
	}
    //finalColor += ambient;
	
	//float3 finalColor = ambient + texColor * Ia + texColor * Ia;
	return float4(finalColor, 1.0f);

   

	//return float4(input.TexCoord, 1.0f);
	//if (input.normal.x == -1 || input.normal.y == -1 || input.normal.z == -1)
	//{
	//	if (input.normal.x == -1)
	//	{
	//		return float4(0.f, input.normal.y + 1.f, input.normal.z + 1.f, 1.f);
	//	}
	//	else if (input.normal.y == -1)
	//	{
	//		return float4(input.normal.x + 1.f, 0.f, input.normal.z + 1.f, 1.f);
	//	}
	//	else// if (input.normal.z == -1)
	//	{
	//		return float4(input.normal.x + 1.f, input.normal.y + 1.f, 0.f, 1.f);
	//	}
	//}
	//else
	//	return float4(input.normal, 1.0f);
	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
};