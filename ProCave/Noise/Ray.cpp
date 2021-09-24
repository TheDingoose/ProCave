#include "Ray.h"

#include "spdlog/spdlog.h"

#include "Noise/Density.h"
#include "Tools/MarchCubeSettings.h"

constexpr unsigned int MaxSteps  = 50;
constexpr float StepSize  = 0.4f;
constexpr float NormOffset = 0.1f;
constexpr float BigNormOffset = 1.f;
constexpr float MinDelta = 0.001f;

XMVECTOR LastDir = XMVectorZero();

XMVECTOR Ray::Test(XMVECTOR Pos, XMVECTOR Direction)
{
	Direction = XMVector3Normalize(Direction);
	XMVECTOR Estimation = Pos;
	float Step = StepSize;

	for (int i = 0; i < MaxSteps; i++) {
		if (GetDensity(Estimation + Direction * Step, MarchCubeSettings::get()->Time) < MarchCubeSettings::get()->DensityLimit) { //hit (Shorter step)
		Step = Step / 2;
		}
		else { //no hit, keep going
			Estimation = Estimation + (Direction * Step);
		};
	}
	return Estimation;
}

XMVECTOR Ray::GetNormal(XMVECTOR Pos)
{
	XMVECTOR Normal;
	//is using StepTest even smart?
	Normal.m128_f32[0] = GetDensity(Pos + XMVectorSet(NormOffset, 0, 0, 0)) - GetDensity(Pos + XMVectorSet(-NormOffset, 0, 0, 0));
	Normal.m128_f32[1] = GetDensity(Pos + XMVectorSet(0, NormOffset, 0, 0)) - GetDensity(Pos + XMVectorSet(0, -NormOffset, 0, 0));
	Normal.m128_f32[2] = GetDensity(Pos + XMVectorSet(0, 0, NormOffset, 0)) - GetDensity(Pos + XMVectorSet(0, 0, -NormOffset, 0));
	Normal = XMVector3Normalize(Normal);
	return Normal;
}

XMVECTOR Ray::GetNormal(XMVECTOR Pos, float Offset)
{
	XMVECTOR Normal;
	//is using StepTest even smart?
	Normal.m128_f32[0] = GetDensity(Pos + XMVectorSet(Offset, 0, 0, 0)) - GetDensity(Pos + XMVectorSet(-Offset, 0, 0, 0));
	Normal.m128_f32[1] = GetDensity(Pos + XMVectorSet(0, Offset, 0, 0)) - GetDensity(Pos + XMVectorSet(0, -Offset, 0, 0));
	Normal.m128_f32[2] = GetDensity(Pos + XMVectorSet(0, 0, Offset, 0)) - GetDensity(Pos + XMVectorSet(0, 0, -Offset, 0));
	Normal = XMVector3Normalize(Normal);
	return Normal;
}

bool Ray::HitCheck(XMVECTOR Pos, XMVECTOR Velocity)
{
	//Get the stepsize for this velocity
	float Step = XMVector3Length(Velocity).m128_f32[0] / MaxSteps;
	//Find the collision point
	XMVECTOR Direction = XMVector3Normalize(Velocity);

	//this is not the best solution, what if the hit is right at the end of the velocity vector?
	for (int i = 0; i < MaxSteps; i++) {
		if (GetDensity(Pos + Direction * Step, MarchCubeSettings::get()->Time) < MarchCubeSettings::get()->DensityLimit) { //hit (Shorter step)
			return true;
		}
		else { //no hit, keep going
			Pos = Pos + (Direction * Step);
		};
	}
	return false;
}

XMVECTOR Ray::CollisionTest(XMVECTOR Pos, XMVECTOR Velocity)
{
	//Get the stepsize for this velocity
	float Step = XMVector3Length(Velocity).m128_f32[0] / MaxSteps;
	//Find the collision point
	XMVECTOR Estimation = Pos;
	XMVECTOR Direction = XMVector3Normalize(Velocity);

	//this is not the best solution, what if the hit is right at the end of the velocity vector?
	for (int i = 0; i < MaxSteps; i++) {
		if (GetDensity(Estimation + Direction * Step, MarchCubeSettings::get()->Time) < MarchCubeSettings::get()->DensityLimit) { //hit (Shorter step)
			Step = Step / 2;
		}
		else { //no hit, keep going
			Estimation = Estimation + (Direction * Step);
		};
	}

	//No collide? just return velocity
	//if (XMVector3Length(Estimation - Pos + Velocity).m128_f32[0] < 0.01f) {
	//	return Velocity;
	//}
	//then remove the length we stepped towards the collision point, and put it towards the final velocity
	return Estimation - Pos;
}

