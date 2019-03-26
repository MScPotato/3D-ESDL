// Following code provided from Practical Rendering and Computation with Direct3d 11
struct Particle
{
    float3 position;
    float3 direction;
    float time;
};

StructuredBuffer<Particle> SimulationState;

struct VS_INPUT
{
    uint vertexid : SV_VertexID;
};

struct VS_OUTPUT
{
    float3 position : Position;
};

VS_OUTPUT VS_main(in VS_INPUT input)
{
    VS_OUTPUT output;

    output.position.xyz = SimulationState[input.vertexid].position;

    return output;
}