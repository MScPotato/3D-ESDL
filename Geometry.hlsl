cbuffer Camera_Buffer : register(b0)
{
    float3 camPos;
};

struct VS_OUT
{
    float4 Pos : SV_POSITION;
    float4 wPos : POSITION;
    float2 TexCoord : TEXCOORD;
    float3 normal : NORMAL;
};

struct GS_OUT
{
    float4 Pos : SV_POSITION;
    float4 wPos : POSITION;
    float3 camPos : CAMPOSITION;
    float2 TexCoord : TEXCOORD;
    float3 normal : NORMAL;
};

[maxvertexcount(3)]
void GS_main(
	triangle VS_OUT input[3] : SV_POSITION, 
	inout TriangleStream< GS_OUT > output
)
{
    GS_OUT element;

    for (uint i = 0; i < 3; i++)
    {
        if (dot(normalize(/*float3(0, 0, -2)*/camPos - input[i].wPos.xyz), input[i].normal) >= 0)
        {
            element.Pos = input[i].Pos;
            element.wPos = input[i].wPos;
            element.camPos = camPos;
            element.TexCoord = input[i].TexCoord;
            element.normal = input[i].normal;
            output.Append(element);
        }
    }
}