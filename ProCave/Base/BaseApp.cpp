#include "BaseApp.h"

#include "Graphics/Renderer.h"
#include "User/Input.h"
#include "Graphics/MarchCube.h"

#include "spdlog/spdlog.h"

#include "Noise/Ray.h"

BaseApp::BaseApp()
{
}

//Basic window start and setup will be done in this phase. but not here. 
// Otherwise Preloading
void BaseApp::Init()
{
	Renderer* aRenderer = Renderer::get();
	Cam = new Camera(&aRenderer->Width, &aRenderer->Height);
	aRenderer->SetVP(&Cam->VP, &Cam->Transform.Translation);
}





//Loading resources and assets, shaderloading goes here too
void BaseApp::Load()
{

	//Vector3 position(0, 20, 0);
	//Quaternion orientation = Quaternion::identity();
	//Transform transformm(position, orientation);
	//RigidBody* body = world->createRigidBody(transformm);
	world = physicsCommon.createPhysicsWorld();

	world->setIsDebugRenderingEnabled(true);
	Renderer::get()->debugRenderer = &world->getDebugRenderer();
	world->getDebugRenderer().setIsDebugItemDisplayed(DebugRenderer::DebugItem::COLLISION_SHAPE, true); ;


	//std::vector<Vertex> v =
	//{
	//	Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f),
	//	Vertex(-1.0f, +1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f),
	//	Vertex(+1.0f, +1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f),
	//	Vertex(+1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f),
	//	Vertex(-1.0f, -1.0f, +1.0f, 0.0f, 1.0f, 1.0f, 1.0f),
	//	Vertex(-1.0f, +1.0f, +1.0f, 1.0f, 1.0f, 1.0f, 1.0f),
	//	Vertex(+1.0f, +1.0f, +1.0f, 1.0f, 0.0f, 1.0f, 1.0f),
	//	Vertex(+1.0f, -1.0f, +1.0f, 1.0f, 0.0f, 0.0f, 1.0f),
	//};
	//
	//std::vector<DWORD> indices = {
	//	// front face
	//	0, 1, 2,
	//	0, 2, 3,
	//
	//	// back face
	//	4, 6, 5,
	//	4, 7, 6,
	//
	//	// left face
	//	4, 5, 1,
	//	4, 1, 0,
	//
	//	// right face
	//	3, 2, 6,
	//	3, 6, 7,
	//
	//	// top face
	//	1, 5, 6,
	//	1, 6, 2,
	//
	//	// bottom face
	//	4, 0, 3,
	//	4, 3, 7
	//};
	//
	//Model Coob(indices, v);
	//
	//Renderer::get()->InitializeModel(Coob);
	//
	//Renderer::get()->Models[0].Transform.Translation = XMFLOAT4(0.f, 0.f, 5.f, 0.f);


	
	//! THIS IS THE MARCHING CUBES
	//float sizee = 10.f;
	//sizee *= MarchCubeSettings::get()->CubeSize;
	//XMVECTOR Worker = XMVectorZero();
	//int ModelNum = 0;
	//XMFLOAT4 Pos;
	//for (float x = -sizee; x < sizee; x += MarchCubeSettings::get()->CubeSize) {
	//	Worker.m128_f32[0] = x;
	//	for (float y = -sizee; y < sizee; y += MarchCubeSettings::get()->CubeSize) {
	//		Worker.m128_f32[1] = y;
	//		for (float z = -sizee; z < sizee; z += MarchCubeSettings::get()->CubeSize) {
	//			Worker.m128_f32[2] = z;
	//
	//			MarchCube Dud(Worker);
	//
	//			if (Dud.Vertices.size() == 0) {
	//				continue;
	//			}
	//			//spdlog::info("------------------------------------");
	//			//spdlog::info("Case: {0:d} , {0:b}", Dud.Case);
	//			spdlog::info(ModelNum);
	//			//std::vector<DWORD> ind;
	//			int j = 0;
	//			//for (int i = 0; i < Dud.Vertices.size(); i += 3) {
	//			//	spdlog::info(j);
	//			//	ind.push_back(j++);
	//			//}
	//			//spdlog::info("------------------------------------");
	//
	//
	//
	//			Renderer::get()->InitializeModel(Model(Dud.Indices, Dud.Vertices));
	//
	//			XMStoreFloat4(&Pos, Worker);
	//
	//			Renderer::get()->Models[ModelNum++].Transform.Translation = Pos;
	//		}
	//	}
	//}

	// Initial position and orientation of the rigid body 
	//Vector3 position(0.0, 0.0, 0.0);
	//Quaternion orientation = Quaternion::identity();
	//Transform transform(position, orientation);
	//// Create a rigid body in the world 
	//RigidBody* body = world->createRigidBody(transform);
	//// Create the sphere shape with a radius of 2m 
	//SphereShape* sphereShape = physicsCommon.createSphereShape(2.0);
	//
	//// Relative transform of the collider relative to the body origin 
	//Transform transformm = Transform::identity();
	//// Add the collider to the rigid body 
	//Collider* collider;
	//collider = body->addCollider(sphereShape, transformm);
	//body->setType(BodyType::DYNAMIC);
	//body->enableGravity(true);
	//body->setIsAllowedToSleep(false);
	////Cam->Transform.Translation.x = 4.f;
	////Cam->Transform.Translation.y = -10.f;
	////Cam->Transform.Translation.z = -12.f;
	//
	//
	//position.y -= 10.f;
	//position.x -= 0.1f;
	//Transform transformb(position, orientation);
	//// Create a rigid body in the world 
	//RigidBody* bodyb = world->createRigidBody(transformb);
	//// Create the sphere shape with a radius of 2m 
	////SphereShape* sphereShape = physicsCommon.createSphereShape(2.0);
	//
	//// Relative transform of the collider relative to the body origin 
	////Transform transformm = Transform::identity();
	//// Add the collider to the rigid body 
	////Collider* collider;
	//collider = bodyb->addCollider(sphereShape, transformm);
	//bodyb->setType(BodyType::STATIC);
	//bodyb->enableGravity(false);
	//bodyb->setIsAllowedToSleep(false);



}

