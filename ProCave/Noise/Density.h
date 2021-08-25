#pragma once
#include <vector>
#include <DirectXMath.h>
//#include "perlin/PerlinNoise.hpp"

#include "Tools/MarchCubeSettings.h"

using namespace DirectX;

//static siv::PerlinNoise perlin;

static XMVECTOR taylorInvSqrt(XMVECTOR r)
{
	return XMVectorSet(1.79284291400159, 1.79284291400159, 1.79284291400159, 1.79284291400159) - (0.85373472095314 * r);
}

#define NOISE_SIMPLEX_1_DIV_289 0.00346020761245674740484429065744f

static XMVECTOR grad4(float j, XMVECTOR ip)
{
	const XMVECTOR ones = XMVectorSet(1.0, 1.0, 1.0, -1.0);
	XMVECTOR p, s;
	//p.xyz = floor(frac(j * ip.xyz) * 7.0) * ip.z - 1.0;
	p = XMVectorFloor(((j * ip) - XMVectorFloor(j * ip)) * 7.0) * ip.m128_f32[2] - XMVectorSet(1.f, 1.f, 1.f, 1.f);
	//p.w = 1.5 - dot(abs(p.xyz), ones.xyz);
	p.m128_f32[3] = 1.5f - XMVector3Dot(XMVectorAbs(p), ones).m128_f32[0];

	// GLSL: lessThan(x, y) = x < y
	// HLSL: 1 - step(y, x) = x < y
	//s = float4(
	//	1 - step(0.0, p)
	//);

	s = XMVectorSet(1,1,1,1);
	if (0.f <= p.m128_f32[0]) { s.m128_f32[0] = 0.f; }
	if (0.f <= p.m128_f32[1]) { s.m128_f32[1] = 0.f; }
	if (0.f <= p.m128_f32[2]) { s.m128_f32[2] = 0.f; }
	if (0.f <= p.m128_f32[3]) { s.m128_f32[3] = 0.f; }

	//p.xyz = p.xyz + (s.xyz * 2 - 1) * s.www;
	p.m128_f32[0] = p.m128_f32[0] + (s.m128_f32[0] * 2.f - 1.f) * s.m128_f32[3];
	p.m128_f32[1] = p.m128_f32[1] + (s.m128_f32[1] * 2.f - 1.f) * s.m128_f32[3];
	p.m128_f32[2] = p.m128_f32[2] + (s.m128_f32[2] * 2.f - 1.f) * s.m128_f32[3];

	if (p.m128_f32[0] > 0 || p.m128_f32[1] > 0 || p.m128_f32[2] > 0) {
		int k = 0;
		k++;
	}


	return p;
}

static XMVECTOR mod289(XMVECTOR x)
{
	//return x - floor(x * NOISE_SIMPLEX_1_DIV_289) * 289.0;
	return (x - (XMVectorFloor(x * NOISE_SIMPLEX_1_DIV_289) * 289.0));
}

static float permute(float x) {
	return fmod(
		x * x * 34.f + x,
		289.f
	);
}

static XMVECTOR permute(XMVECTOR x)
{
	return mod289(
		(x * x * 34.0) + x
	);
}

