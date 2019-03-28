cbuffer CONSTANT_BUFFER : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 projection;
};

struct Particle
{
    float3 position;
    float3 direction;
    float time;
};

StructuredBuffer<Particle> SimulationState;

struct VS_INPUT
{
    uint vertexID : SV_VertexID;
};

struct VS_OUTPUT
{
    float3 position : Position;
    float4x4 world : World;
    float4x4 view : View;
    float4x4 projection : Projection;
};

VS_OUTPUT VS_main(in VS_INPUT input)
{
    VS_OUTPUT output;

    output.position.xyz = SimulationState[input.vertexID].position;
    output.world = world;
    output.view = view;
    output.projection = projection;

    return output;
}