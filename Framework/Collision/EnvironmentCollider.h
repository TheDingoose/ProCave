#pragma once

#include "reactphysics3d/reactphysics3d.h"

using namespace reactphysics3d;

class EnvironmentCollider
{
public:
	EnvironmentCollider(RigidBody* aCollisionBody, float a_Radius = 1.f) { CollisionBody = aCollisionBody; Radius = a_Radius; };
	//EnvironmentCollider() { };

	//What is my ID in the collisionworld?
	//unsigned int ID;
	//This is extremely unsafe
	RigidBody* CollisionBody;


	//In what radius around me should I generate coobs?
	float Radius = 1.f;

	//A pointer to where rendertranslation should be?

};

