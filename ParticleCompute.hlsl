
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
    uint myID = DispatchThreadID.x 
    + DispatchThreadID.y * size_x
    + DispatchThreadID.z * size_x * size_x;

        
    posStruct p = OutputTex[myID];

    p.position.y -= 0.1f;

    if (p.position.y < 0.0f)
    {
        p.position.y = 25.f;
    }

    OutputTex[myID] = p;
    
}