XMVECTOR Ray::InvertedCollisionTest(XMVECTOR Pos, XMVECTOR Velocity)
{
	//Get the stepsize for this velocity
	float Step = XMVector3Length(Velocity).m128_f32[0] / MaxSteps;
	//Find the collision point
	XMVECTOR Estimation = Pos;
	XMVECTOR Direction = XMVector3Normalize(Velocity);

	//this is not the best solution, what if the hit is right at the end of the velocity vector?
	for (int i = 0; i < MaxSteps; i++) {
		if (GetDensity(Estimation + Direction * Step, MarchCubeSettings::get()->Time) > MarchCubeSettings::get()->DensityLimit) { //hit (Shorter step)
			Step = Step / 2;
		}
		else { //no hit, keep going
			Estimation = Estimation + (Direction * Step);
		};
	}

	Estimation = Estimation + (Direction * Step);

	//No collide? just return velocity
	//if (XMVector3Length(Estimation - Pos + Velocity).m128_f32[0] < 0.01f) {
	//	return Velocity;
	//}
	//then remove the length we stepped towards the collision point, and put it towards the final velocity
	return Estimation - Pos;
}



XMVECTOR Ray::DensityCollisionTest(XMVECTOR Pos, XMVECTOR Velocity)
{
	XMVECTOR FinalVelocity = XMVectorZero();

	//Get the stepsize for this velocity
	float Step = XMVector3Length(Velocity).m128_f32[0] / MaxSteps;
	float StepTest = Step / 10.f;
	//Find the collision point
	XMVECTOR Estimation = Pos;
	XMVECTOR Direction = XMVector3Normalize(Velocity);

	//this is not the best solution, what if the hit is right at the end of the velocity vector?
	for (int i = 0; i < MaxSteps; i++) {
		if (GetDensity(Estimation + Direction * Step, MarchCubeSettings::get()->Time) < MarchCubeSettings::get()->DensityLimit) { //hit (Shorter step)
			Step = Step / 2;
		}
		else { //no hit, keep going
			Estimation = Estimation + (Direction * Step);
		};
	}
	
	//No collide? just return velocity
	if (XMVector3Length(Estimation - Pos + Velocity).m128_f32[0] < 0.001f) {
		return Velocity;
	}

	//Otherwise find the normal of the terrain, by sampling 6(xyz -xyz) points and normalizing the difference
	XMVECTOR Normal = GetNormal(Estimation);

	

	//then remove the length we stepped towards the collision point, and put it towards the final velocity
	FinalVelocity = Estimation - Pos;
	Velocity -= FinalVelocity;

	

	//Project the Rest of the velocity along a plane represented by the normal
	Velocity = Velocity - (XMVector3Dot(Velocity, Normal).m128_f32[0] * Normal);

	Velocity *= 0.5f; //Scuffed Friction

	if (XMVector3Length(Velocity).m128_f32[0] > 0.1f && !XMVector3Length(Normal).m128_f32[0] == 0.f) {
		FinalVelocity += DensityCollisionTest(Estimation + (Normal * 0.01f), Velocity);
	}
	else {
		FinalVelocity += Velocity;
	}



	//FinalVelocity += Velocity;//CollisionTest(Estimation + Normal * 0.4f ,Velocity);

	FinalVelocity.m128_f32[3] = 1.f;

	return FinalVelocity;
}

