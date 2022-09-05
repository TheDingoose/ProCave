#define IDENTITY_MATRIX float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)

float4x4 inverse(float4x4 m)
{
    float n11 = m[0][0], n12 = m[1][0], n13 = m[2][0], n14 = m[3][0];
    float n21 = m[0][1], n22 = m[1][1], n23 = m[2][1], n24 = m[3][1];
    float n31 = m[0][2], n32 = m[1][2], n33 = m[2][2], n34 = m[3][2];
    float n41 = m[0][3], n42 = m[1][3], n43 = m[2][3], n44 = m[3][3];

    float t11 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44;
    float t12 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44;
    float t13 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44;
    float t14 = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;

    float det = n11 * t11 + n21 * t12 + n31 * t13 + n41 * t14;
    float idet = 1.0f / det;

    float4x4 ret;

    ret[0][0] = t11 * idet;
    ret[0][1] = (n24 * n33 * n41 - n23 * n34 * n41 - n24 * n31 * n43 + n21 * n34 * n43 + n23 * n31 * n44 - n21 * n33 * n44) * idet;
    ret[0][2] = (n22 * n34 * n41 - n24 * n32 * n41 + n24 * n31 * n42 - n21 * n34 * n42 - n22 * n31 * n44 + n21 * n32 * n44) * idet;
    ret[0][3] = (n23 * n32 * n41 - n22 * n33 * n41 - n23 * n31 * n42 + n21 * n33 * n42 + n22 * n31 * n43 - n21 * n32 * n43) * idet;

    ret[1][0] = t12 * idet;
    ret[1][1] = (n13 * n34 * n41 - n14 * n33 * n41 + n14 * n31 * n43 - n11 * n34 * n43 - n13 * n31 * n44 + n11 * n33 * n44) * idet;
    ret[1][2] = (n14 * n32 * n41 - n12 * n34 * n41 - n14 * n31 * n42 + n11 * n34 * n42 + n12 * n31 * n44 - n11 * n32 * n44) * idet;
    ret[1][3] = (n12 * n33 * n41 - n13 * n32 * n41 + n13 * n31 * n42 - n11 * n33 * n42 - n12 * n31 * n43 + n11 * n32 * n43) * idet;

    ret[2][0] = t13 * idet;
    ret[2][1] = (n14 * n23 * n41 - n13 * n24 * n41 - n14 * n21 * n43 + n11 * n24 * n43 + n13 * n21 * n44 - n11 * n23 * n44) * idet;
    ret[2][2] = (n12 * n24 * n41 - n14 * n22 * n41 + n14 * n21 * n42 - n11 * n24 * n42 - n12 * n21 * n44 + n11 * n22 * n44) * idet;
    ret[2][3] = (n13 * n22 * n41 - n12 * n23 * n41 - n13 * n21 * n42 + n11 * n23 * n42 + n12 * n21 * n43 - n11 * n22 * n43) * idet;

    ret[3][0] = t14 * idet;
    ret[3][1] = (n13 * n24 * n31 - n14 * n23 * n31 + n14 * n21 * n33 - n11 * n24 * n33 - n13 * n21 * n34 + n11 * n23 * n34) * idet;
    ret[3][2] = (n14 * n22 * n31 - n12 * n24 * n31 - n14 * n21 * n32 + n11 * n24 * n32 + n12 * n21 * n34 - n11 * n22 * n34) * idet;
    ret[3][3] = (n12 * n23 * n31 - n13 * n22 * n31 + n13 * n21 * n32 - n11 * n23 * n32 - n12 * n21 * n33 + n11 * n22 * n33) * idet;

    return ret;
}


cbuffer cbPerObject
{
    float4x4 WVP;
    float4 PlayerPos;
    float4 SampleMod;
    float4 SampleOffset;
    
    float CubeSize;
    float PlayerLightStrength;
    float LightStrength;
    float Time;
    
    float4 PlayerLightColor;
    float4 LightColor;
    float4 FogColor;
    
    float FogDistanceNear;
    float FogDistance;
    float DensityLimit;
    float NormalSampleDistance;
    
    float TextureBlendOffset;
    float TextureBlendExponent;
    float TextureBlendHeightStrength;
    float Pad;             //Three More Slots Here!
};
cbuffer Lights
{
    float4 LightPositions[64];
    unsigned int LightSize;
    
};

Texture2D shaderTexture;
SamplerState SampleType;

Texture2D shaderTextureN;
SamplerState SampleTypeN;

Texture2D shaderTextureH;
SamplerState SampleTypeH;

struct GS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Normal : NORMAL;
    float4 Color : COLOR;
};

