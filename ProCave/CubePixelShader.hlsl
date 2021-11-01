cbuffer cbPerObject
{
    float4x4 WVP;
    float4 PlayerPos;
    float4 SampleMod;
    float4 SampleOffset;
    float CubeSize;
    float LightStrength;
    float Time;
    float DensityLimit;
};

struct GS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float4 Normal : NORMAL;
    
};

float4 main(GS_OUTPUT input) : SV_TARGET
{
 
    float LightAngleStrength = dot(((input.Color) - PlayerPos).xyz, input.Normal.xyz);
    LightAngleStrength = min(max(LightAngleStrength, 0.f) + 0.1f , 1.f);
    
    float Distance = distance(PlayerPos, input.Color);
    
    Distance = ((LightStrength - Distance) / LightStrength);
    
    
    float3 Clr = float3(0.92f, 0.45f, 0.15f) * LightAngleStrength * Distance;
    
    return float4(Clr, 1.f);
}