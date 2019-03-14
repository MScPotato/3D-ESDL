cbuffer CONSTANT_BUFFER : register(b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;
};

cbuffer lightconst : register(b1)
{
    float4x4 lightView;
    float4x4 lightProjection;
};

struct VS_IN
{
	float3 Pos : POSITION;
	float2 TexCoord : TEXCOORD;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float4 LightPos : POSITION;
	float2 TexCoord : TEXCOORD;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	
	output.Pos = float4(input.Pos, 1);
	//output.Pos = mul(world, output.Pos);
    //output.wPos = output.Pos;
    //output.Pos = mul(view, output.Pos);
    //output.Pos = mul(projection, output.Pos);

    //output.LightPos = mul(world, output.Pos);
    output.LightPos = mul(lightView, output.LightPos);
    output.LightPos = mul(lightProjection, output.LightPos);
    
    output.TexCoord = input.TexCoord;

	return output;
}
