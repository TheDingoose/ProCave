cbuffer cbPerObject
{
    float4x4 WVP;
    float MarchCubeSize;
};

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
};

VS_OUTPUT main(float4 inPos : POSITION, float4 inNormal : NORMAL)
{
	VS_OUTPUT output;

    output.Pos = mul(inPos, WVP);
    output.Pos.z *= 0.01f;
    output.Color = abs(float4(inNormal.xyz, 1));

	return output;
}
