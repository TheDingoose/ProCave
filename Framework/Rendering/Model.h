#pragma once

#include <d3d11.h>
#include <iterator>
#include <vector>

#include "Rendering/Vertex.h"
using namespace std;
struct Model {

	Model() {};
	Model(vector<DWORD>& aIndices, vector<Vertex>& aVertices) {
		copy(aIndices.begin(), aIndices.end(), back_inserter(Indices));
		copy(aVertices.begin(), aVertices.end(), back_inserter(Vertices));
	};

	vector<Vertex>Vertices;
	vector<DWORD> Indices;
};