#pragma once
#include <vector>
#include <DirectXMath.h>
#include "perlin/PerlinNoise.hpp"

using namespace DirectX;

static siv::PerlinNoise perlin;

static float GetDensity(XMFLOAT4 aPos) {
	float Density = 0.f;

	//Density -= aPos.y;
	
	aPos.x *= 0.05;
	aPos.y *= 0.05;
	aPos.z *= 0.05;



	//Density += (fmod(((float)rand()) , 100.f) / 100.f) / aPos.y;
	Density += std::fmin(perlin.noise3D(aPos.x, aPos.y, aPos.z) * 10, 1.f) + 0.5f;
	
	if (aPos.y < 0.f) {
	//	Density = 2.f;
	}

	return Density;
}

static float GetDensity(XMVECTOR aPos) {
	XMFLOAT4 Pos;
	XMStoreFloat4(&Pos, aPos);

	return GetDensity(Pos);
}



float snoise(XMVECTOR v)
{
	XMVECTOR C; 

	C.m128_f32[0] = 0.138196601125011;  // (5 - sqrt(5))/20 G4
	C.m128_f32[1] = 0.276393202250021;	// 2 * G4
	C.m128_f32[2] = 0.414589803375032;	// 3 * G4
	C.m128_f32[3] = -0.447213595499958;	// -1 + 4 * G4



	XMVECTOR TMP;
	TMP.m128_f32[0] = 0.309016994374947451; // (sqrt(5) - 1) / 4
	TMP.m128_f32[1] = 0.309016994374947451; // (sqrt(5) - 1) / 4
	TMP.m128_f32[2] = 0.309016994374947451; // (sqrt(5) - 1) / 4
	TMP.m128_f32[3] = 0.309016994374947451; // (sqrt(5) - 1) / 4

	XMVECTOR i = XMVectorFloor(XMVectorAdd(v, XMVector3Dot(v, TMP))); //replace TMP with a single float?
	// First corner

	XMVECTOR x0 = XMVectorSubtract(v, XMVectorAdd(i, XMVector3Dot(i , C)))

	// Other corners


		//I WAS HERE 
		/*


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
	XMVECTOR ip;
	ip.m128_f32[0] = 0.003401360544217687075;  // 1/294
	ip.m128_f32[1] = 0.020408163265306122449;  // 1/49
	ip.m128_f32[2] = 0.142857142857142857143;  // 1/7
	ip.m128_f32[3] = 0.0;


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
