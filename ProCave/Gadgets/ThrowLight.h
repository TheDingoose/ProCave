#pragma once

//#include "Raccoon/Raccoon.h"
//#include "General/Transform.h"
#include "reactphysics3d/reactphysics3d.h"
#include <DirectXMath.h>

#include "Graphics/Renderer.h"
#include "Tools/Loader.h"
#include "General/UpdateObject.h"
#include "Collision/ReactHelper.h"
using namespace reactphysics3d;
using namespace DirectX;

class ThrowLight : public UpdateObject
{
public:
	unsigned short ModelIndex;
	float LifeTime = 4.0f;
	PhysicsWorld* physicsWorld = nullptr;
	ThrowLight(PhysicsWorld* World, PhysicsCommon* physicsCommon) {
		ThrowLight(World, physicsCommon, XMVectorSet(0, 0, 0, 0), XMVectorSet(0, 0, 0, 0));
	};

	ThrowLight(PhysicsWorld* World, PhysicsCommon* physicsCommon, XMVECTOR Pos, XMVECTOR Direction) {
		physicsWorld = World;
		
		Vector3 position(Pos.m128_f32[0] + Direction.m128_f32[0], Pos.m128_f32[1] + Direction.m128_f32[1], Pos.m128_f32[2] + Direction.m128_f32[2]);
		Quaternion orientation = Quaternion::identity();
		Transform transform(position, orientation);

		CollisionBody = World->createRigidBody(transform);

		CapsuleShape* Cappy = physicsCommon->createCapsuleShape(0.1f, 0.9f);
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

		ModelIndex = Renderer::get()->InitializeMesh(Loader::LoadStatic("../Assets/Model/StickLight.glb"));
		Renderer::get()->Models[ModelIndex].Transform.Scale = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.f);
	};

	void Update(float deltaTime) {

		XMStoreFloat4(&Renderer::get()->Models[ModelIndex].Transform.Translation, GetRePhyTranslation(CollisionBody->getTransform()));
		XMStoreFloat4(&Renderer::get()->Models[ModelIndex].Transform.Rotation, GetRePhyRotation(CollisionBody->getTransform()));
		Renderer::get()->Models[ModelIndex].Transform.UpdateMatrix();
		LifeTime -= deltaTime;
	};

	~ThrowLight() {
		Renderer::get()->RemoveMesh(ModelIndex);
		//physicsWorld->destroyCollisionBody(CollisionBody);
		physicsWorld->destroyRigidBody(CollisionBody);
		CollisionBody = nullptr;
		physicsWorld = nullptr;
	};

	RigidBody* CollisionBody;

};

