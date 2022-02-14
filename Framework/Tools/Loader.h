#pragma once

#include <string>

class Mesh;

class Loader {
public:
	static Mesh LoadStatic(std::string Name);
};

