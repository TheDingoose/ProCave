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
    float TextureBlendOffset;
    float TextureBlendExponent;
    float Padding;
};

Texture2D shaderTexture;
SamplerState SampleType;

//Texture2D shaderTexture2;
//SamplerState SampleType2;

Texture2D shaderTextureN;
SamplerState SampleTypeN;

struct GS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Normal : NORMAL;
    float4 Color : COLOR;
};
//float3(0.92f, 0.45f, 0.15f)
float4 main(GS_OUTPUT input) : SV_TARGET
{
 
    float3 Weights = abs(input.Normal.xyz);
    Weights = saturate(Weights - TextureBlendOffset);
    Weights = pow(Weights, TextureBlendExponent);
    Weights = normalize(Weights);
    
    //float3 colores = abs(input.Normal);
    
    float3 Clr = shaderTexture.Sample(SampleType, input.Color.zy * 0.1f) * Weights.x;
          Clr += shaderTexture.Sample(SampleType, input.Color.xz * 0.1f) * Weights.y;
          Clr += shaderTexture.Sample(SampleType, input.Color.xy * 0.1f) * Weights.z;
    
    
    
    float3 tNormalX = (shaderTextureN.Sample(SampleTypeN, input.Color.zy * 0.1f).xyz * 2 - 1); //- 1.f; //* (input.Normal.x >= 0.f) ? -1.f : 1.f;
    float3 tNormalY = (shaderTextureN.Sample(SampleTypeN, input.Color.xz * 0.1f).xyz * 2 - 1);//- 1.f;//* (input.Normal.y >= 0.f) ? -1.f : 1.f;
    float3 tNormalZ = (shaderTextureN.Sample(SampleTypeN, input.Color.xy * 0.1f).xyz * 2 - 1);//
    -1.f; //* (input.Normal.z >= 0.f) ? -1.f : 1.f;

    float3 Flipper = sign(input.Normal.xyz);
    
    float3 NormalX = float3(0.f, tNormalX.yx);
    float3 NormalY = float3(tNormalY.x, 0.f, tNormalY.y);
    float3 NormalZ = float3(tNormalZ.xy, 0.f);
    
   // NormalX.z = 0; //Flipper.x;
   // NormalY.z = 0; //Flipper.y;
   // NormalZ.z = 0; //Flipper.z;
    
   // float3 norm = (NormalX.zyx * Weights.x) + (NormalY.xzy * Weights.y) + (NormalZ.xyz * Weights.z) + input.Normal.xyz;
    float3 norm = (NormalX.xyz * Weights.x) + (NormalY.xyz * Weights.y) + (NormalZ.xyz * Weights.z) + input.Normal.xyz;
    
    norm = normalize(norm.xyz);
    
    //norm = normalize((norm)); //* (dot(input.Normal.xyz, norm) < 0.f) ? -1.f : 1.f);
   
    
   // norm.y = norm.y * (input.Normal.y >= 0.f) ? 1.f : -1.f;
    
    float LightAngleStrength = clamp(dot(normalize((PlayerPos - input.Color).xyz).xyz, normalize(norm.xyz)), 0.f, 1.f);
    //float LightAngleStrength = clamp(dot(normalize(float3(-1, 0, 0)).xyz, norm.xyz), 0.f, 1.f);
    //LightAngleStrength += clamp(dot(normalize(float3(1, 0, 0)).xyz, norm.xyz), 0.f, 1.f);
   // LightAngleStrength = abs(LightAngleStrength);
    
    //LightAngleStrength = ((LightAngleStrength + 1.f) * 0.5f);
    //LightAngleStrength = min(max(LightAngleStrength, 0.f), 1.f);
    
    float Distance = distance(PlayerPos, input.Color);
    
    //float RealLightStrength = LightStrength * clamp(((abs(sin(Time * 40.f))) - abs(sin(Time * 12.1213f))+ 0.5), 0.8f, 1.f);
    //float RealLightStrength = LightStrength * clamp(((abs(sin(Time * 80.f))) - abs(sin(Time * 32.1213f))+ 0.5), 0.9f, 1.f);
    //float RealLightStrength = LightStrength * max(abs(sin(Time)), 0.1f);
    
    //Distance = ((RealLightStrength - Distance) / RealLightStrength);
    Distance = ((LightStrength - Distance) / LightStrength);
    
   // Clr = min(Clr, max(float3(150.f / 255.f, 50.f / 255.f, 30.f / 255.f) * (LightAngleStrength * Distance), float3(255.f / 255.f, 200.f / 255.f, 6.f / 255.f) * (LightAngleStrength * 0.3f * Distance)));
   Clr = (Clr * LightAngleStrength * Distance);
   //Clr = min(Clr * LightAngleStrength * Distance, float3(20.f / 255.f, 255.f / 255.f, 20.f / 255.f));
    
    
  //  Clr = Clr * 0.0001f + norm;
    
   // Clr = Clr * (LightAngleStrength); //* Distance);
    
    return float4(Clr, 1.f);
}