float4 main(GS_OUTPUT input) : SV_TARGET
{
 
    float HeightX = (shaderTextureN.Sample(SampleTypeN, input.Color.zy * 0.1f).x);// * 2 - 1;                                                                              
    float HeightY = (shaderTextureN.Sample(SampleTypeN, input.Color.xz * 0.1f).x);// * 2 - 1;
    float HeightZ = (shaderTextureN.Sample(SampleTypeN, input.Color.xy * 0.1f).x);// * 2 - 1;
    
    
    
    float3 Weights = abs(input.Normal.xyz);
    Weights = saturate(Weights - TextureBlendOffset);
    //triW *= lerp(1, float3(heightX, heightY, heightZ), _BlendHeightStrength);
    Weights *= lerp(1.f, float3(HeightX, HeightY, HeightZ), TextureBlendHeightStrength);
    Weights = pow(Weights, TextureBlendExponent);
    Weights = normalize(Weights);
    
    //float3 colores = abs(input.Normal);
    
    float3 Clr = shaderTexture.Sample(SampleType, input.Color.zy * 0.1f) * Weights.x;
          Clr += shaderTexture.Sample(SampleType, input.Color.xz * 0.1f) * Weights.y;
          Clr += shaderTexture.Sample(SampleType, input.Color.xy * 0.1f) * Weights.z;
    
    float3 tNormalX = (shaderTextureN.Sample(SampleTypeN, input.Color.zy * 0.1f).xyz * 2 - 1);
    float3 tNormalY = (shaderTextureN.Sample(SampleTypeN, input.Color.xz * 0.1f).xyz * 2 - 1);
    float3 tNormalZ = (shaderTextureN.Sample(SampleTypeN, input.Color.xy * 0.1f).xyz * 2 - 1);

    float3 Flipper = sign(input.Normal.xyz);
    
    float3 NormalX = float3(0.f, tNormalX.yx);
    float3 NormalY = float3(tNormalY.x, 0.f, tNormalY.y);
    float3 NormalZ = float3(tNormalZ.xy, 0.f);
    
    float3 norm = (NormalX.xyz * Weights.x) + (NormalY.xyz * Weights.y) + (NormalZ.xyz * Weights.z) + input.Normal.xyz;
    
    norm = normalize(norm.xyz);
    
    float LightAngleStrength = clamp(dot(normalize((PlayerPos - input.Color).xyz).xyz, normalize(norm.xyz)), 0.f, 1.f);
    
    //LightAngleStrength = ((LightAngleStrength + 1.f) * 0.5f);
    //LightAngleStrength = min(max(LightAngleStrength, 0.f), 1.f);
    
    float Distance = distance(PlayerPos, input.Color);


    //float RealLightStrength = LightStrength * clamp(((abs(sin(Time * 40.f))) - abs(sin(Time * 12.1213f))+ 0.5), 0.8f, 1.f);
    float RealLightStrength = LightStrength * clamp(((abs(sin(Time * 80.f))) - abs(sin(Time * 32.1213f)) + 0.5), 0.9f, 1.f);
    //float RealLightStrength = LightStrength * max(abs(sin(Time)), 0.1f);
    
    //Distance = ((RealLightStrength - Distance) / RealLightStrength);
    float aDistance = ((PlayerLightStrength - Distance) / PlayerLightStrength);
    
    float3 oClr = (min(Clr, PlayerLightColor.xyz) * LightAngleStrength * aDistance);
    
    
    for (int i = 0; i < LightSize; i++)
    {
        
        float tLightAngleStrength = clamp(dot(normalize((LightPositions[i] - input.Color).xyz).xyz, normalize(norm.xyz)), 0.f, 1.f);
        float tDistance =((LightStrength - abs(distance(LightPositions[i], input.Color))) / LightStrength);
        
        oClr = max(min(Clr, LightColor.xyz) * tLightAngleStrength * tDistance * (LightPositions[i].w / 3), oClr);
        
    }
    //Distance = min(Distance, 1.f);
    
    //! FOG
    //oClr = oClr + (FogColor - oClr) * smoothstep(FogDistanceNear, FogDistance, Distance);
    float3 LitFogColor = float3(0, 0, 0);
    float FoglitExponent = 0.f;
   
    //if (LightSize > 0)
    //{
    //    FoglitExponent = 1 + dot(normalize(LightPositions[0] - PlayerPos), normalize(input.Color - PlayerPos)) / 2;
    //    //FoglitExponent *= distance(PlayerPos, LightPositions[0]) / FogDistance;
    //    LitFogColor = FogColor * FoglitExponent;
    //    
    //}
    oClr = lerp(oClr , (LitFogColor) , smoothstep(FogDistanceNear, FogDistance, Distance));
    
    //oClr = max(oClr, FogColor.xyz * smoothstep(FogDistanceNear, FogDistance, Distance));
    
   //Clr = min(Clr, max(float3(150.f / 255.f, 50.f / 255.f, 30.f / 255.f) * (LightAngleStrength * Distance), float3(255.f / 255.f, 200.f / 255.f, 6.f / 255.f) * (LightAngleStrength * 0.3f * Distance)));
   //Clr = (Clr * Distance);
   //Clr = (Clr * LightAngleStrength * Distance);
   //Clr = min(Clr * LightAngleStrength * Distance, float3(20.f / 255.f, 255.f / 255.f, 20.f / 255.f));
    
    return float4(oClr, 1.f);
}