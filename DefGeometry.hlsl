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
    float3x3 TBN : TBNMATRIX;
    //float bfTest : BFTEST;
};

float3 calcTangent(float3 pos1, float3 pos2, float3 pos3, float2 uv1, float2 uv2, float2 uv3)
{
    float3 edge1 = pos2 - pos1;
    float3 edge2 = pos3 - pos1;
    float2 deltaUV1 = uv2 - uv1;
    float2 deltaUV2 = uv3 - uv1;

    float f = 1.f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    float3 tangent;
    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
   
    return normalize(tangent);
}

[maxvertexcount(3)]
void GS_main(
	triangle VS_OUT input[3] : SV_POSITION,
	inout TriangleStream<GS_OUT> output
)
{
    GS_OUT element;
    
    float3 vec1 = input[1].wPos.xyz - input[0].wPos.xyz;
    float3 vec2 = input[2].wPos.xyz - input[0].wPos.xyz;
    float3 normal = normalize(cross(vec1, vec2));
    
    float3 tangent = calcTangent(input[0].wPos.xyz, input[1].wPos.xyz, input[2].wPos.xyz, input[0].TexCoord, input[1].TexCoord, input[2].TexCoord);
    float3 bitangent = normalize(cross(tangent, normal));

    float3x3 TBN = float3x3(tangent, bitangent, normal);
 
	for (uint i = 0; i < 3; i++)
    {
        if (dot(normalize(/*float3(0, 4.9, -2)*/camPos - input[i].wPos.xyz), normal) > 0.f)
        {
            element.Pos = input[i].Pos;
            element.wPos = input[i].wPos;
            element.TexCoord = input[i].TexCoord;
            element.normal = input[i].normal;
            element.TBN = TBN;
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