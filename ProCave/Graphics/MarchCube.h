#pragma once
#include <vector>
#include <DirectXMath.h>
//#include <d3d11.h>

#include "Rendering/Vertex.h"

using namespace DirectX;

const float DensityLimit = 0.0f;
constexpr float CubeSize = 5.f;
constexpr float CubeSized2 = CubeSize / 2.f;


class MarchCube {
public:


	MarchCube();
	MarchCube(XMFLOAT4 aPos);
	MarchCube(XMVECTOR aPos);

	void Sample(XMFLOAT4 aPos);
	void Sample(XMVECTOR aPos);


	
	unsigned char Case;
	std::vector<Vertex> Vertices;
	std::vector<unsigned long> Indices;
};