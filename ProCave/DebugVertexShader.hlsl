cbuffer cbPerObject
{
    float4x4 WVP;
    float MarchCubeSize;
};


float4 main(float3 inPos : POSITION, float inColor : COLOR) : SV_POSITION
{
    float4 Out = float4(0.f, 0.f, 0.f, 1.f);
    
    Out.xyz = inPos;
    
    Out = mul(Out, WVP);
    Out.z = 0.f;
    return Out;
}