void Ray::Decollide(XMVECTOR* Pos, CollisionShape Shape)
{
	XMVECTOR move = XMVectorZero();
	XMVECTOR Normal = XMVectorZero();

	for (auto i : Shape.Positions) {
		if (GetDensity(*Pos + i, MarchCubeSettings::get()->Time) < MarchCubeSettings::get()->DensityLimit) {
			Normal += Ray::GetNormal(*Pos + i);
		}
	}

	XMVECTOR BigNormal = Ray::GetNormal(*Pos, BigNormOffset);

	Normal = XMVector3Normalize(Normal);
	//Normal = XMVector3Normalize(Normal + (BigNormal * 0.2f));
	Normal.m128_f32[3] = 0.f;

	for (auto i : Shape.Positions) {
		//if (GetDensity(*Pos + i, MarchCubeSettings::get()->Time) < MarchCubeSettings::get()->DensityLimit) {
			XMVECTOR Calc = InvertedCollisionTest(*Pos + i, Normal);

			//if (XMVector3Length(Calc).m128_f32[0] > XMVector3Length(move).m128_f32[0]) { move = Calc; }
			if (fabs(move.m128_f32[0]) < fabs(Calc.m128_f32[0])) { move.m128_f32[0] = Calc.m128_f32[0]; }
			if (fabs(move.m128_f32[1]) < fabs(Calc.m128_f32[1])) { move.m128_f32[1] = Calc.m128_f32[1]; }
			if (fabs(move.m128_f32[2]) < fabs(Calc.m128_f32[2])) { move.m128_f32[2] = Calc.m128_f32[2]; }
			move.m128_f32[3] = 0.f;
		//}
	}

	spdlog::info("Decollide! {0:f}, {1:f}, {2:f}, {3:f}", move.m128_f32[0], move.m128_f32[1], move.m128_f32[2], move.m128_f32[3]);
	
	//if (XMVector3Length(move).m128_f32[0] <= 0.001f) {
	//	return;
	//}
	
	*Pos += move;
	Pos->m128_f32[3] = 0.f;

	//for (auto i : Shape.Positions) {
	//	if (GetDensity(*Pos + i, MarchCubeSettings::get()->Time) < MarchCubeSettings::get()->DensityLimit) {
	//		Decollide(Pos, Shape);
	//		return;
	//	}
	//}
}

void Ray::Decollide2(XMVECTOR* Pos, CollisionShape Shape)
{
	XMVECTOR move = XMVectorZero();
	XMVECTOR Normal = XMVectorZero();

	for (auto i : Shape.Positions) {
		if (GetDensity(*Pos + i, MarchCubeSettings::get()->Time) < MarchCubeSettings::get()->DensityLimit) {
			Normal += Ray::GetNormal(*Pos + i);
		}
	}

	//XMVECTOR BigNormal = Ray::GetNormal(*Pos, BigNormOffset);

	Normal = XMVector3Normalize(Normal);
	//Normal = XMVector3Normalize(Normal + (BigNormal * 0.2f));
	Normal.m128_f32[3] = 0.f;

	for (auto i : Shape.Positions) {
		if (GetDensity(*Pos + i, MarchCubeSettings::get()->Time) < MarchCubeSettings::get()->DensityLimit) {
			XMVECTOR Calc = InvertedCollisionTest(*Pos + i, Normal);

			//	if (XMVector3Length(Calc).m128_f32[0] > XMVector3Length(move).m128_f32[0]) { move = Calc; }
			if (fabs(move.m128_f32[0]) < fabs(Calc.m128_f32[0])) { move.m128_f32[0] = Calc.m128_f32[0]; }
			if (fabs(move.m128_f32[1]) < fabs(Calc.m128_f32[1])) { move.m128_f32[1] = Calc.m128_f32[1]; }
			if (fabs(move.m128_f32[2]) < fabs(Calc.m128_f32[2])) { move.m128_f32[2] = Calc.m128_f32[2]; }
			move.m128_f32[3] = 0.f;
		}
	}

	spdlog::info("Decollide! {0:f}, {1:f}, {2:f}, {3:f}", move.m128_f32[0], move.m128_f32[1], move.m128_f32[2], move.m128_f32[3]);

	//if (XMVector3Length(move).m128_f32[0] <= 0.001f) {
	//	return;
	//}

	*Pos += move;
	Pos->m128_f32[3] = 0.f;

	//for (auto i : Shape.Positions) {
	//	if (GetDensity(*Pos + i, MarchCubeSettings::get()->Time) < MarchCubeSettings::get()->DensityLimit) {
	//		Decollide(Pos, Shape);
	//		return;
	//	}
	//}
}

