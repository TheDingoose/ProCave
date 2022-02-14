#include "Loader.h"

#include <memory>


#include "spdlog/spdlog.h"

#define TINYGLTF_IMPLEMENTATION
// #define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "tinygltf/tiny_gltf.h"

#include "Rendering/Mesh.h"

struct Offset {
	Offset(float asx, float asy, float asz, float atx, float aty, float atz) {
		sx = asx;
		sy = asy;
		sz = asz;
		tx = atx;
		ty = aty;
		tz = atz;
	}
	Offset() {};

	float sx = 1.f;
	float sy = 1.f;
	float sz = 1.f;

	float tx = 0.f;
	float ty = 0.f;
	float tz = 0.f;
};

tinygltf::Model TinyLoad(std::string name) {
	tinygltf::Model model;
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;
	//std::string name = "../Assets/Model/box01.glb";

	bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, name);

	if (!warn.empty()) {
		spdlog::warn("%s\n", warn.c_str());
	}

	if (!err.empty()) {
		spdlog::error("%s\n", err.c_str());
	}

	if (!ret) {
		spdlog::error("Failed to parse glTF: %s\n", name.c_str());
	}

	return model;
}





std::vector<float> ReadDataAsFloat(std::vector<unsigned char>& data, unsigned int offset, unsigned int count) {
	std::vector<float> ret;
	ret.assign(reinterpret_cast<const float*>(&data[offset]), &reinterpret_cast<const float*>(&data[offset])[count]);
	return ret;
};


std::vector<unsigned short> ReadDataAsUnsignedShort(std::vector<unsigned char>& data, unsigned int offset, unsigned int count) {
	std::vector<unsigned short> ret;
	ret.assign(reinterpret_cast<const unsigned short*>(&data[offset]), &reinterpret_cast<const unsigned short*>(&data[offset])[count]);

	//ret.push_back(*reinterpret_cast<const unsigned short*>(&data[0]));
	//for (int i = 0; i < data.size() / 2; i++) {
	//	ret.push_back(reinterpret_cast<const unsigned short*>(&data[0])[i]);
	//}
	return ret;
};

std::vector<DWORD> ReadDataAsDWORD(std::vector<unsigned char>& data) {
	std::vector<DWORD> ret;
	//ret =  reinterpret_cast<unsigned short*>(data[0]);
	ret.assign(reinterpret_cast<const DWORD*>(data[0]), reinterpret_cast<const DWORD*>(data[0] + (data.size() / 4)));

	return ret;
};


