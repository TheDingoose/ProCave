#pragma once

#include <d3d11.h>

#include <vector>

#include "Rendering/Vertex.h"
using namespace std;
struct Model {

	Model() {};
	Model(vector<Vertex>& aVertices, vector<DWORD>& aIndices) {
		copy(aVertices.begin(), aVertices.end(), Vertices.begin());
		copy(aIndices.begin(), aIndices.end(), Indices.begin());
	};

	vector<Vertex>Vertices;
	vector<DWORD> Indices;
};