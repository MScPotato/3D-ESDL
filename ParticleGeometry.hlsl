static const float4 g_positions[4] =
{
    float4(-1, 1, 0, 0),
    float4(1, 1, 0, 0),
    float4(-1, -1, 0, 0),
    float4(1, -1, 0, 0),
};

static const float2 g_texcoords[4] =
{
    float2(0, 1),
    float2(1, 1),
    float2(0, 0),
    float2(1, 0),
};

struct VS_OUTPUT
{
    float3 position : Position;
    float4x4 world : World;
    float4x4 view : View;
    float4x4 projection : Projection;
};

struct GS_OUTPUT
{
    float4 position : Position;
    float2 texCoord : texCoord;
};

[maxvertexcount(4)]
void GS_main(point VS_OUTPUT input[1],
inout TriangleStream<GS_OUTPUT> output)
{
    GS_OUTPUT element;

    float4 viewPosition = mul(input[0].world, float4(input[0].position, 1.0f));
    viewPosition = mul(input[0].view, viewPosition);
  
    viewPosition = mul(input[0].projection, viewPosition);
    element.position = viewPosition;
    element.texCoord = float2(0, 0);
    output.Append(element);
  
    //for (int i = 0; i < 4; i++)
    //{
    //    element.position = mul(input[0].projection, viewPosition + g_positions[i]);
    //    element.texCoord = g_texcoords[i];
    //    output.Append(element);
    //}
   
    output.RestartStrip();

}