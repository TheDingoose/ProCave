cbuffer cbPerObject
{
    float4x4 WVP;
    float MarchCubeSize;
};

struct GS_Output
{
	float4 pos : SV_POSITION;
    float4 Color : COLOR;
};

[maxvertexcount(5)]
void main(
	point float4 input[1] : SV_POSITION, 
	inout TriangleStream< GS_Output > output
)
{
    //float4x4 ActualWVP = mul(input[0], WVP);
    
    GS_Output element;
    element.Color = float4(0.5f, 0.5f, 0.5f, 1.f);
    
    for (int i = 0; i < 1; i++)
    {
        element.pos = input[i];
        element.pos.y += 0.3f;
        element.pos = mul(element.pos, WVP);
        output.Append(element);
    
        element.pos = input[i];
        element.pos.y -= 0.1f;
        element.pos.x += 0.3f;
        element.pos = mul(element.pos, WVP);
        output.Append(element);
    
        element.pos = input[i];
        element.pos.y -= 0.1f;
        element.pos.x -= 0.3f;
        element.pos = mul(element.pos, WVP);
        output.Append(element);
        output.RestartStrip();
    }
    //this?
   
}