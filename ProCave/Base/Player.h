#pragma once

#include "Collision/EnvironmentCollider.h"
#include "reactphysics3d/reactphysics3d.h"
#include "General/Transform.h"

class Player
{
public:
	Player(rp3d::RigidBody* a_Body);

	//input handling
	void HandleInput(rp3d::Vector3 a_Input);
	void SyncPos(cTransform* a_TransfromtoSync);

	//Rigidbody
	rp3d::RigidBody* Collisionbody;
	float Speed = 30.f;
};

