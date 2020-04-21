#pragma once
#ifdef SPHDLL_EXPORTS
#define DLLEXPORT __declspec(dllexport)  
#else
#define DLLEXPORT __declspec(dllimport)  
#endif

#include "Random.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Particle.h"
#include "SpatialHash.h"
#include "ParticleCollider.h"
#include "SPHParameters.h"

extern "C"
{

	DLLEXPORT void ClearHash();
	DLLEXPORT void SetCellSize(int i_in);
	DLLEXPORT int GetCellSize();

	DLLEXPORT Vector3 V3abs(Vector3 v_in);
	DLLEXPORT void Initialize();
	DLLEXPORT void InitColliders();
	DLLEXPORT void Clear();
	DLLEXPORT Vector3 GetParticleScale(int num_in);
	DLLEXPORT Vector3 GetparticlePosition(int num_in);
	DLLEXPORT void SetParamsDefault();
	DLLEXPORT void CalculateDensityAndPressure();
	DLLEXPORT void CalculateForces();
	DLLEXPORT void Integrate();
	DLLEXPORT void CalculateCollisions();
	DLLEXPORT void SetClearing(bool b_in);
	DLLEXPORT bool GetClearing();

	DLLEXPORT float GetParticleRadius();
	DLLEXPORT float GetSmoothingRadius();
	DLLEXPORT float GetSmoothingRadiusSq();
	DLLEXPORT float GetRestDensity();
	DLLEXPORT float GetGravityMultiplier();
	DLLEXPORT float GetParticleMass();
	DLLEXPORT float GetParticleViscosity();
	DLLEXPORT float GetParticleDrag();

	DLLEXPORT void SetParticleRadius(float val_in);
	DLLEXPORT void SetSmoothingRadius(float val_in);
	DLLEXPORT void SetSmoothingRadiusSq(float val_in);
	DLLEXPORT void SetRestDensity(float val_in);
	DLLEXPORT void SetGravityMultiplier(float val_in);
	DLLEXPORT void SetParticleMass(float val_in);
	DLLEXPORT void SetParticleViscosity(float val_in);
	DLLEXPORT void SetParticleDrag(float val_in);

	DLLEXPORT Vector3 GetColliderRealPos(int num_in);
	DLLEXPORT Vector3 GetColliderRealRot(int num_in);
	DLLEXPORT Vector3 GetColliderRealScale(int num_in);
	DLLEXPORT Vector3 GetColliderPosition(int num_in);
	DLLEXPORT Vector3 GetColliderUp(int num_in);
	DLLEXPORT Vector3 GetColliderRight(int num_in);
	DLLEXPORT Vector2 GetColliderScale(int num_in);
	DLLEXPORT void SetColliderPosition(int num_in, Vector3 v_in);
	DLLEXPORT void SetColliderUp(int num_in, Vector3 v_in);
	DLLEXPORT void SetColliderRight(int num_in, Vector3 v_in);
	DLLEXPORT void SetColliderScale(int num_in, Vector2 v_in);
	DLLEXPORT void SetPerRow(int i_in);
	DLLEXPORT int GetPerRow();
}