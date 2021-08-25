#include "Ray.h"

#include "Noise/Density.h"
#include "Tools/MarchCubeSettings.h"

constexpr unsigned int MaxSteps  = 50;
constexpr float StepSize  = 0.4f;

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
