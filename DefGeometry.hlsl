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
    float2 TexCoord : TEXCOORD;
    float3 normal : NORMAL;
    //float bfTest : BFTEST;
};

[maxvertexcount(3)]
void GS_main(
	triangle VS_OUT input[3] : SV_POSITION,
	inout TriangleStream<GS_OUT> output
)
{
    GS_OUT element;
    
    float3 vec1 = input[1].wPos.xyz - input[0].wPos.xyz;
    float3 vec2 = input[2].wPos.xyz - input[0].wPos.xyz;
    float3 normal = cross(vec1, vec2);
 
	for (uint i = 0; i < 3; i++)
    {
        if (dot(normalize(/*float3(0, 4.9, -2)*/camPos - input[i].wPos.xyz), normal) > 0.f)
        {
            element.Pos = input[i].Pos;
            element.wPos = input[i].wPos;
            element.TexCoord = input[i].TexCoord;
            element.normal = input[i].normal;
            //element.bfTest = 1;
            output.Append(element);
        }
        //else
        //{
        //    element.Pos = input[i].Pos;
        //    element.wPos = input[i].wPos;
        //    element.TexCoord = input[i].TexCoord;
        //    element.normal = input[i].normal;
        //    element.bfTest = 0;
        //    output.Append(element);
        //}
    }
}