XMVECTOR Velocity = XMVectorZero();

//Game update info goes here, more advanced functionality,
//such as messaging, happens in ProCave.cpp
void BaseApp::Tick(float Deltatime)
{
	//MarchCubeSettings::get()->Time += Deltatime;
	
	
	//Cam->Transform.Translation.y -= 3.f;
	//Cam->Update();

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

	PlayerAcc = XMVector4Transform(PlayerAcc, Cam->Transform.Transform);

	XMFLOAT4 Raypos;
	XMStoreFloat4(&Raypos, XMVectorRound(NoiseRay::Test(XMLoadFloat4(&Cam->Transform.Translation), Cam->Target)));
	if(Raypos.x != 0.f)
	{
		std::vector<RigidBody*> Bodies;
		std::vector<MarchCube> Coobs;

		int Bodynum = 0;
		float AreaSize = 1.f;


		//const int nbVertices = TestCoob.Vertices.size();
		//const int nbTriangles = TestCoob.Indices.size() / 3;
		//float vertices[3 * nbVertices] = Tes;
		//int indices[3 * nbTriangles] = ...;
		for (float x = -AreaSize * MarchCubeSettings::get()->CubeSize; x <= AreaSize * MarchCubeSettings::get()->CubeSize; x += MarchCubeSettings::get()->CubeSize) {
			for (float y = -AreaSize * MarchCubeSettings::get()->CubeSize; y <= AreaSize * MarchCubeSettings::get()->CubeSize; y += MarchCubeSettings::get()->CubeSize) {
				for (float z = -AreaSize * MarchCubeSettings::get()->CubeSize; z <= AreaSize * MarchCubeSettings::get()->CubeSize; z += MarchCubeSettings::get()->CubeSize) {
					Coobs.push_back((XMLoadFloat4(&Raypos) + XMVectorSet(x, y, z, 0.f)));
					if (Coobs.back().Vertices.size() > 0) {

						TriangleVertexArray* triangleArray =
							new TriangleVertexArray(Coobs.back().Vertices.size(), &Coobs.back().LooseVertices[0], 4 * sizeof(float), Coobs.back().Indices.size() / 3,
								&Coobs.back().Indices[0], 3 * sizeof(int),
								TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
								TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE);

						TriangleMesh* triangleMesh = physicsCommon.createTriangleMesh();

						// Add the triangle vertex array to the triangle mesh 
						triangleMesh->addSubpart(triangleArray);

						// Create the concave mesh shape 
						ConcaveMeshShape* concaveMesh = physicsCommon.createConcaveMeshShape(triangleMesh);

						Vector3 position((XMLoadFloat4(&Raypos) + XMVectorSet(x, y, z, 0.f)).m128_f32[0], (XMLoadFloat4(&Raypos) + XMVectorSet(x, y, z, 0.f)).m128_f32[1], (XMLoadFloat4(&Raypos) + XMVectorSet(x, y, z, 0.f)).m128_f32[2]);
						Quaternion orientation = Quaternion::identity();
						Transform transform(position, orientation);

						// Create a rigid body in the world 
						Bodies.push_back(world->createRigidBody(transform));

						// Relative transform of the collider relative to the body origin 
						Transform transformm = Transform::identity();
						// Add the collider to the rigid body 
						
						Bodies.back()->addCollider(concaveMesh, transformm);
						Bodies.back()->setType(BodyType::STATIC);
						Bodies.back()->enableGravity(false);
						Bodies.back()->setIsAllowedToSleep(false);

						//Bodies.push_back(body);
					}
				}
			}
		}
		if (Deltatime > 0.f) { world->update(Deltatime); };

		for (int i = 0; i < Bodies.size(); i++) {
			world->destroyRigidBody(Bodies[i]);
		}
	}
	//}
	//XMStoreFloat4(, PlayerAcc + XMLoadFloat4(&Cam->Transform.Translation));
	Cam->Transform.Translation.x += PlayerAcc.m128_f32[0];
	Cam->Transform.Translation.y += PlayerAcc.m128_f32[1];
	Cam->Transform.Translation.z += PlayerAcc.m128_f32[2];

	Cam->Update();



	if (!Input::get()->GetKey(Key_MouseUnlock) && GetForegroundWindow() == HandleWindow) {
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

	Cam->Update();
	//XMStoreFloat4(&Renderer::get()->Models[0].Transform.Translation, NoiseRay::Test(XMLoadFloat4(&Cam->Transform.Translation), Cam->Target));
	//
	//
	//float Rotat = 0.01f;
	//
	//Renderer::get()->Models[0].Transform.Rotation.x += Rotat;
	//Renderer::get()->Models[0].Transform.Rotation.y += Rotat;
	//Renderer::get()->Models[0].Transform.Rotation.z += Rotat;
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
