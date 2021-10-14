#include "pch.h"
#include "EnvironmentCollisionHandler.h"

#include "Tools/MarchCubeSettings.h"
#include "Graphics/MarchCube.h"

using namespace rp3d;
using namespace std;

EnvironmentCollisionHandler::EnvironmentCollisionHandler()
{
	MarchCubeIndices.reserve(256); //this could go out of scope and move, messing everything up
	MarchCubeVertices.reserve(256);
}

void EnvironmentCollisionHandler::GenerateEnvironmentConcave(EnvironmentCollider Collider)
{
	Vector3 ColPos = world->getRigidBody(Collider.ID)->getTransform().getPosition();

	ColPos.x = round(ColPos.x);
	ColPos.y = round(ColPos.y);
	ColPos.z = round(ColPos.z);

	int Bodynum = 0;
	float AreaSize = Collider.Radius;
	for (float x = -AreaSize * MarchCubeSettings::get()->CubeSize; x <= AreaSize * MarchCubeSettings::get()->CubeSize; x += MarchCubeSettings::get()->CubeSize) {
		for (float y = -AreaSize * MarchCubeSettings::get()->CubeSize; y <= AreaSize * MarchCubeSettings::get()->CubeSize; y += MarchCubeSettings::get()->CubeSize) {
			for (float z = -AreaSize * MarchCubeSettings::get()->CubeSize; z <= AreaSize * MarchCubeSettings::get()->CubeSize; z += MarchCubeSettings::get()->CubeSize) {
				MarchCube CurrentCoob(XMVectorSet(ColPos.x + x, ColPos.y + y, ColPos.z + z, 0.f));
				if (CurrentCoob.Vertices.size() > 0) {
					
					//! Should copy these over in one chunk
					MarchCubeIndices.push_back(new std::vector<unsigned int>());
					MarchCubeIndices.back()->reserve(CurrentCoob.Indices.size());
					for (auto i : CurrentCoob.Indices)
					{
						MarchCubeIndices.back()->push_back(i);
					}
					MarchCubeVertices.push_back(new std::vector<float>());
					MarchCubeVertices.back()->reserve(CurrentCoob.LooseVertices.size() * 3);
					for (auto i : CurrentCoob.LooseVertices)
					{
						MarchCubeVertices.back()->push_back(i.m128_f32[0]);
						MarchCubeVertices.back()->push_back(i.m128_f32[1]);
						MarchCubeVertices.back()->push_back(i.m128_f32[2]);
					}


					TriangleVertexArray* triangleArray =
						new TriangleVertexArray(MarchCubeVertices.back()->size() / 3, &MarchCubeVertices[MarchCubeVertices.size() - 1][0][0], 3 * sizeof(float), MarchCubeIndices.back()->size() / 3,
							&MarchCubeIndices[MarchCubeIndices.size() - 1][0][0], 3 * sizeof(int),
							TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
							TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE);

					TriangleMesh* triangleMesh = physicsCommon->createTriangleMesh();

					// Add the triangle vertex array to the triangle mesh 
					triangleMesh->addSubpart(triangleArray);

					// Create the concave mesh shape 
					ConcaveMeshShape* concaveMesh = physicsCommon->createConcaveMeshShape(triangleMesh);

					Vector3 position(ColPos.x + x, ColPos.y + y, ColPos.z + z);
					Quaternion orientation = Quaternion::identity();
					Transform transform(position, orientation);

					// Create a rigid body in the world 
					RigidBodies.push_back(world->createRigidBody(transform));

					// Relative transform of the collider relative to the body origin 
					Transform transformm = Transform::identity();

					// Add the collider to the rigid body 
					RigidBodies.back()->addCollider(concaveMesh, transformm);
					RigidBodies.back()->setType(BodyType::STATIC);
					RigidBodies.back()->enableGravity(false);
					RigidBodies.back()->setIsAllowedToSleep(false);
					RigidBodies.back()->getCollider(0)->getMaterial().setBounciness(0.f);
					RigidBodies.back()->getCollider(0)->getMaterial().setFrictionCoefficient(0.5f);
				}
			}
		}
	}
}

void EnvironmentCollisionHandler::MakeCollide()
{
	for (auto& i : Colliders) {
		GenerateEnvironmentConcave(i);
	}
}

void EnvironmentCollisionHandler::BreakCollide()
{
	for (int i = 0; i < RigidBodies.size(); i++) {
		world->destroyRigidBody(RigidBodies[i]);
	}
	//? Memory leak?
	RigidBodies.clear();

	for (auto& i : MarchCubeIndices) {
		i->clear();
		delete i;
	}
	MarchCubeIndices.clear();

	for (auto& i : MarchCubeVertices) {
		i->clear();
		delete i;
	}
	MarchCubeVertices.clear();
}
