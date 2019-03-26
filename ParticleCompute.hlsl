
#define size_x 512
#define size_y 1
#define size_z 1

struct posStruct
{
    float3 pos;
};

//Texture2D<float3> InputTex;
RWStructuredBuffer<posStruct> OutputTex;

[numthreads(size_x, size_y, size_z)]
void CS_main(uint3 DispatchThreadID : SV_DispatchThreadID)
{
    
}