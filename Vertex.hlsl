cbuffer CONSTANT_BUFFER : register(b0)
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;
};

struct VS_IN
{
	float3 Pos : POSITION;
	float2 TexCoord : TEXCOORD;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	//float4 wPos : POSITION;
	float2 TexCoord : TEXCOORD;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	
	output.Pos = float4(input.Pos, 1);
	//output.Pos = mul(world, output.Pos);
    //output.wPos = mul(world, output.Pos);

	output.TexCoord = input.TexCoord;

	return output;
}
