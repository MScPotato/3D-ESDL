cbuffer CONSTANT_BUFFER : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 projection;
};

struct Particle
{
    float3 position;
};

StructuredBuffer<Particle> SimulationState : register(t0);

struct VS_INPUT
{
    uint id : SV_VertexID;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4x4 world : World;
    float4x4 view : View;
    float4x4 projection : Projection;
};

VS_OUTPUT VS_main(in VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.pos = float4(SimulationState[input.id].position, 1);
    output.world = world;
    output.view = view;
    output.projection = projection;

    return output;
}
