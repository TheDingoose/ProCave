#pragma once
struct Vertex    //Overloaded Vertex Structure
{
	Vertex() {
		x = 0.f; y = 0.f; z = 0.f;  nx = 0.f; ny = 0.f; nz = 0.f;
	}
	Vertex(float sx, float sy, float sz, float snx, float sny, float snz) {
		x = sx;
		y = sy;
		z = sz;

		nx = snx;
		ny = sny;
		nz = snz;
	}

	union {
		float pos[3];
		struct
		{
			float x;
			float y;
			float z;
		};
	};
	union {
		float normal[3];
		struct
		{
			float nx;
			float ny;
			float nz;
		};
	};
};