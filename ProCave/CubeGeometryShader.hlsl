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

cbuffer TableBuffer
{
    int triTable[256 * 16];
};
//ConstantBuffer<int> triTable[256][16] : register(b1);



struct GS_Output
{
	float4 pos : SV_POSITION;
    float4 Color : COLOR;
};

// ----------------------------------- 4D SIMPLEX NOISE -------------------------------------

#define NOISE_SIMPLEX_1_DIV_289 0.00346020761245674740484429065744f

float mod289(float x)
{
    return x - floor(x * NOISE_SIMPLEX_1_DIV_289) * 289.0;
}

float3 mod289(float3 x)
{
    return x - floor(x * NOISE_SIMPLEX_1_DIV_289) * 289.0;
}

float4 mod289(float4 x)
{
    return x - floor(x * NOISE_SIMPLEX_1_DIV_289) * 289.0;
}

float permute(float x)
{
    return mod289(
        x * x * 34.0 + x
    );
}

float4 permute(float4 x)
{
    return mod289(
		x * x * 34.0 + x
	);
}

float taylorInvSqrt(float r)
{
    return 1.79284291400159 - 0.85373472095314 * r;
}

float4 taylorInvSqrt(float4 r)
{
    return 1.79284291400159 - 0.85373472095314 * r;
}

float4 grad4(float j, float4 ip)
{
    const float4 ones = float4(1.0, 1.0, 1.0, -1.0);
    float4 p, s;
    p.xyz = floor(frac(j * ip.xyz) * 7.0) * ip.z - 1.0;
    p.w = 1.5 - dot(abs(p.xyz), ones.xyz);
	
	// GLSL: lessThan(x, y) = x < y
	// HLSL: 1 - step(y, x) = x < y
    s = float4(
		1 - step(0.0, p)
	);
    p.xyz = p.xyz + (s.xyz * 2 - 1) * s.www;
	
    return p;
}

float snoise(float4 v)
{
    const float4 C = float4(
		0.138196601125011, // (5 - sqrt(5))/20 G4
		0.276393202250021, // 2 * G4
		0.414589803375032, // 3 * G4
	 -0.447213595499958 // -1 + 4 * G4
	);

// First corner
    float4 i = floor(
		v +
		dot(
			v,
			0.309016994374947451 // (sqrt(5) - 1) / 4
		)
	);
    float4 x0 = v - i + dot(i, C.xxxx);

// Other corners

// Rank sorting originally contributed by Bill Licea-Kane, AMD (formerly ATI)
    float4 i0;
    float3 isX = step(x0.yzw, x0.xxx);
    float3 isYZ = step(x0.zww, x0.yyz);
    i0.x = isX.x + isX.y + isX.z;
    i0.yzw = 1.0 - isX;
    i0.y += isYZ.x + isYZ.y;
    i0.zw += 1.0 - isYZ.xy;
    i0.z += isYZ.z;
    i0.w += 1.0 - isYZ.z;

	// i0 now contains the unique values 0,1,2,3 in each channel
    float4 i3 = saturate(i0);
    float4 i2 = saturate(i0 - 1.0);
    float4 i1 = saturate(i0 - 2.0);

	//	x0 = x0 - 0.0 + 0.0 * C.xxxx
	//	x1 = x0 - i1  + 1.0 * C.xxxx
	//	x2 = x0 - i2  + 2.0 * C.xxxx
	//	x3 = x0 - i3  + 3.0 * C.xxxx
	//	x4 = x0 - 1.0 + 4.0 * C.xxxx
    float4 x1 = x0 - i1 + C.xxxx;
    float4 x2 = x0 - i2 + C.yyyy;
    float4 x3 = x0 - i3 + C.zzzz;
    float4 x4 = x0 + C.wwww;

// Permutations
    i = mod289(i);
    float j0 = permute(
		permute(
			permute(
				permute(i.w) + i.z
			) + i.y
		) + i.x
	);
    float4 j1 = permute(
		permute(
			permute(
				permute(
					i.w + float4(i1.w, i2.w, i3.w, 1.0)
				) + i.z + float4(i1.z, i2.z, i3.z, 1.0)
			) + i.y + float4(i1.y, i2.y, i3.y, 1.0)
		) + i.x + float4(i1.x, i2.x, i3.x, 1.0)
	);

// Gradients: 7x7x6 points over a cube, mapped onto a 4-cross polytope
// 7*7*6 = 294, which is close to the ring size 17*17 = 289.
    const float4 ip = float4(
		0.003401360544217687075, // 1/294
		0.020408163265306122449, // 1/49
		0.142857142857142857143, // 1/7
		0.0
	);

    float4 p0 = grad4(j0, ip);
    float4 p1 = grad4(j1.x, ip);
    float4 p2 = grad4(j1.y, ip);
    float4 p3 = grad4(j1.z, ip);
    float4 p4 = grad4(j1.w, ip);

// Normalise gradients
    float4 norm = taylorInvSqrt(float4(
		dot(p0, p0),
		dot(p1, p1),
		dot(p2, p2),
		dot(p3, p3)
	));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;
    p4 *= taylorInvSqrt(dot(p4, p4));

// Mix contributions from the five corners
    float3 m0 = max(
		0.6 - float3(
			dot(x0, x0),
			dot(x1, x1),
			dot(x2, x2)
		),
		0.0
	);
    float2 m1 = max(
		0.6 - float2(
			dot(x3, x3),
			dot(x4, x4)
		),
		0.0
	);
    m0 = m0 * m0;
    m1 = m1 * m1;
	
    return 49.0 * (
		dot(
			m0 * m0,
			float3(
				dot(p0, x0),
				dot(p1, x1),
				dot(p2, x2)
			)
		) + dot(
			m1 * m1,
			float2(
				dot(p3, x3),
				dot(p4, x4)
			)
		)
	);
}



