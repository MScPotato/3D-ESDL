//cbuffer ParticleRenderParameters
//{
//    float4 EmitterLocation;
//    float4 ConsumerLocation;
//};

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
    float3 position : Position;
    float2 texCoord : texCoord;
};

[maxvertexcount(4)]
void GS_main(point VS_OUTPUT input[1],
inout TriangleStream<GS_OUTPUT> output)
{
    GS_OUTPUT element;

    float4 viewPosition = mul(float4(input[0].position, 1.0f), input[0].world);
    viewPosition = mul(viewPosition, input[0].view);

    for (int i = 0; i < 4; i++)
    {
        element.position = mul(viewPosition + g_positions[i], input[0].projection);
        element.texCoord = g_texcoords[i];
        output.Append(element);
    }
    output.RestartStrip();

}