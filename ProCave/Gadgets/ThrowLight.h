#pragma once

//#include "Raccoon/Raccoon.h"
//#include "General/Transform.h"
#include "reactphysics3d/reactphysics3d.h"
#include <DirectXMath.h>

using namespace reactphysics3d;
using namespace DirectX;

class ThrowLight
{
public:
	ThrowLight(PhysicsWorld* World, PhysicsCommon* physicsCommon) {
		Vector3 position(1.0, 0.0, 0.0);
		Quaternion orientation = Quaternion::identity();
		Transform transform(position, orientation);

		CollisionBody = World->createRigidBody(transform);

		CapsuleShape* Cappy = physicsCommon->createCapsuleShape(0.1f, 0.3f);
		//SphereShape* sphereShape = physicsCommon->createSphereShape(0.1f);

		// Relative transform of the collider relative to the body origin 
		position = Vector3(0.0, 0.0, 0.0);
		transform = Transform(position, orientation);

		// Add the collider to the rigid body 
		Collider* colliderb;
		colliderb = CollisionBody->addCollider(Cappy, transform);
		colliderb->getMaterial().setFrictionCoefficient(0.6f);
		colliderb->getMaterial().setBounciness(0.8f);
		colliderb->getMaterial().setMassDensity(40);

		CollisionBody->setType(BodyType::DYNAMIC);
		CollisionBody->enableGravity(true);
		CollisionBody->setIsAllowedToSleep(false);
		CollisionBody->setAngularDamping(0.1f);
		CollisionBody->setLinearDamping(0.01f);
	};
	ThrowLight(PhysicsWorld* World, PhysicsCommon* physicsCommon, XMVECTOR Pos, XMVECTOR Direction) {
		Vector3 position(Pos.m128_f32[0] + Direction.m128_f32[0], Pos.m128_f32[1] + Direction.m128_f32[1], Pos.m128_f32[2] + Direction.m128_f32[2]);
		Quaternion orientation = Quaternion::identity();
		Transform transform(position, orientation);

		CollisionBody = World->createRigidBody(transform);

		CapsuleShape* Cappy = physicsCommon->createCapsuleShape(0.2f, 0.5f);
		//SphereShape* sphereShape = physicsCommon->createSphereShape(0.1f);

		// Relative transform of the collider relative to the body origin 
		position = Vector3(0.0, 0.0, 0.0);
		transform = Transform(position, orientation);

		// Add the collider to the rigid body 
		Collider* colliderb;
		colliderb = CollisionBody->addCollider(Cappy, transform);
		colliderb->getMaterial().setFrictionCoefficient(0.6f);
		colliderb->getMaterial().setBounciness(0.8f);
		colliderb->getMaterial().setMassDensity(40);

		CollisionBody->setType(BodyType::DYNAMIC);
		CollisionBody->enableGravity(true);
		CollisionBody->setIsAllowedToSleep(false);
		CollisionBody->setAngularDamping(0.1f);
		CollisionBody->setLinearDamping(0.01f);

		CollisionBody->applyForceToCenterOfMass(Vector3(Direction.m128_f32[0], Direction.m128_f32[1], Direction.m128_f32[2]) * 800.f);
	}
	//ThrowLight(rp3d::PhysicsWorld* World, rp3d::PhysicsCommon* physicsCommon, DirectX::XMVECTOR Pos) {
	//
	//};
	//~ThrowLight() {};

	RigidBody* CollisionBody;

};

