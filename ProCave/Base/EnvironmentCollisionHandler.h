#pragma once

#include <vector>

#include "reactphysics3d/reactphysics3d.h"

#include "Collision/EnvironmentCollider.h"

class EnvironmentCollisionHandler
{
public:
	//Keep a vector of all the currently active environmentcollider.
	//Also, management functions for these! (Checkallstillalive, add, remove)
	//Maybe just keep track of the ID's here? Then I might need less management

	EnvironmentCollisionHandler();


	std::vector<unsigned int> EnvironmentIndices;
	std::vector<float> EnvironmentVertices;
	rp3d::RigidBody* EnvironmentBody;
	int Inds = 0;
	int Verts = 0;


	std::vector<EnvironmentCollider> Colliders;
	std::vector<std::vector<unsigned int>*> MarchCubeIndices;
	std::vector<std::vector<float>*> MarchCubeVertices;
	std::vector<rp3d::RigidBody*> MarchCubeRigidBodies;

	rp3d::PhysicsWorld* world;
	rp3d::PhysicsCommon* physicsCommon;

	void GenerateEnvironmentConcave(EnvironmentCollider Collider);

	void CleanRemoved();

	void MakeCollide();
	void BreakCollide();
	
	
	//function to generate environment around an object
	//Keep a cache of what spots have already been generated this frame.
};