void Ray::DensityCollisionVelocityTest(XMVECTOR* Pos, XMVECTOR* Velocity, float DeltaTime)
{
	if (HitCheck(*Pos, *Velocity * DeltaTime)) {

		XMVECTOR TotalStep = *Velocity * DeltaTime;

		

		//Reduce with deltatime!

		//How long do we have before we hit the object?
		XMVECTOR FirstStep = CollisionTest(*Pos, TotalStep);
		
		Velocity->m128_f32[3] = 0.f;
		Pos->m128_f32[3] = 0.f;


		//DO A FIX HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		//if (XMVector3Length(FirstStep).m128_f32[0] < 0.00001f) { //We are inside of terrain!
		//
		//	XMVECTOR Normal = Ray::GetNormal(*Pos);
		//	Normal.m128_f32[3] = 0.f;
		//
		//	XMVECTOR move = InvertedCollisionTest(*Pos, Normal) + (Normal * 0.001f);
		//	*Pos += move;
		//	Pos->m128_f32[3] = 0.f;
		//
		//	FirstStep = CollisionTest(*Pos, TotalStep);
		//}

		

		*Pos += FirstStep;

		float DeltaStep = XMVector3Length(FirstStep).m128_f32[0] / XMVector3Length(*Velocity).m128_f32[0];

		DeltaStep = DeltaTime - DeltaStep;

		//if (DeltaStep == DeltaTime) {
		//	return;
		//}


		XMVECTOR Normal = Ray::GetNormal(*Pos + FirstStep);


		Velocity->m128_f32[3] = 0.f;
		Normal.m128_f32[3] = 0.f;
		Pos->m128_f32[3] = 0.f;

		//*Velocity = (*Velocity - (XMVector3Dot(*Velocity, Normal).m128_f32[0] * Normal)) * 0.8f;

		//The friction here should probably be inversely related with speed instead of angle here
		*Velocity = (*Velocity - (XMVector3Dot(*Velocity, Normal).m128_f32[0] * Normal)) * fmax(0.8f, (1.f +XMVector3Dot(XMVector3Normalize(*Velocity), Normal).m128_f32[0]));

		Velocity->m128_f32[3] = 0.f;
		Normal.m128_f32[3] = 0.f;
		Pos->m128_f32[3] = 0.f;
		
		//the rest of this delta step will be as if we are skidding along
		//And this should be used to calculate what amount of friction we are experiencing

		*Pos += (Normal * 0.001f);

		DensityCollisionVelocityTest(Pos, Velocity, DeltaStep);


		

		


		

		


		//if (DeltaStep > 0.00001f) {
		//	FirstStep += (RedirectVelocity * DeltaStep);
		//}


		//*Velocity = RedirectVelocity * 0.8f;

		//*Pos += FirstStep;
		return;
	}
	//Now how long are we skidding over this area?


	*Pos += *Velocity * DeltaTime;

	return;
}

