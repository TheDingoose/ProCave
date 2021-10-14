#pragma once

class EnvironmentCollider
{
public:
	EnvironmentCollider(unsigned int a_ID, float a_Radius = 1.f) { ID = a_ID; Radius = a_Radius; };
	EnvironmentCollider() { };

	//What is my ID in the collisionworld?
	unsigned int ID;

	//In what radius around me should I generate coobs?
	float Radius = 1.f;

	//A pointer to where rendertranslation should be?

};

