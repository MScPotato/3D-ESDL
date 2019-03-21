struct VS_OUT
{
    float4 Pos : SV_POSITION;
    float4 wPos : POSITION;
    float2 TexCoord : TEXCOORD;
};

struct GS_OUT
{
    float4 Pos : SV_POSITION;
    float4 wPos : POSITION;
    float2 TexCoord : TEXCOORD;
};

[maxvertexcount(3)]
void GS_main(
	triangle VS_OUT input[3] : SV_POSITION,
	inout TriangleStream<GS_OUT> output
)
{
    GS_OUT element;

	for (uint i = 0; i < 3; i++)
	{
		element.wPos = input[i].wPos;
        element.Pos = input[i].Pos;
        element.TexCoord = input[i].TexCoord;
		output.Append(element);
	}
}