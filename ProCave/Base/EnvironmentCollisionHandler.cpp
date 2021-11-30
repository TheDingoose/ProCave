#include "pch.h"
#include "EnvironmentCollisionHandler.h"

#include "Tools/MarchCubeSettings.h"
#include "Graphics/MarchCube.h"

#include <iostream>

using namespace rp3d;
using namespace std;

EnvironmentCollisionHandler::EnvironmentCollisionHandler()
{
	MarchCubeIndices.reserve(256); //this could go out of scope and move, messing everything up
	MarchCubeVertices.reserve(256);
}

void EnvironmentCollisionHandler::GenerateEnvironmentConcave(EnvironmentCollider Collider)
{
	Vector3 ColPos = Collider.CollisionBody->getTransform().getPosition();

	ColPos.x = round(ColPos.x);
	ColPos.y = round(ColPos.y);
	ColPos.z = round(ColPos.z);

	//! Still gotta filter same coordinates
	int Bodynum = 0;
	float AreaSize = Collider.Radius;
	for (float x = -AreaSize * MarchCubeSettings::get()->CubeSize; x <= AreaSize * MarchCubeSettings::get()->CubeSize; x += MarchCubeSettings::get()->CubeSize) {
		for (float y = -AreaSize * MarchCubeSettings::get()->CubeSize; y <= AreaSize * MarchCubeSettings::get()->CubeSize; y += MarchCubeSettings::get()->CubeSize) {
			for (float z = -AreaSize * MarchCubeSettings::get()->CubeSize; z <= AreaSize * MarchCubeSettings::get()->CubeSize; z += MarchCubeSettings::get()->CubeSize) {
				MarchCube CurrentCoob(XMVectorSet(ColPos.x + x, ColPos.y + y, ColPos.z + z, 0.f));
				if (CurrentCoob.Vertices.size() > 0) {
					//for (auto i : CurrentCoob.Indices)
					//{
					//	
					//	MarchCubeIndices.back()->push_back(Inds);
					//	
					//}
					
					for (auto i : CurrentCoob.LooseVertices)
					{
						//MarchCubeVertices.back()->push_back(i.m128_f32[0] + ColPos.x + x);
						EnvironmentVertices.push_back(i.m128_f32[0] + ColPos.x + x);
						EnvironmentIndices.push_back(Inds++);
						//MarchCubeVertices.back()->push_back(i.m128_f32[1] + ColPos.y + y);
						EnvironmentVertices.push_back(i.m128_f32[1] + ColPos.y + y);
						EnvironmentIndices.push_back(Inds++);
						//MarchCubeVertices.back()->push_back(i.m128_f32[2] + ColPos.z + z);
						EnvironmentVertices.push_back(i.m128_f32[2] + ColPos.z + z);
						EnvironmentIndices.push_back(Inds++);

						Verts += 1;
					}

				}
			}
		}
	}
}

void EnvironmentCollisionHandler::CleanRemoved()
{
	for (int i = 0; i < Colliders.size(); i++) {
		if (Colliders[i].CollisionBody == nullptr) 
		{
			Colliders.erase(Colliders.begin() + i);
		};
	}
	return;
}

void EnvironmentCollisionHandler::MakeCollide()
{
	Inds = 0;
	Verts = 0;


	for (auto& i : Colliders) {
		GenerateEnvironmentConcave(i);
	}
	
	if (Verts > 0) {

		TriangleVertexArray* triangleArray =
			new TriangleVertexArray(Verts, &EnvironmentVertices[0], 3 * sizeof(float), Verts / 3,
				&EnvironmentIndices[0], 3 * sizeof(int),
				TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
				TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE);

		TriangleMesh* triangleMesh = physicsCommon->createTriangleMesh();

		// Add the triangle vertex array to the triangle mesh 
		triangleMesh->addSubpart(triangleArray);

		// Create the concave mesh shape 
		ConcaveMeshShape* concaveMesh = physicsCommon->createConcaveMeshShape(triangleMesh);

		Vector3 position(0, 0, 0);
		Quaternion orientation = Quaternion::identity();
		Transform transform(position, orientation);

		// Create a rigid body in the world 
		EnvironmentBody = world->createRigidBody(transform);

		// Relative transform of the collider relative to the body origin 
		Transform transformm = Transform::identity();

		// Add the collider to the rigid body 
		EnvironmentBody->addCollider(concaveMesh, transformm);
		EnvironmentBody->setType(BodyType::STATIC);
		EnvironmentBody->enableGravity(false);
		EnvironmentBody->setIsAllowedToSleep(false);
		EnvironmentBody->getCollider(0)->getMaterial().setBounciness(0.f);
		EnvironmentBody->getCollider(0)->getMaterial().setFrictionCoefficient(0.5f);
	}

}

void EnvironmentCollisionHandler::BreakCollide()
{
	if (Verts > 0) {

		EnvironmentIndices.clear();
		EnvironmentVertices.clear();
		world->destroyRigidBody(EnvironmentBody);
	}
}