Mesh Loader::LoadStatic(std::string name) {
Mesh ret;
std::vector<std::vector<unsigned short>> Indices;
std::vector<std::vector<float>> Positions;
std::vector<std::vector<float>> Normals;
std::vector<Offset> Offsets;

	tinygltf::Model data = TinyLoad(name);
	//now that we have the file loaded we need to stitch all the data together into only what we need for a static mesh;

	Offsets.resize(data.nodes.size());

	unsigned int Index = 0;
	for (auto& mesh: data.meshes) {
		for (auto& primitive : mesh.primitives) {
			if (primitive.mode == TINYGLTF_MODE_TRIANGLES) {
				
				if (data.nodes[Index].scale.size() != 0) {
					Offsets[Index].sx = data.nodes[Index].scale[0];
					Offsets[Index].sy = data.nodes[Index].scale[1];
					Offsets[Index].sz = data.nodes[Index].scale[2];
				}
				if (data.nodes[Index].translation.size() != 0) {
					Offsets[Index].tx = data.nodes[Index].translation[0];
					Offsets[Index].ty = data.nodes[Index].translation[1];
					Offsets[Index].tz = data.nodes[Index].translation[2];
				}


				//ret.Indices = data.buffers[data.bufferViews[data.accessors[primitive.indices].bufferView].buffer].data;
				Indices.push_back(ReadDataAsUnsignedShort(data.buffers[data.bufferViews[data.accessors[primitive.indices].bufferView].buffer].data
				,data.bufferViews[data.accessors[primitive.indices].bufferView].byteOffset, 
				data.accessors[primitive.indices].count
				));
				//ret.Indices.assign(Ind.begin(), Ind.end());
				//ret.Indices.assign()
				for (auto& attribute : primitive.attributes) {
					if (attribute.first == "POSITION") {//is this a position buffer?
						//add this buffer to the Model

						spdlog::info("Load Positions");

						//? Make into a function that receives only data and the relevant accessor.
						Positions.push_back(ReadDataAsFloat(data.buffers[data.bufferViews[data.accessors[attribute.second].bufferView].buffer].data
							, data.bufferViews[data.accessors[attribute.second].bufferView].byteOffset,
							  data.accessors[attribute.second].count * 3
						));


						data.accessors[attribute.second];
					}
					else if (attribute.first == "NORMAL") {//is this a normal buffer?
						
						spdlog::info("Load Normals");
						//? Make into a function that receives only data and the relevant accessor.
						Normals.push_back(ReadDataAsFloat(data.buffers[data.bufferViews[data.accessors[attribute.second].bufferView].buffer].data
							, data.bufferViews[data.accessors[attribute.second].bufferView].byteOffset,
							data.accessors[attribute.second].count * 3
						));

					}

				}
			}
			else {
				spdlog::error("This model is not in simple triangles!: %s", name.c_str());
			}
		}
		Index++;
	}

	//for (auto i : ret.Indices) {
	//	spdlog::info("Index?: {0:d}", i);
	//}

	//spdlog::info("Now Vertices");
	//ret.Vertices.reserve(Positions.size() / 3);
	
	//What is the total size?
	unsigned int TotalSize = 0;
	for (auto& i : Indices) {
		TotalSize += i.size();
	}

	//? INDICES NEED TO BE UPPED PER MESH, NOW EVERYTHING IS ONLY POINTING TO THE FIRST MESH!

	ret.Indices.resize(TotalSize);
	Index = 0;
	unsigned int Part = 0;
	for (int j = 0; j < Indices.size(); j++) {
		spdlog::info("Loaded mesh Indices {0:d} of size: {1:d}", j, Indices[j].size());
		for (int i = 0; i < Indices[j].size(); i++) {
			ret.Indices[Index] = (Indices[j][i]) + Part;
			Index++;
		}
		Part += (Positions[j].size() / 3);
		spdlog::info("Posses: {0:d}", Part);
	}


	for (int i = 0; i < ret.Indices.size(); i+= 100) {
		spdlog::info("Indss: {0:d}", ret.Indices[i]);
	}

	TotalSize = 0;
	for (auto& i : Positions) {
		TotalSize += i.size();
	}
	ret.Vertices.resize(TotalSize);

	Index = 0;
	for(int j = 0; j < Positions.size(); j++) {
		spdlog::info("Loaded mesh Positions {0:d} of size: {1:d}", j, Positions[j].size() / 3);
		for (int i = 0; i < Positions[j].size() / 3; i++) {	
			ret.Vertices[Index].x = (Positions[j][i * 3]	 * Offsets[j].sx) + Offsets[j].tx;
			ret.Vertices[Index].y = (Positions[j][i * 3 + 1] * Offsets[j].sy) + Offsets[j].ty;
			ret.Vertices[Index].z = (Positions[j][i * 3 + 2] * Offsets[j].sz) + Offsets[j].tz;

			ret.Vertices[Index].nx = Normals[j][i * 3];
			ret.Vertices[Index].ny = Normals[j][i * 3 + 1];
			ret.Vertices[Index].nz = Normals[j][i * 3 + 2];
			Index++;
		}
	}

	//for (auto i : ret.Vertices) {
	//	spdlog::info("Positions: {:03.2f}, {:03.2f}, {:03.2f} ||| Normals: {:03.2f}, {:03.2f}, {:03.2f}", i.x, i.y, i.z, i.nx, i.ny, i.nz);
	//}


	return ret;
}
