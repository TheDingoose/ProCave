
struct Vertex    //Overloaded Vertex Structure
{
	Vertex() {
		x = 0.f; y = 0.f; z = 0.f;  r = 0.f; g = 0.f; b = 0.f; a = 0.f;
	}
	Vertex(float sx, float sy, float sz, float sr, float sg, float sb, float sa) {
		x = sx;
		y = sy;
		z = sz;

		r = sr;
		g = sg;
		b = sb;
		a = sa;
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
		float colour[4];
		struct
		{
			float r;
			float g;
			float b;
			float a;
		};
	};
};