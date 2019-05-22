static const float4 g_positions[4] =
{
    float4(-1, 1, 0, 0),
    float4(1, 1, 0, 0),
    float4(-1, -1, 0, 0),
    float4(1, -1, 0, 0),
};

cbuffer Camera_Buffer : register(b0)
{
    float3 camPos;
};

static const float2 g_texcoords[4] =
{
    float2(0, 0),
    float2(1, 0),
    float2(0, 1),
    float2(1, 1),
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    //float4 wPos : POSITION;

    float4x4 world : World;
    float4x4 view : View;
    float4x4 projection : Projection;
};

struct GS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 wPos : POSITION;
    float2 texCoord : texCoord;
};

[maxvertexcount(4)]
void GS_main(point VS_OUTPUT input[1],
inout TriangleStream<GS_OUTPUT> output)
{
    GS_OUTPUT element;

    float3 normal = normalize(camPos - input[0].pos.xyz);
    float3 vecRight = normalize(cross(normal, float3(0.f, 1.f, 0.f)));
    float3 vecUp = normalize(cross(vecRight, normal));

    float size = 0.25f;
    
    //top left
    float4 tlPos = float4(input[0].pos.xyz - (size * vecRight) + (size * vecUp), 1.f);
    //top right
    float4 trPos = float4(input[0].pos.xyz + (size * vecRight) + (size * vecUp), 1.f);
    //bottom left
    float4 blPos = float4(input[0].pos.xyz - (size * vecRight) - (size * vecUp), 1.f);
    //bottom right
    float4 brPos = float4(input[0].pos.xyz + (size * vecRight) - (size * vecUp), 1.f);
    

    //top left
    element.pos = mul(input[0].world, tlPos);
    element.wPos = element.pos;
    element.pos = mul(input[0].view, element.pos);
    element.pos = mul(input[0].projection, element.pos);
    element.texCoord = g_texcoords[0];
    output.Append(element);
    //top right
    element.pos = mul(input[0].world, trPos);
    element.wPos = element.pos;
    element.pos = mul(input[0].view, element.pos);
    element.pos = mul(input[0].projection, element.pos);
    element.texCoord = g_texcoords[1];
    output.Append(element);
    //bottom left
    element.pos = mul(input[0].world, blPos);
    element.wPos = element.pos;
    element.pos = mul(input[0].view, element.pos);
    element.pos = mul(input[0].projection, element.pos);
    element.texCoord = g_texcoords[2];
    output.Append(element);
    //bottom right
    element.pos = mul(input[0].world, brPos);
    element.wPos = element.pos;
    element.pos = mul(input[0].view, element.pos);
    element.pos = mul(input[0].projection, element.pos);
    element.texCoord = g_texcoords[3];
    output.Append(element);      

    //element.pos = mul(input[0].projection, element.pos);
    //element.texCoord = float2(0, 0);
   // output.Append(element);
  
    //for (int i = 0; i < 4; i++)
    //{
    //    //element.pos = mul(input[0].projection, element.pos + g_positions[i]);
    //    element.texCoord = g_texcoords[i];
    //    output.Append(element);
    //}
   
    //output.RestartStrip();

}