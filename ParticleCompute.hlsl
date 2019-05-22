
#define size_x 100
#define size_y 1
#define size_z 1

struct posStruct
{
    float3 position;
};

//Texture2D<float3> InputTex;
RWStructuredBuffer<posStruct> OutputTex;


[numthreads(size_x, size_y, size_z)]
void CS_main(uint3 DispatchThreadID : SV_DispatchThreadID)
{
    uint myID = DispatchThreadID.x; // all variations are in x, x will be 0 .. 1000
        
    posStruct p = OutputTex[myID];

    // Particles has a constant value of (0, 4.9, 0) (in front of the camera) for now.

    p.position.y -= 0.00075f;
    if (p.position.y < 2.85f)
    {
        p.position.y = 20.f;
    }

    p.position.x -= 0.00025f;
    
    if (p.position.x < -31.f)
    {
        p.position.x = 31.f;
    }

    OutputTex[myID] = p;
    
}