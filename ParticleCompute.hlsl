Texture2D<float> InputTex : register(t0);
RWTexture2D<float> OutputTex : register(u0);

#define size_x 20;
#define size_y 20;

[numthreads(32, 16, 1)]
void CS_main( uint3 dispatchThreadID : SV_DispatchThreadID )
{
    int3 textureLoc = int3(0, 0, 0);
    textureLoc.x = dispatchThreadID.x;
    textureLoc.y = dispatchThreadID.y;

    float value = InputTex.Load(textureLoc); // each thread reads fro munique location form thetexture!
    OutputTex[dispatchThreadID.xy] = 2.0f * value; // each thread writes to a unique location!



}