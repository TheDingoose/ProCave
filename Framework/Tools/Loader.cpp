#include "Loader.h"

#include <memory>
#include <DirectXMath.h>

#include "spdlog/spdlog.h"

#define TINYGLTF_IMPLEMENTATION
// #define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "tinygltf/tiny_gltf.h"

#include "Rendering/Mesh.h"

using namespace DirectX;

struct Offset {
	Offset(float asx, float asy, float asz, float atx, float aty, float atz, float arx, float ary, float arz, float arw) {
		sx = asx;
		sy = asy;
		sz = asz;
		tx = atx;
		ty = aty;
		tz = atz;

		rx = arx;
		ry = ary;
		rz = arz;
		rw = arw;
	}
	Offset() {};
	float osx = 1.f;
	float osy = 1.f;
	float osz = 1.f;

	float sx = 1.f;
	float sy = 1.f;
	float sz = 1.f;

	float tx = 0.f;
	float ty = 0.f;
	float tz = 0.f;

	float rx = 0.f;
	float ry = 0.f;
	float rz = 0.f;
	float rw = 1.f;
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
	//std::vector<Offset> Offsets;
	std::vector<XMMATRIX> TransformMatrix;

	tinygltf::Model data = TinyLoad(name);
	//now that we have the file loaded we need to stitch all the data together into only what we need for a static mesh;



	Indices.resize(data.nodes.size());
	Positions.resize(data.nodes.size());
	Normals.resize(data.nodes.size());
	TransformMatrix.resize(data.nodes.size());



	for(auto& i: TransformMatrix) {
		i = XMMatrixIdentity();
	}


	XMMATRIX SizeMatrix = XMMatrixIdentity();
	XMMATRIX RotationMatrix = XMMatrixIdentity();
	XMMATRIX TranslationMatrix = XMMatrixIdentity();


	unsigned int Index = 0;
	for (auto& node : data.nodes) {
		SizeMatrix = XMMatrixIdentity();
		RotationMatrix = XMMatrixIdentity();
		TranslationMatrix = XMMatrixIdentity();

		
		//Scale
		if (data.nodes[Index].scale.size() != 0) {
			SizeMatrix = XMMatrixScaling(data.nodes[Index].scale[0], data.nodes[Index].scale[1], data.nodes[Index].scale[2]);

		}

		//Rotations
		if (data.nodes[Index].rotation.size() != 0) {
			//RotationMatrix = XMMatrixRotationRollPitchYaw(data.nodes[Index].rotation[0], data.nodes[Index].rotation[1], data.nodes[Index].rotation[2]);
			RotationMatrix = XMMatrixRotationQuaternion(XMVectorSet(data.nodes[Index].rotation[0], data.nodes[Index].rotation[1], data.nodes[Index].rotation[2], data.nodes[Index].rotation[3]));

		}

		//Translation
		if (data.nodes[Index].translation.size() != 0) {
			TranslationMatrix = XMMatrixTranslation(data.nodes[Index].translation[0], data.nodes[Index].translation[1], data.nodes[Index].translation[2]);
		}

		TransformMatrix[Index] = (SizeMatrix * RotationMatrix * TranslationMatrix) * TransformMatrix[Index];

		for (auto i : data.nodes[Index].children) {
			TransformMatrix[i] = TransformMatrix[i] * TransformMatrix[Index];
		}

		if (node.mesh != - 1) {
			for (auto& primitive : data.meshes[node.mesh].primitives) {
				if (primitive.mode == TINYGLTF_MODE_TRIANGLES) {




					//ret.Indices = data.buffers[data.bufferViews[data.accessors[primitive.indices].bufferView].buffer].data;
					Indices[Index] = (ReadDataAsUnsignedShort(data.buffers[data.bufferViews[data.accessors[primitive.indices].bufferView].buffer].data
						, data.bufferViews[data.accessors[primitive.indices].bufferView].byteOffset,
						data.accessors[primitive.indices].count
					));

					for (auto& attribute : primitive.attributes) {
						if (attribute.first == "POSITION") {//is this a position buffer?
							//add this buffer to the Model

							//? Make into a function that receives only data and the relevant accessor.
							Positions[Index] = (ReadDataAsFloat(data.buffers[data.bufferViews[data.accessors[attribute.second].bufferView].buffer].data
								, data.bufferViews[data.accessors[attribute.second].bufferView].byteOffset,
								data.accessors[attribute.second].count * 3
							));


							data.accessors[attribute.second];
						}
						else if (attribute.first == "NORMAL") {//is this a normal buffer?

							spdlog::info("Load Normals");
							//? Make into a function that receives only data and the relevant accessor.
							Normals[Index] = (ReadDataAsFloat(data.buffers[data.bufferViews[data.accessors[attribute.second].bufferView].buffer].data
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
		}
		Index++;
	}

	//What is the total size?
	unsigned int TotalSize = 0;
	for (auto& i : Indices) {
		TotalSize += i.size();
	}

	ret.Indices.resize(TotalSize);
	Index = 0;
	unsigned int Part = 0;
	for (int j = 0; j < Indices.size(); j++) {
		//spdlog::info("Loaded mesh Indices {0:d} of size: {1:d}", j, Indices[j].size());
		for (int i = 0; i < Indices[j].size(); i++) {
			ret.Indices[Index] = (Indices[j][i]) + Part;
			Index++;
		}
		Part += (Positions[j].size() / 3);
		//spdlog::info("Posses: {0:d}", Part);
	}


	for (int i = 0; i < ret.Indices.size(); i += 100) {
		//spdlog::info("Indss: {0:d}", ret.Indices[i]);
	}

	TotalSize = 0;
	for (auto& i : Positions) {
		TotalSize += i.size();
	}
	ret.Vertices.resize(TotalSize);

	Index = 0;
	for (int j = 0; j < Positions.size(); j++) {
		//spdlog::info("Loaded mesh Positions {0:d} of size: {1:d}", j, Positions[j].size() / 3);
		for (int i = 0; i < Positions[j].size() / 3; i++) {

			XMVECTOR Pos = XMVectorSet(Positions[j][i * 3], Positions[j][i * 3 + 1], Positions[j][i * 3 + 2], 0);
			Pos = XMVector3Transform(Pos, TransformMatrix[j]);

			ret.Vertices[Index].x = (Pos.m128_f32[0]);
			ret.Vertices[Index].y = (Pos.m128_f32[1]);
			ret.Vertices[Index].z = (Pos.m128_f32[2]);

			ret.Vertices[Index].nx = Normals[j][i * 3];
			ret.Vertices[Index].ny = Normals[j][i * 3 + 1];
			ret.Vertices[Index].nz = Normals[j][i * 3 + 2];
			Index++;
		}
	}

	//for (auto i : ret.Vertices) {
	//	spdlog::info("Positions: {:03.2f}, {:03.2f}, {:03.2f} ||| Normals: {:03.2f}, {:03.2f}, {:03.2f}", i.x, i.y, i.z, i.nx, i.ny, i.nz);
	//}
	//spdlog::info("FinalVertices {0:d}", ret.Vertices.size());

	return ret;
}