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

RWTexture2D<float4> tex;

cbuffer Constants
{
    int SampleSize = 64;
    int Axis;
    float SampleDistance;
    int pad2;
    float4 SourcePos;
    float4 Offset;
    float4 Mod;
};
//int SampleSize = 64;
//int Axis = 0;
//float SampleDistance = 1.f;
//int pad2;
//float SourcePos[3] = { 0.f, 0.f, 0.f };
//

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint Height = 64;
    uint Width = 64;
    tex.GetDimensions(Width, Height);
    int Num = 0;
    
    for (int i = -(SampleSize / 2); i < SampleSize / 2; i++)
    {
        for (int j = -(SampleSize / 2); j < SampleSize / 2; j++)
        {
            //Not efficient, but thats okay
            float4 SamplePos = float4(SourcePos.x, SourcePos.y, SourcePos.z, 0.f);

            if (Axis == 0)
            { //along X
                SamplePos.y -= (float) i * SampleDistance;
                SamplePos.z -= (float) j * SampleDistance;
            }
            else if (Axis == 1)
            { //along Y
                SamplePos.x += (float) i * SampleDistance;
                SamplePos.z -= (float) j * SampleDistance;
            }
            else if (Axis == 2)
            { //along Z
                SamplePos.x += (float) j * SampleDistance;
                SamplePos.y -= (float) i * SampleDistance;
            }
            
            float c = snoise((SamplePos + Offset) * Mod);
           
            tex[float2(Num % Width, Num / Width)] = float4(c, c, c, 1.f);
            Num++;
        }
    }
    //for (int i = 0; i < Width * Height; i++)
    //{
    //    tex[float2(i % Width, i / Width)] = float4(1.f, 1.f, 1.f, 1.f);
    //}

}