struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
};

Texture2D shaderTexture;
SamplerState SampleType;

float4 main(VS_OUTPUT input) : SV_TARGET
{
	//return input.Color;
    return shaderTexture.Sample(SampleType, input.Color.xy);
}