//------------------------------------------------------------------------------------------

// ----------------------------------- MARCHING CUBES --------------------------------------

///  3 out, 3 in...
float3 hash33(float3 p3)
{
    p3 = frac(p3 * float3(.1031f, .1030f, .0973f));
    p3 += dot(p3, p3.yxz + 33.33);
    return frac((p3.xxy + p3.yxx) * p3.zyx);

}

float lerp(float min, float max, float t)
{
    return (min * (1 - t) + max * t);
}
//------------------------------------------------------------------------------------------



[maxvertexcount(15)]
void main(
	point float4 input[1] : SV_POSITION, 
	inout TriangleStream< GS_Output > output
)
{
    GS_Output element;
    
    //PERLIN TIME
    float MCSized2 = CubeSize / 2.f;
    
    element.Color = float4(0.37f, 0.18f, 0.56f, 1.f);
    
    float Distance = distance(PlayerPos, input[0]); 
    if (Distance < LightStrength)
    {
        Distance = (LightStrength - Distance) / LightStrength;

        element.Color = max(float4(0.92f * Distance, 0.45f * Distance, 0.15f * Distance, 1.f), element.Color);
    }
    float Samples[8];
    
    Samples[0] = snoise(float4(((input[0].x - MCSized2) + SampleOffset.x) * SampleMod.x, ((input[0].y - MCSized2) + SampleOffset.y) * SampleMod.y, ((input[0].z + MCSized2) + SampleOffset.z) * SampleMod.z, (Time + SampleOffset.w) * SampleMod.w));
    Samples[1] = snoise(float4(((input[0].x + MCSized2) + SampleOffset.x) * SampleMod.x, ((input[0].y - MCSized2) + SampleOffset.y) * SampleMod.y, ((input[0].z + MCSized2) + SampleOffset.z) * SampleMod.z, (Time + SampleOffset.w) * SampleMod.w));
    Samples[2] = snoise(float4(((input[0].x + MCSized2) + SampleOffset.x) * SampleMod.x, ((input[0].y - MCSized2) + SampleOffset.y) * SampleMod.y, ((input[0].z - MCSized2) + SampleOffset.z) * SampleMod.z, (Time + SampleOffset.w) * SampleMod.w));
    Samples[3] = snoise(float4(((input[0].x - MCSized2) + SampleOffset.x) * SampleMod.x, ((input[0].y - MCSized2) + SampleOffset.y) * SampleMod.y, ((input[0].z - MCSized2) + SampleOffset.z) * SampleMod.z, (Time + SampleOffset.w) * SampleMod.w));
    Samples[4] = snoise(float4(((input[0].x - MCSized2) + SampleOffset.x) * SampleMod.x, ((input[0].y + MCSized2) + SampleOffset.y) * SampleMod.y, ((input[0].z + MCSized2) + SampleOffset.z) * SampleMod.z, (Time + SampleOffset.w) * SampleMod.w));
    Samples[5] = snoise(float4(((input[0].x + MCSized2) + SampleOffset.x) * SampleMod.x, ((input[0].y + MCSized2) + SampleOffset.y) * SampleMod.y, ((input[0].z + MCSized2) + SampleOffset.z) * SampleMod.z, (Time + SampleOffset.w) * SampleMod.w));
    Samples[6] = snoise(float4(((input[0].x + MCSized2) + SampleOffset.x) * SampleMod.x, ((input[0].y + MCSized2) + SampleOffset.y) * SampleMod.y, ((input[0].z - MCSized2) + SampleOffset.z) * SampleMod.z, (Time + SampleOffset.w) * SampleMod.w));
    Samples[7] = snoise(float4(((input[0].x - MCSized2) + SampleOffset.x) * SampleMod.x, ((input[0].y + MCSized2) + SampleOffset.y) * SampleMod.y, ((input[0].z - MCSized2) + SampleOffset.z) * SampleMod.z, (Time + SampleOffset.w) * SampleMod.w));
    
        unsigned int CubeIndex = 0;
    if (Samples[0] < DensityLimit)
        CubeIndex |= 1;           
    if (Samples[1] < DensityLimit)
        CubeIndex |= 2;           
    if (Samples[2] < DensityLimit)
        CubeIndex |= 4;           
    if (Samples[3] < DensityLimit)
        CubeIndex |= 8;           
    if (Samples[4] < DensityLimit)
        CubeIndex |= 16;          
    if (Samples[5] < DensityLimit)
        CubeIndex |= 32;          
    if (Samples[6] < DensityLimit)
        CubeIndex |= 64;          
    if (Samples[7] < DensityLimit)
        CubeIndex |= 128;
    
    if (CubeIndex == 0 || CubeIndex == 255)
        return;
    
   int ntriang = 0;
   float Adapt = 0.f;
    for (int i = 0; round(triTable[CubeIndex * 16 + i].x) != -1 && !(i >= 16); i++)
   {
        Adapt = 0.f;
       //Vertices.push_back(Vertex(edgeposTable[triTable[CubeIndex * 16 + i] * 16 + 0], edgeposTable[triTable[CubeIndex * 16 + i] * 16 + 1], edgeposTable[triTable[CubeIndex * 16 + i] * 16 + 2],
       if (round(triTable[CubeIndex * 16 + i].x) == 0)
       {
            if (Samples[0] < Samples[1])
            {
                Adapt = -lerp(-1, 1, Samples[0] - Samples[1]) / 2.f;
            }
            else
            {
                Adapt = lerp(-1, 1, Samples[1] - Samples[0]) / 2.f;
            }
            element.pos = float4(/*Adapt +*/ input[0].x, -MCSized2 + input[0].y, MCSized2 + input[0].z, 1.f);
        }
        else if (round(triTable[CubeIndex * 16 + i].x) == 1)
       {
            if (Samples[2] < Samples[1])
            {
                Adapt = -lerp(-1, 1, Samples[2] - Samples[1]) / 2.f;
            }
            else
            {
                Adapt = lerp(-1, 1, Samples[1] - Samples[2]) / 2.f;
            }
            element.pos = float4(MCSized2 + input[0].x, -MCSized2 + input[0].y, /*Adapt +*/ input[0].z, 1.f);
        }
        else if (round(triTable[CubeIndex * 16 + i].x) == 2)
       {
            if (Samples[3] < Samples[2])
            {
                Adapt = -lerp(-1, 1, Samples[3] - Samples[2]) / 2.f;
            }
            else
            {
                Adapt = lerp(-1, 1, Samples[2] - Samples[3]) / 2.f;
            }
            element.pos = float4(/*Adapt +*/ input[0].x, -MCSized2 + input[0].y, -MCSized2 + input[0].z, 1.f);
        }
        else if (round(triTable[CubeIndex * 16 + i].x) == 3)
       {
            if (Samples[3] < Samples[0])
            {
                Adapt = -lerp(-1, 1, Samples[3] - Samples[0]) / 2.f;
            }
            else
            {
                Adapt = lerp(-1, 1, Samples[0] - Samples[3]) / 2.f;
            }
            element.pos = float4(-MCSized2 + input[0].x, -MCSized2 + input[0].y, /*Adapt +*/ input[0].z, 1.f);
        }
        else if (round(triTable[CubeIndex * 16 + i].x) == 4)
       {
            if (Samples[4] < Samples[5])
            {
                Adapt = -lerp(-1, 1, Samples[4] - Samples[5]) / 2.f;
            }
            else
            {
                Adapt = lerp(-1, 1, Samples[5] - Samples[4]) / 2.f;
            }
            element.pos = float4(/*Adapt +*/ input[0].x, MCSized2 + input[0].y, MCSized2 + input[0].z, 1.f);
        }
        else if (round(triTable[CubeIndex * 16 + i].x) == 5)
       {
            if (Samples[6] < Samples[5])
            {
                Adapt = -lerp(-1, 1, Samples[6] - Samples[5]) / 2.f;
            }
            else
            {
                Adapt = lerp(-1, 1, Samples[5] - Samples[6]) / 2.f;
            }
            element.pos = float4(MCSized2 + input[0].x, MCSized2 + input[0].y, /*Adapt +*/ input[0].z, 1.f);
        }
        else if (round(triTable[CubeIndex * 16 + i].x) == 6)
       {
            if (Samples[7] < Samples[6])
            {
                Adapt = -lerp(-1, 1, Samples[7] - Samples[6]) / 2.f;
            }
            else
            {
                Adapt = lerp(-1, 1, Samples[6] - Samples[7]) / 2.f;
            }
            element.pos = float4(/*Adapt +*/ input[0].x, MCSized2 + input[0].y, -MCSized2 + input[0].z, 1.f);
        }
        else if (round(triTable[CubeIndex * 16 + i].x) == 7)
       {
            if (Samples[7] < Samples[4])
            {
                Adapt = -lerp(-1, 1, Samples[7] - Samples[4]) / 2.f;
            }
            else
            {
                Adapt = lerp(-1, 1, Samples[4] - Samples[7]) / 2.f;
            }
            element.pos = float4(-MCSized2 + input[0].x, MCSized2 + input[0].y, /*Adapt +*/ input[0].z, 1.f);
        }
        else if (round(triTable[CubeIndex * 16 + i].x) == 8)
       {
            if (Samples[0] < Samples[4])
            {
                Adapt = -lerp(-1, 1, Samples[0] - Samples[4]) / 2.f;
            }
            else
            {
                Adapt = lerp(-1, 1, Samples[4] - Samples[0]) / 2.f;
            }
            
            element.pos = float4(-MCSized2 + input[0].x, /*Adapt +*/ input[0].y, MCSized2 + input[0].z, 1.f);
        }
        else if (round(triTable[CubeIndex * 16 + i].x) == 9)
       {
            if (Samples[1] < Samples[5])
            {
                Adapt = -lerp(-1, 1, Samples[1] - Samples[5]) / 2.f;
            }
            else
            {
                Adapt = lerp(-1, 1, Samples[5] - Samples[1]) / 2.f;
            }
            element.pos = float4(MCSized2 + input[0].x, /*Adapt +*/ input[0].y, MCSized2 + input[0].z, 1.f);
        }
        else if (round(triTable[CubeIndex * 16 + i].x) == 10)
       {
            if (Samples[2] < Samples[6])
            {
                Adapt = -lerp(-1, 1, Samples[2] - Samples[6]) / 2.f;
            }
            else
            {
                Adapt = lerp(-1, 1, Samples[6] - Samples[2]) / 2.f;
            }
            element.pos = float4(MCSized2 + input[0].x, /*Adapt +*/ input[0].y, -MCSized2 + input[0].z, 1.f);
        }
        else if (round(triTable[CubeIndex * 16 + i].x) == 11)
       {
            if (Samples[3] < Samples[7])
            {
                Adapt = -lerp(-1, 1, Samples[3] - Samples[7]) / 2.f;
            }
            else
            {
                Adapt = lerp(-1, 1, Samples[7] - Samples[3]) / 2.f;
            }
            element.pos = float4(-MCSized2 + input[0].x, /*Adapt +*/ input[0].y, -MCSized2 + input[0].z, 1.f);
        }
        
       element.pos = mul(element.pos, WVP);
       output.Append(element);
     //  output.RestartStrip();
   }
}