static float snoise(XMVECTOR v)
{
	const XMVECTOR C = XMVectorSet(
		0.138196601125011,   // (5 - sqrt(5))/20 G4
		0.276393202250021,	 // 2 * G4
		0.414589803375032,	 // 3 * G4
		-0.447213595499958); // -1 + 4 * G4

	// First corner
	XMVECTOR i = XMVectorFloor(v + XMVector4Dot(v, XMVectorSet(0.309016994374947451, 0.309016994374947451, 0.309016994374947451, 0.309016994374947451)));
	
	XMVECTOR x0 = v - i + XMVector4Dot(i, XMVectorSet(C.m128_f32[0], C.m128_f32[0], C.m128_f32[0], C.m128_f32[0]));

	// Other corners

	// Rank sorting originally contributed by Bill Licea-Kane, AMD (formerly ATI)
	XMVECTOR i0 = XMVectorZero();
	
	//float3 isX = step(x0.yzw, x0.xxx);
	XMVECTOR isX = XMVectorZero();
	if (x0.m128_f32[1] <= x0.m128_f32[0]) { isX.m128_f32[0] = 1; }
	if (x0.m128_f32[2] <= x0.m128_f32[0]) { isX.m128_f32[1] = 1; }
	if (x0.m128_f32[3] <= x0.m128_f32[0]) { isX.m128_f32[2] = 1; }

	//float3 isYZ = step(x0.zww, x0.yyz);
	XMVECTOR isYZ = XMVectorZero();
	if (x0.m128_f32[2] <= x0.m128_f32[1]) { isYZ.m128_f32[0] = 1; }
	if (x0.m128_f32[3] <= x0.m128_f32[1]) { isYZ.m128_f32[1] = 1; }
	if (x0.m128_f32[3] <= x0.m128_f32[2]) { isYZ.m128_f32[2] = 1; }

	//i0.x = isX.x + isX.y + isX.z;
	i0.m128_f32[0] = isX.m128_f32[0] + isX.m128_f32[1] + isX.m128_f32[2];
	//i0.yzw = 1.0 - isX;
	i0.m128_f32[1] = 1.0f - isX.m128_f32[0];
	i0.m128_f32[2] = 1.0f - isX.m128_f32[1];
	i0.m128_f32[3] = 1.0f - isX.m128_f32[2];
	//i0.y += isYZ.x + isYZ.y;
	i0.m128_f32[1] += isYZ.m128_f32[0] + isYZ.m128_f32[1];
	//i0.zw += 1.0 - isYZ.xy;
	i0.m128_f32[2] += 1.0f - isYZ.m128_f32[0];
	i0.m128_f32[3] += 1.0f - isYZ.m128_f32[1];
	//i0.z += isYZ.z;
	i0.m128_f32[2] += isYZ.m128_f32[2];
	//i0.w += 1.0 - isYZ.z;
	i0.m128_f32[3] += 1.0f - isYZ.m128_f32[2];

	// i0 now contains the unique values 0,1,2,3 in each channel

	//float4 i3 = saturate(i0);
	XMVECTOR i3 = XMVectorZero();
	i3.m128_f32[0] = fmin(1.0f, fmax(0.0f, i0.m128_f32[0]));
	i3.m128_f32[1] = fmin(1.0f, fmax(0.0f, i0.m128_f32[1]));
	i3.m128_f32[2] = fmin(1.0f, fmax(0.0f, i0.m128_f32[2]));
	i3.m128_f32[3] = fmin(1.0f, fmax(0.0f, i0.m128_f32[3]));
	//float4 i2 = saturate(i0 - 1.0);
	XMVECTOR i2 = XMVectorZero();
	i2.m128_f32[0] = fmin(1.0f, fmax(0.0f, i0.m128_f32[0] - 1.0));
	i2.m128_f32[1] = fmin(1.0f, fmax(0.0f, i0.m128_f32[1] - 1.0));
	i2.m128_f32[2] = fmin(1.0f, fmax(0.0f, i0.m128_f32[2] - 1.0));
	i2.m128_f32[3] = fmin(1.0f, fmax(0.0f, i0.m128_f32[3] - 1.0));
	//float4 i1 = saturate(i0 - 2.0);
	XMVECTOR i1 = XMVectorZero();
	i1.m128_f32[0] = fmin(1.0f, fmax(0.0f, i0.m128_f32[0] - 2.0));
	i1.m128_f32[1] = fmin(1.0f, fmax(0.0f, i0.m128_f32[1] - 2.0));
	i1.m128_f32[2] = fmin(1.0f, fmax(0.0f, i0.m128_f32[2] - 2.0));
	i1.m128_f32[3] = fmin(1.0f, fmax(0.0f, i0.m128_f32[3] - 2.0));


	//	x0 = x0 - 0.0 + 0.0 * C.xxxx
	//	x1 = x0 - i1  + 1.0 * C.xxxx
	//	x2 = x0 - i2  + 2.0 * C.xxxx
	//	x3 = x0 - i3  + 3.0 * C.xxxx
	//	x4 = x0 - 1.0 + 4.0 * C.xxxx
	
	//float4 x1 = x0 - i1 + C.xxxx;
	XMVECTOR x1 = x0 - i1 + XMVectorSet(C.m128_f32[0], C.m128_f32[0], C.m128_f32[0], C.m128_f32[0]);
	//float4 x2 = x0 - i2 + C.yyyy;
	XMVECTOR x2 = x0 - i2 + XMVectorSet(C.m128_f32[1], C.m128_f32[1], C.m128_f32[1], C.m128_f32[1]);
	//float4 x3 = x0 - i3 + C.zzzz;
	XMVECTOR x3 = x0 - i3 + XMVectorSet(C.m128_f32[2], C.m128_f32[2], C.m128_f32[2], C.m128_f32[2]);
	//float4 x4 = x0 + C.wwww;
	XMVECTOR x4 = x0 + XMVectorSet(C.m128_f32[3], C.m128_f32[3], C.m128_f32[3], C.m128_f32[3]);



	// Permutations
	i = mod289(i);
	//float j0 = permute(
	//	permute(
	//		permute(
	//			permute(i.w) + i.z
	//		) + i.y
	//	) + i.x
	//);
	float j0 = 
	permute(
		permute(
			permute(
				permute(i.m128_f32[3]) + i.m128_f32[2]
			) + i.m128_f32[1]
		) + i.m128_f32[0]
	);
	//float4 j1 = permute(
	//	permute(
	//		permute(
	//			permute(
	//				i.w + float4(i1.w, i2.w, i3.w, 1.0)
	//			) + i.z + float4(i1.z, i2.z, i3.z, 1.0)
	//		) + i.y + float4(i1.y, i2.y, i3.y, 1.0)
	//	) + i.x + float4(i1.x, i2.x, i3.x, 1.0)
	//);
	XMVECTOR j1 = permute(
		permute(
			permute(
				permute(
					XMVectorSet(i.m128_f32[3], i.m128_f32[3], i.m128_f32[3], i.m128_f32[3]) + XMVectorSet(i1.m128_f32[3], i2.m128_f32[3], i3.m128_f32[3], 1.0f)
				) + XMVectorSet(i.m128_f32[2], i.m128_f32[2], i.m128_f32[2], i.m128_f32[2]) + XMVectorSet(i1.m128_f32[2], i2.m128_f32[2], i3.m128_f32[2], 1.0f)
			)	  + XMVectorSet(i.m128_f32[1], i.m128_f32[1], i.m128_f32[1], i.m128_f32[1]) + XMVectorSet(i1.m128_f32[1], i2.m128_f32[1], i3.m128_f32[1], 1.0f)
		)		  + XMVectorSet(i.m128_f32[0], i.m128_f32[0], i.m128_f32[0], i.m128_f32[0]) + XMVectorSet(i1.m128_f32[0], i2.m128_f32[0], i3.m128_f32[0], 1.0f)
	);

	// Gradients: 7x7x6 points over a cube, mapped onto a 4-cross polytope
	// 7*7*6 = 294, which is close to the ring size 17*17 = 289.

	const XMVECTOR ip = XMVectorSet(
		0.003401360544217687075,  // 1/294
		0.020408163265306122449,  // 1/49
		0.142857142857142857143,  // 1/7
		0.0);

	XMVECTOR p0 = grad4(j0, ip);
	XMVECTOR p1 = grad4(j1.m128_f32[0], ip);
	XMVECTOR p2 = grad4(j1.m128_f32[1], ip);
	XMVECTOR p3 = grad4(j1.m128_f32[2], ip);
	XMVECTOR p4 = grad4(j1.m128_f32[3], ip);

	// Normalise gradients
	//float4 norm = taylorInvSqrt(float4(
	//	dot(p0, p0),
	//	dot(p1, p1),
	//	dot(p2, p2),
	//	dot(p3, p3)
	//));
	XMVECTOR norm = taylorInvSqrt(
		XMVectorSet(
			XMVector4Dot(p0, p0).m128_f32[0],
			XMVector4Dot(p1, p1).m128_f32[0],
			XMVector4Dot(p2, p2).m128_f32[0],
			XMVector4Dot(p3, p3).m128_f32[0]
		));

	//p0 *= norm.x;
	p0 *= norm.m128_f32[0];
	//p1 *= norm.y;
	p1 *= norm.m128_f32[1];
	//p2 *= norm.z;
	p2 *= norm.m128_f32[2];
	//p3 *= norm.w;
	p3 *= norm.m128_f32[3];

	p4 *= taylorInvSqrt(XMVector4Dot(p4, p4));

	// Mix contributions from the five corners
	XMVECTOR m0 =
		XMVectorMax(
			XMVectorSet(
				0.6f - XMVector4Dot(x0, x0).m128_f32[0],
				0.6f - XMVector4Dot(x1, x1).m128_f32[0],
				0.6f - XMVector4Dot(x2, x2).m128_f32[0],
				0.f),
			XMVectorSet(0.f, 0.f, 0.f, 0.f)
		);
	XMVECTOR m1 =
		XMVectorMax(
			XMVectorSet(
				0.6f - XMVector4Dot(x3, x3).m128_f32[0],
				0.6f - XMVector4Dot(x4, x4).m128_f32[0],
				0.f,
				0.f),
			XMVectorSet(0.f, 0.f, 0.f, 0.f)
		);

	m0 = m0 * m0;
	m1 = m1 * m1;

	//return 49.0 * (
	//	dot(
	//		m0 * m0,
	//		float3(
	//			dot(p0, x0),
	//			dot(p1, x1),
	//			dot(p2, x2)
	//		)
	//	) + dot(
	//		m1 * m1,
	//		float2(
	//			dot(p3, x3),
	//			dot(p4, x4)
	//		)
	//	)
	//	);
	return 49.0f * (
		XMVector3Dot(m0 * m0, 
			XMVectorSet(
				XMVector4Dot(p0, x0).m128_f32[0],
				XMVector4Dot(p1, x1).m128_f32[0],
				XMVector4Dot(p2, x2).m128_f32[0],
				0.f)
			).m128_f32[0] +
		XMVector2Dot(
			m1 * m1,
			XMVectorSet(
			XMVector4Dot(p3, x3).m128_f32[0],
			XMVector4Dot(p4, x4).m128_f32[0],
			0.f, 0.f)
		).m128_f32[0]
		);
}

static float GetDensity(XMVECTOR aPos) {
	XMVECTOR sPoint = XMVectorSet(
		(aPos.m128_f32[0] + MarchCubeSettings::get()->SampleOffset.x) * MarchCubeSettings::get()->SampleMod.x,
		(aPos.m128_f32[1] + MarchCubeSettings::get()->SampleOffset.y) * MarchCubeSettings::get()->SampleMod.y,
		(aPos.m128_f32[2] + MarchCubeSettings::get()->SampleOffset.z) * MarchCubeSettings::get()->SampleMod.z,
		(aPos.m128_f32[3] + MarchCubeSettings::get()->SampleOffset.w) * MarchCubeSettings::get()->SampleMod.w);
	return snoise(sPoint);
}

static float GetDensity(XMVECTOR aPos, float Time) {
	return GetDensity(XMVectorSet(aPos.m128_f32[0], aPos.m128_f32[1], aPos.m128_f32[2], Time));
}


static float GetDensity(XMFLOAT4 aPos) {
	return GetDensity(XMLoadFloat4(&aPos));
}

