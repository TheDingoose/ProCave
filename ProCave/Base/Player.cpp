#include "Player.h"

using namespace rp3d;
using namespace DirectX;

Player::Player(RigidBody* a_Body)
{
	Collisionbody = a_Body;
}

void Player::HandleInput(rp3d::Vector3 a_Input)
{
	Collisionbody->applyForceToCenterOfMass(a_Input * Speed);
	//Collisionbody->setLinearVelocity(Vector3(a_Input.x * Speed, Collisionbody->getLinearVelocity().y, a_Input.z * Speed));
}

void Player::SyncPos(cTransform* a_TransfromtoSync)
{
	Vector3 point = Collisionbody->getTransform().getPosition();
	a_TransfromtoSync->Translation = XMFLOAT4(point.x, point.y + 0.6f, point.z, 1.f);
	a_TransfromtoSync->UpdateMatrix();
}
