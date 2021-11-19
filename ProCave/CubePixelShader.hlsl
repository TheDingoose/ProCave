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
    float NormalSampleDistance;
    float3 Padding;
};

Texture2D shaderTexture;
SamplerState SampleType;

struct GS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Normal : NORMAL;
    float4 Color : COLOR;
};
//float3(0.92f, 0.45f, 0.15f)
float4 main(GS_OUTPUT input) : SV_TARGET
{
 
    float LightAngleStrength = dot(normalize(((input.Color)) - (PlayerPos)).xyz, input.Normal.xyz);
    LightAngleStrength = ((LightAngleStrength + 1.f) * 0.5f);
    //LightAngleStrength = min(max(LightAngleStrength, 0.f), 1.f);
    
    float Distance = distance(PlayerPos, input.Color);
    
    Distance = ((LightStrength - Distance) / LightStrength);
    
    float3 Clr = float3(shaderTexture.Sample(SampleType, input.Color.yz).x * abs(input.Normal.x), shaderTexture.Sample(SampleType, input.Color.xz).y * abs(input.Normal.y), shaderTexture.Sample(SampleType, input.Color.xy).z * abs(input.Normal.z)) * 2 * (LightAngleStrength * Distance);
    //float3 Clr = float3(0.92f, 0.45f, 0.15f) * (LightAngleStrength * Distance);
    
    return float4(Clr, 1.f);
}