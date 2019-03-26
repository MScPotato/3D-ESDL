// Following code provided from Practical Rendering and Computation with Direct3d 11
cbuffer ParticleRenderParameters
{
    float4 EmitterLocation;
    float4 ConsumerLocation;
};

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

[maxvertexcount(4)]
void GS_main( /*point GS_INPUT input[1], inout TriangleStream<PS_INPUT>
SpriteStream*/)
{
    //PS_INPUT output;

    //float dist = saturate(length(input[0].position - ConsumerLocation.xyz)
    //                                                    / 100.0f);
    //float4 color = float4(0.2f, 0.2f, 1.0f, 0.0f) * (dist)
    //              + float4(1.0f, 0.2f, 0.2f, 0.0f) * (1.0f - dist);

    //// Transform to view space
    //float4 viewposition = mul(float4(input[0].position, 1.0f),
    //                            WorldViewMatrix);

    //// Emit two new triangles
    //for (int i = 0; i < 4; i++)
    //{
    //    // Transform to clip space
    //    output.position = mul(viewposition + g_positions[i], ProjMatrix);
    //    output.texcoords = g_texcoords[i];
    //    output.color = color;
    //    SpriteStream.Append(output);
    //}

    //SpriteStream.RestartStrip();
}