void Ray::DensityCollisionVelocityTest(XMVECTOR* Pos, CollisionShape Shape, XMVECTOR* Velocity, float DeltaTime)
{
	std::vector<unsigned int> RelevantShape;
	bool Hit = false;
	for (int i = 0; i < Shape.Positions.size(); i++) {
		if (HitCheck(*Pos + Shape.Positions[i], *Velocity * DeltaTime)) {
			RelevantShape.push_back(i);
			Hit = true;
			//break;
		}
	}

	if (Hit) {

		XMVECTOR TotalStep = *Velocity * DeltaTime;

		XMVECTOR Center = XMVectorZero();

		//How long do we have before we hit the object?
		//std::vector<XMVECTOR> CalculatorHits;
		XMVECTOR FirstStep = *Velocity;
		
		for (auto i : RelevantShape) {
			//FirstStep = XMVectorMin(FirstStep, CollisionTest(*Pos + Shape.Positions[i], TotalStep));
			XMVECTOR Calc = CollisionTest(*Pos + Shape.Positions[i], TotalStep);
			
			if (fabs(FirstStep.m128_f32[0]) > fabs(Calc.m128_f32[0])) { FirstStep.m128_f32[0] = Calc.m128_f32[0]; }
			if (fabs(FirstStep.m128_f32[1]) > fabs(Calc.m128_f32[1])) { FirstStep.m128_f32[1] = Calc.m128_f32[1]; }
			if (fabs(FirstStep.m128_f32[2]) > fabs(Calc.m128_f32[2])) { FirstStep.m128_f32[2] = Calc.m128_f32[2]; }
			FirstStep.m128_f32[3] = 0.f;

			Center += Shape.Positions[i];
		}

		//spdlog::info("{0:f}, {1:f}, {2:f}, {3:f}", FirstStep.m128_f32[0], FirstStep.m128_f32[1], FirstStep.m128_f32[2], FirstStep.m128_f32[3]);

		Center = Center / RelevantShape.size();

		Velocity->m128_f32[3] = 0.f;
		Pos->m128_f32[3] = 0.f;


		//DO A FIX HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		if (XMVector3Length(FirstStep).m128_f32[0] < 0.00001f) { //We are inside of terrain!

			Decollide(Pos, Shape);

			XMVECTOR FirstStep = *Velocity;


			for (auto i : RelevantShape) {
				//FirstStep = XMVectorMin(FirstStep, CollisionTest(*Pos + Shape.Positions[i], TotalStep));
				XMVECTOR Calc = CollisionTest(*Pos + Shape.Positions[i], TotalStep);

				if (fabs(FirstStep.m128_f32[0]) > fabs(Calc.m128_f32[0])) { FirstStep.m128_f32[0] = Calc.m128_f32[0]; }
				if (fabs(FirstStep.m128_f32[1]) > fabs(Calc.m128_f32[1])) { FirstStep.m128_f32[1] = Calc.m128_f32[1]; }
				if (fabs(FirstStep.m128_f32[2]) > fabs(Calc.m128_f32[2])) { FirstStep.m128_f32[2] = Calc.m128_f32[2]; }
				FirstStep.m128_f32[3] = 0.f;
			}

			
			
		}

		float DeltaStep = XMVector3Length(FirstStep).m128_f32[0] / XMVector3Length(*Velocity).m128_f32[0];
		DeltaStep = fmax(DeltaStep, MinDelta);
		DeltaStep = DeltaTime - DeltaStep;
		


		*Pos += *Velocity * DeltaStep;

		

		//if (DeltaStep == DeltaTime) {
		//	return;
		//}

		XMVECTOR Normal = Ray::GetNormal(*Pos + FirstStep + Center);

		Velocity->m128_f32[3] = 0.f;
		Normal.m128_f32[3] = 0.f;
		Pos->m128_f32[3] = 0.f;

		*Velocity = (*Velocity - (XMVector3Dot(*Velocity, Normal).m128_f32[0] * Normal));

		//*Velocity = *Velocity - (*Velocity * (DeltaTime - DeltaStep)) * 0.5f;

	//	spdlog::info(XMVector3Dot(XMVector3Normalize(*Velocity), Normal).m128_f32[0]);

		//The friction here should probably be inversely related with speed instead of angle here
		//*Velocity = (*Velocity - (XMVector3Dot(*Velocity, Normal).m128_f32[0] * Normal)) * fmax(0.8f, (1.f + XMVector3Dot(XMVector3Normalize(*Velocity), Normal).m128_f32[0]));

		Velocity->m128_f32[3] = 0.f;
		Normal.m128_f32[3] = 0.f;
		Pos->m128_f32[3] = 0.f;

		//the rest of this delta step will be as if we are skidding along
		//And this should be used to calculate what amount of friction we are experiencing

		//*Pos += (Normal * 0.01f);

		//spdlog::info("Veloc: {0:f}, {1:f}, {2:f}, {3:f}", Velocity->m128_f32[0], Velocity->m128_f32[1], Velocity->m128_f32[2], Velocity->m128_f32[3]);

		DensityCollisionVelocityTest(Pos, Shape, Velocity, DeltaStep);

		return;
	}

	*Pos += *Velocity * DeltaTime;

	return;
}

