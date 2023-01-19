#include "BaseApp.h"

#include "Graphics/Renderer.h"
#include "User/Input.h"
#include "Graphics/MarchCube.h"

#include "spdlog/spdlog.h"

#include "Noise/Ray.h"

#include "EnvironmentCollisionHandler.h"
#include "Collision/EnvironmentCollider.h"

#include "Tools/Loader.h"
#include "Tools/MarchCubeSettings.h"

#include "tinygltf/tiny_gltf.h"

#include "Collision/ReactHelper.h"

BaseApp::BaseApp()
{
}

//Basic window start and setup will be done in this phase. but not here. 
// Otherwise Preloading
void BaseApp::Init()
{
	Renderer* aRenderer = Renderer::get();
	Cam = new Camera(&aRenderer->Width, &aRenderer->Height);
	aRenderer->SetVP(&Cam->VP, &Cam->Transform.Transform ,&Cam->Transform.Translation, &Cam->Target);

	world = physicsCommon.createPhysicsWorld();

	world->setIsDebugRenderingEnabled(true);
	Renderer::get()->debugRenderer = &world->getDebugRenderer();
	world->getDebugRenderer().setIsDebugItemDisplayed(DebugRenderer::DebugItem::COLLISION_SHAPE, true); ;

	EnvCollision = new EnvironmentCollisionHandler;
	EnvCollision->world = world;
	EnvCollision->physicsCommon = &physicsCommon;
}



RigidBody* Testbody;

//Loading resources and assets, shaderloading goes here too
void BaseApp::Load()
{

	Vector3 position(0, 20, 0);
	Quaternion orientation = Quaternion::identity();
	Transform transform(position, orientation);
	//RigidBody* body = world->createRigidBody(transformm);

	//Renderer::get()->InitializeMesh(Loader::LoadStatic("../Assets/Model/BadCrymstalTest.glb"));
	//Renderer::get()->Models[0].Transform.Translation = XMFLOAT4(2.f, -10.f, 8.f, 0.f);
	//
	//Renderer::get()->InitializeMesh(Loader::LoadStatic("../Assets/Model/Creeper.glb"));
	//Renderer::get()->Models[0].Transform.Translation = XMFLOAT4(5.f, -10.f, 8.f, 0.f);

	position = Vector3(0.0, 0.0, 0.0);
	transform = Transform(position, orientation);

	// Create a rigid body in the world 
	ThePlayer = new Player(world->createRigidBody(transform));

	// Create the sphere shape with a radius of 2m 
	SphereShape* sphereShape = physicsCommon.createSphereShape(1.0);
	// Relative transform of the collider relative to the body origin 
	position = Vector3(0.0, 0.0, 0.0);
	transform = Transform(position, orientation);
	// Add the collider to the rigid body 
	Collider* colliderb;
	colliderb = ThePlayer->Collisionbody->addCollider(sphereShape, transform);
	colliderb->getMaterial().setFrictionCoefficient(0.6f);
	colliderb->getMaterial().setBounciness(0.f);
	colliderb->getMaterial().setMassDensity(80.f);
	ThePlayer->Collisionbody->setType(BodyType::DYNAMIC);
	ThePlayer->Collisionbody->enableGravity(true);
	ThePlayer->Collisionbody->setIsAllowedToSleep(false);
	ThePlayer->Collisionbody->setAngularDamping(1.f);
	ThePlayer->Collisionbody->setLinearDamping(0.01f);

	EnvCollision->Colliders.push_back(EnvironmentCollider(&ThePlayer->Collisionbody, 1.f));

	ThePlayer->SyncPos(&Cam->Transform);
	
}

XMVECTOR Velocity = XMVectorZero();

//Game update info goes here, more advanced functionality,
//such as messaging, happens in ProCave.cpp
void BaseApp::Tick(float Deltatime)
{
	
	MarchCubeSettings::get()->Time += Deltatime;

	XMVECTOR PlayerAcc = XMVectorZero();
	XMFLOAT4 Step;
	if (Input::get()->GetKey(Key_Forward)) {
		PlayerAcc.m128_f32[2] = 1.f;
	}
	if (Input::get()->GetKey(Key_Backward)) {
		PlayerAcc.m128_f32[2] = -1.f;
	}
	if (Input::get()->GetKey(Key_Right)) {
		PlayerAcc.m128_f32[0] = 1.f;
	}
	if (Input::get()->GetKey(Key_Left)) {
		PlayerAcc.m128_f32[0] = -1.f;
	}

	PlayerAcc = XMVector3Normalize(XMVector4Transform(PlayerAcc, Cam->Transform.Transform));

	ThePlayer->HandleInput(Vector3(PlayerAcc.m128_f32[0], 0.f, PlayerAcc.m128_f32[2]));

	if (MouseLock && GetForegroundWindow() == HandleWindow) {
		POINT cPos;
		GetCursorPos(&cPos);
		RECT rect = { NULL };
		GetWindowRect(HandleWindow, &rect);
		float Movex = -(rect.left + (rect.right - rect.left) / 2 - cPos.x) / 300.f;
		float Movey = -(rect.top + (rect.bottom - rect.top) / 2 - cPos.y) / 300.f;

		if (Cam->Transform.Rotation.x + Movey > -3.0f / 2.f && Cam->Transform.Rotation.x + Movey < 3.0f / 2.f) {
			Cam->Transform.Rotation.x += Movey;
		}
		Cam->Transform.Rotation.y += Movex;

		SetCursorPos(rect.left + (rect.right - rect.left) / 2, rect.top + (rect.bottom - rect.top) / 2);
	}

	if (Input::get()->GetKeyDown(Key_Light)) {

		Lights.push_back(new ThrowLight(world, &physicsCommon, XMLoadFloat4(&Cam->Transform.Translation), Cam->Target));

		EnvCollision->Colliders.push_back(EnvironmentCollider(&Lights[Lights.size() - 1]->CollisionBody, 0.5f));
	}

	EnvCollision->MakeCollide();

	if (Deltatime > 0.f) { world->update(Deltatime); };


	ThePlayer->SyncPos(&Cam->Transform);

	if (Input::get()->GetKey(Key_MouseUnlock)) {
		MouseLock = !MouseLock;
	}

	std::vector<XMVECTOR> LightPosses;
	for (int i = 0; i < Lights.size(); i++) {
		Lights[i]->Update(Deltatime);
		if (Lights[i]->LifeTime <= 0.f) {
			delete Lights[i];
			Lights.erase(Lights.begin() + i);
			i--;
			continue;
		}
		LightPosses.push_back(GetRePhyTranslation(Lights[i]->CollisionBody->getTransform()));
		LightPosses.back().m128_f32[3] = Lights[i]->LifeTime;
	}
	Renderer::get()->SetLights(LightPosses);
	EnvCollision->CleanRemoved();

	EnvCollision->BreakCollide();
	

	Cam->Update();


}

//Free all currently claimed resources
//Unload any loaded assets
void BaseApp::UnLoad()
{
}

//Kill screen, And stop final processes
void BaseApp::Exit()
{
}
