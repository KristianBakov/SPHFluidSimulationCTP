#pragma once
#include "Vector3.h"
class Particle
{
public:
	Vector3 scale;
	Vector3 position;

	Vector3 velocity;
	Vector3 forceCombined;

	float density;
	float pressure;

	int parameterID;

	bool operator <(const Particle& rhs) const
	{
		return density < rhs.density;
	}
};