void Ray::LoosyCollisionTest(XMVECTOR* Pos, CollisionShape Shape, XMVECTOR* Velocity, float DeltaTime)
{
	//Broad Phase
	std::vector<unsigned int> RelevantShape;
	bool Hit = false;
	for (int i = 0; i < Shape.Positions.size(); i++) {
		if (HitCheck(*Pos + Shape.Positions[i], *Velocity * DeltaTime)) {
			RelevantShape.push_back(i);
			Hit = true;
		}
	}

	//Narrow phase
	if (Hit) {

		XMVECTOR TotalStep = *Velocity * DeltaTime;

		XMVECTOR Center = XMVectorZero();

		//How long do we have before we hit the object?
		XMVECTOR FirstStep = *Velocity;

		for (auto i : RelevantShape) {
			XMVECTOR Calc = CollisionTest(*Pos + Shape.Positions[i], TotalStep);

			if (fabs(FirstStep.m128_f32[0]) > fabs(Calc.m128_f32[0])) { FirstStep.m128_f32[0] = Calc.m128_f32[0]; }
			if (fabs(FirstStep.m128_f32[1]) > fabs(Calc.m128_f32[1])) { FirstStep.m128_f32[1] = Calc.m128_f32[1]; }
			if (fabs(FirstStep.m128_f32[2]) > fabs(Calc.m128_f32[2])) { FirstStep.m128_f32[2] = Calc.m128_f32[2]; }
			FirstStep.m128_f32[3] = 0.f;

			Center += Shape.Positions[i];
		}

		//spdlog::info("{0:f}, {1:f}, {2:f}, {3:f}", FirstStep.m128_f32[0], FirstStep.m128_f32[1], FirstStep.m128_f32[2], FirstStep.m128_f32[3]);

		Center = Center / RelevantShape.size();

		Velocity->m128_f32[3] = 0.f;
		Pos->m128_f32[3] = 0.f;

		float DeltaStep = XMVector3Length(FirstStep).m128_f32[0] / XMVector3Length(*Velocity).m128_f32[0];
		DeltaStep = fmax(DeltaStep, MinDelta);
		DeltaStep = DeltaTime - DeltaStep;

		DeltaTime = fmax(DeltaTime, MinDelta);;
		if (DeltaStep <= 0.f) { return; }

		*Pos += *Velocity * DeltaStep;

		//Now do collision checks and resolve the normals as force put upon the object. Normals take the dot product of the vector as strength

		//Do we put all the normals we find together, or do we resolve them each seperately?

		//How far we go into the wall is how much force we get back?

		//All the colliding boyes should give their force

		for (auto i : Shape.Positions) {
			if (GetDensity(*Pos + i, MarchCubeSettings::get()->Time) < MarchCubeSettings::get()->DensityLimit) {
				
				XMVECTOR Normal = Ray::GetNormal(*Pos + i);
				//FRICTION!

				//This * is probably also not the correct function
				XMVECTOR NormalStrength = (Normal * -fmin(XMVector3Dot(*Velocity, Normal).m128_f32[0], 0.f)) * (DeltaStep / DeltaTime);
				
				//float P = -XMVector3Dot(*Velocity, Normal).m128_f32[0];
				//if (P < 0.1f) {
				//	P = 0.f;
				//}
				//XMVECTOR NormalStrength = (Normal * P) * (DeltaStep / DeltaTime);
				NormalStrength.m128_f32[3] = 0.f;


				//spdlog::info("Loosy! {0:f}, {1:f}, {2:f}, {3:f}", NormalStrength.m128_f32[0], NormalStrength.m128_f32[1], NormalStrength.m128_f32[2], NormalStrength.m128_f32[3]);
				*Velocity = ((*Velocity) + NormalStrength);
				

			}
		}
		Velocity->m128_f32[3] = 0.f;
		//spdlog::info("Loosy! {0:f}, {1:f}, {2:f}, {3:f}", Velocity->m128_f32[0], Velocity->m128_f32[1], Velocity->m128_f32[2], Velocity->m128_f32[3]);

		////////////////////////////////////////////////
		///////////////////////////////////////////////
		//XMVECTOR Normal = XMVectorZero();
		//
		//for (auto i : Shape.Positions) {
		//	if (GetDensity(*Pos + i, MarchCubeSettings::get()->Time) < MarchCubeSettings::get()->DensityLimit) {
		//
		//		Normal += Ray::GetNormal(*Pos + i);
		//
		//
		//	}
		//}
		//
		//
		////FRICTION!
		//Normal = XMVector3Normalize(Normal);
		////This * is probably also not the correct function
		//XMVECTOR NormalStrength = (Normal * -fmin(XMVector3Dot(*Velocity, Normal).m128_f32[0], 0.f)); //* (DeltaStep / DeltaTime);
		////spdlog::info("Loosy! {0:f}, {1:f}, {2:f}, {3:f}", NormalStrength.m128_f32[0], NormalStrength.m128_f32[1], NormalStrength.m128_f32[2], NormalStrength.m128_f32[3]);
		//*Velocity = ((*Velocity) + NormalStrength);
		//
		//Velocity->m128_f32[3] = 0.f;
		//spdlog::info("Loosy! {0:f}, {1:f}, {2:f}, {3:f}", Velocity->m128_f32[0], Velocity->m128_f32[1], Velocity->m128_f32[2], Velocity->m128_f32[3]);
		//////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////



		LoosyCollisionTest(Pos, Shape, Velocity, DeltaStep);

		return;
	}

	*Pos += *Velocity * DeltaTime;

	return;
}
