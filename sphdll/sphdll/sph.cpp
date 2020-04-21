#include "pch.h"
#include "sph.h"
#include <cmath>

//Consts
const float Pi = 3.141593f;
const static int Amount = 1000;
const float GAS = 2000.0f;
const Vector3 gravity(0.0f, -9.81f, 0.0f);
const float DT = 0.0008f;
const float BOUND_DAMPING = -0.5f;

SPHParameters parameters[1];
//Globals
int cellSize = 1;
SpatialHash hash{ cellSize };
Particle* particles[Amount];
ParticleCollider colliders[5];
int rowSize = 3;
bool clearing;

std::vector<Particle> neighbourParticles;
int parameterID = 0;

void SetParamsDefault()
{
    parameters[parameterID].particleRadius = 1;
    parameters[parameterID].smoothingRadius = 1;
    parameters[parameterID].smoothingRadiusSq = 1;
    parameters[parameterID].restDensity = 45;
    parameters[parameterID].gravityMult = 2000;
    parameters[parameterID].particleMass = 0.1f;
    parameters[parameterID].particleViscosity = 1;
    parameters[parameterID].particleDrag = 0.025f;
}

void ClearHash()
{
    hash.Clear();
}

void Initialize()
{
    
    for (int i = 0; i < Amount; i++)
    {
        particles[i] = new Particle();

        //randomize
        float jitter = (randdouble(0.0f, 1.0f) * 2.0f - 1.0f) * parameters[parameterID].particleRadius * 0.1f;

        //calculate random range in a cube
        float x = (i % rowSize) + randdouble(-0.1f, 0.1f);
        float y = 2 + (float)((i / rowSize) / rowSize) * 1.1f;
        float z = ((i / rowSize) % rowSize) + randdouble(-0.1f, 0.1f);

        particles[i]->position = Vector3(x + jitter, y, z + jitter);
        particles[i]->scale = Vector3(1, 1, 1) * parameters[parameterID].particleRadius;

        hash.Insert(particles[i]->position, *particles[i]);
    }
    //for (int i = 0; i < Amount; i++)
    //{
    //    particles[i] = new Particle();
    //    //calculate random range in a cube
    //    float x = (i % rowSize) + randdouble(-0.1f, 0.1f);
    //    float y = (2 * parameters[parameterID].particleRadius) + (float)((i / rowSize) / rowSize) * 1.1f;
    //    float z = ((i / rowSize) % rowSize) + randdouble(-0.1f, 0.1f);

    //    particles[i]->position = Vector3(x, y, z);
    //    particles[i]->scale = Vector3(1, 1, 1) * parameters[parameterID].particleRadius;
    //}
}
void InitColliders()
{
    //set all the values for collider's transform and calculations here
    colliders[0].RealPos = colliders[0].Position = Vector3(0, 0, 0);
    colliders[1].RealPos = colliders[1].Position = Vector3(0, 15, 20);
    colliders[2].RealPos = colliders[2].Position = Vector3(20, 15, 0);
    colliders[3].RealPos = colliders[3].Position = Vector3(-20, 15, 0);
    colliders[4].RealPos = colliders[4].Position = Vector3(0, 15, -20);

    colliders[0].RealRot = Vector3(90, 0, 0);
    colliders[1].RealRot = Vector3(0, 0, 0);
    colliders[2].RealRot = Vector3(0, 90, 0);
    colliders[3].RealRot = Vector3(0, -90, 0);
    colliders[4].RealRot = Vector3(0, 180, 0);

    colliders[0].RealScale = Vector3(40, 40, 1);
    colliders[1].RealScale = Vector3(40, 30, 1);
    colliders[2].RealScale = Vector3(40, 30, 1);
    colliders[3].RealScale = Vector3(40, 30, 1);
    colliders[4].RealScale = Vector3(40, 30, 1);

    colliders[0].Up = Vector3(0, 0, 1);
    colliders[1].Up = Vector3(0, 1, 0);
    colliders[2].Up = Vector3(0, 1, 0);
    colliders[3].Up = Vector3(0, 1, 0);
    colliders[4].Up = Vector3(0, 1, 0);

    colliders[0].Right = Vector3(1, 0, 0);
    colliders[1].Right = Vector3(1, 0, 0);
    colliders[2].Right = Vector3(0, 0, -1);
    colliders[3].Right = Vector3(0, 0, 1);
    colliders[4].Right = Vector3(-1, 0, 0);

    colliders[0].Scale = Vector2(colliders[0].RealScale.x / 2.0f, colliders[0].RealScale.y / 2.0f);
    colliders[1].Scale = Vector2(colliders[1].RealScale.x / 2.0f, colliders[1].RealScale.y / 2.0f);
    colliders[2].Scale = Vector2(colliders[2].RealScale.x / 2.0f, colliders[2].RealScale.y / 2.0f);
    colliders[3].Scale = Vector2(colliders[3].RealScale.x / 2.0f, colliders[3].RealScale.y / 2.0f);
    colliders[4].Scale = Vector2(colliders[4].RealScale.x / 2.0f, colliders[4].RealScale.y / 2.0f);
}
void Clear()
{

    if (!clearing)
    {
        clearing = true;
    }

    for (int i = 0; i < Amount; i++)
    {
        delete particles[i];
        particles[i] = nullptr;
        hash.Reset();
    }
    Initialize();
    InitColliders();
    clearing = false;

}

void CalculateDensityAndPressure()
{
    //for (int i = 0; i < Amount; i++)
    //{

    //    if (clearing)
    //    {
    //        return;
    //    }

    //    for (int j = 0; j < Amount; j++)
    //    {
    //        Vector3 direction = particles[j]->position - particles[i]->position;
    //        float distance = direction.Length();

    //        particles[i]->density = ParticleDensity(*particles[i], distance);
    //        particles[i]->pressure = GAS * (particles[i]->density - 15);
    //    }
    //}
    for (int i = 0; i < Amount; i++)
    {
        if (clearing)
        {
            return;
        }

        hash.Insert(particles[i]->position, *particles[i]);
        particles[i]->density = 0.0f;

        neighbourParticles = hash.GetParticlesFromBins(hash.GetNeighbourBinsFromPosition(particles[i]->position));

        for (int j = 0; j < neighbourParticles.size(); j++)
        {
            Vector3 dist = neighbourParticles[j].position - particles[i]->position;
            float r2 = dist.LengthSqr();

            if (r2 < parameters[particles[i]->parameterID].smoothingRadiusSq)
            {
                particles[i]->density += parameters[parameterID].particleMass *
                    (315.0f / (64.0f * Pi * pow(parameters[parameterID].smoothingRadius, 9.0f))) *
                    pow(parameters[parameterID].smoothingRadiusSq - r2, 3.0f);
            }
        }

        particles[i]->pressure = GAS * (particles[i]->density - parameters[parameterID].restDensity);
    }
}

void CalculateForces()
{
    //for (int i = 0; i < Amount; i++)
    //{

    //    if (clearing)
    //    {
    //        return;
    //    }

    //    Vector3 forcePressure = Vector3(0, 0, 0);
    //    Vector3 forceViscosity = Vector3(0, 0, 0);

    //    for (int j = 0; j < Amount; j++)
    //    {
    //        if (i == j) continue;

    //        Vector3 direction = particles[j]->Position - particles[i]->Position;
    //        float distance = direction.Length();

    //        forcePressure += ParticlePressure(*particles[i], *particles[j], direction, distance);
    //        forceViscosity += ParticleViscosity(*particles[i], *particles[j], distance);
    //    }

    //    Vector3 forceGravity = gravity * particles[i]->Density * gravityMultiplicator;

    //    particles[i]->CombinedForce = forcePressure + forceViscosity + forceGravity;
    //    particles[i]->Velocity += ((particles[i]->CombinedForce) / particles[i]->Density) * deltaTime;
    //    particles[i]->Position += (particles[i]->Velocity) * deltaTime;
    //    //make it return Vector3 type. Then, return particles[i]->Position. Assign that value to the particles in C#.
    //    //particles[i]->Go.transform.position = particles[i]->Position;
    //}

    for (int i = 0; i < Amount; i++)
    {
        if (clearing)
        {
            return;
        }
        Vector3 forcePressure = Vector3(0,0,0);
        Vector3 forceViscosity = Vector3(0,0,0);

        //cache
        Vector3 position = particles[i]->position;
        Vector3 velocity = particles[i]->velocity;
        float pressure = particles[i]->pressure;
        float density = particles[i]->density;

        neighbourParticles = hash.GetParticlesFromBins(hash.GetNeighbourBinsFromPosition(particles[i]->position));

        // Physics
        for (int j = 0; j < neighbourParticles.size(); j++)
        {
            if (i == j) continue;

            Vector3 rij = neighbourParticles[j].position - position;
            float r2 = rij.LengthSqr();
            float r = sqrt(r2);

            if (r < parameters[parameterID].smoothingRadius)
            {
                forcePressure += rij.Normalize() * -1 * parameters[parameterID].particleMass *
                    (2.0f * pressure) / (2.0f * density) *
                    (-45.0f / (Pi * pow(parameters[parameterID].smoothingRadius, 6.0f))) *
                    pow(parameters[particles[i]->parameterID].smoothingRadius - r, 2.0f);

                forceViscosity += (neighbourParticles[j].velocity - velocity) * 
                    parameters[parameterID].particleViscosity *
                    parameters[parameterID].particleMass / density *
                    (45.0f / (Pi * pow(parameters[parameterID].smoothingRadius, 6.0f))) *
                    (parameters[parameterID].smoothingRadius - r);
            }
        }

        Vector3 forceGravity = gravity * density * parameters[parameterID].gravityMult;

        // Apply
        particles[i]->forceCombined = forcePressure + forceViscosity + forceGravity;
    }
}

void Integrate()
{
    for (int i = 0; i < Amount; i++)
    {
        // Cache
        Vector3 velocity = particles[i]->velocity;
        Vector3 position = particles[i]->position;

        // Process
        velocity += particles[i]->forceCombined / particles[i]->density * DT;
        position += velocity * DT;

        // Apply
        particles[i]->velocity = velocity;
        particles[i]->position = position;
    }
}

static bool Collision(ParticleCollider collider, Vector3 position, float radius, Vector3& penetrationNormal,
    Vector3& penetrationPosition, float& penetrationLength)
{
    Vector3 colliderProjection = collider.Position - position;

    penetrationNormal = penetrationNormal.CrossProduct(collider.Right, collider.Up);
    penetrationLength = abs(colliderProjection.Dot(penetrationNormal)) - (radius / 2.0f);
    penetrationPosition = collider.Position - colliderProjection;

    return penetrationLength < 0.0f
        && abs(colliderProjection.Dot(collider.Right)) < collider.Scale.x
        && abs(colliderProjection.Dot(collider.Up)) < collider.Scale.y;
}

Vector3 DampenVelocity(ParticleCollider collider, Vector3 velocity, Vector3 penetrationNormal, float drag)
{
    Vector3 newVelocity = penetrationNormal * velocity.Dot(penetrationNormal) * BOUND_DAMPING +
        collider.Right * velocity.Dot(collider.Right) * drag +
        collider.Up * velocity.Dot(collider.Up) * drag;

    return Vector3(0, 0, 1) * newVelocity.Dot(Vector3(0, 0, 1)) + Vector3(1, 0, 0) * newVelocity.Dot(Vector3(1, 0, 0))
        + Vector3(0, 1, 0) * newVelocity.Dot(Vector3(0, 1, 0));
}

void CalculateCollisions()
{
    //for (int i = 0; i < Amount; i++)
    //{
    //    for (int j = 0; j < 5; j++)
    //    {
    //        if (clearing || sizeof(colliders) == 0)
    //        {
    //            return;
    //        }

    //        Vector3 penetrationNormal;
    //        Vector3 penetrationPosition;
    //        float penetrationLength;
    //        if (Collision(colliders[j], particles[i]->Position, Radius, *&penetrationNormal, *&penetrationPosition, *&penetrationLength))
    //        {
    //            particles[i]->Velocity = DampenVelocity(colliders[j], particles[i]->Velocity, penetrationNormal, 1.0f - Drag);
    //            particles[i]->Position = penetrationPosition - penetrationNormal * abs(penetrationLength);
    //        }
    //    }
    //}

    for (int i = 0; i < Amount; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (clearing || sizeof(colliders) == 0)
            {
                return;
            }

            // Check collision
            Vector3 penetrationNormal;
            Vector3 penetrationPosition;
            float penetrationLength;
            if (Collision(colliders[j], particles[i]->position, parameters[particles[i]->parameterID].particleRadius,
                *&penetrationNormal, *&penetrationPosition, *&penetrationLength))
            {
                particles[i]->velocity = DampenVelocity(colliders[j], particles[i]->velocity, penetrationNormal,
                    1.0f - parameters[particles[i]->parameterID].particleDrag);
                particles[i]->position = penetrationPosition - penetrationNormal * abs(penetrationLength);
            }
        }
    }
}

void SetClearing(bool b_in)
{
    clearing = b_in;
}

bool GetClearing()
{
    return clearing;
}

Vector3 GetColliderRealPos(int num_in)
{
    return colliders[num_in].RealPos;
}

Vector3 GetColliderRealRot(int num_in)
{
    return colliders[num_in].RealRot;
}

Vector3 GetColliderRealScale(int num_in)
{
    return colliders[num_in].RealScale;
}

void SetColliderPosition(int num_in, Vector3 v_in)
{
    colliders[num_in].Position = v_in;
}

void SetColliderUp(int num_in, Vector3 v_in)
{
    colliders[num_in].Up = v_in;
}

void SetColliderRight(int num_in, Vector3 v_in)
{
    colliders[num_in].Right = v_in;
}

void SetColliderScale(int num_in, Vector2 v_in)
{
    colliders[num_in].Scale = v_in;
}

void SetPerRow(int i_in)
{
    rowSize = i_in;
}

int GetPerRow()
{
    return rowSize;
}

void SetCellSize(int i_in)
{
    cellSize = i_in;
}

int GetCellSize()
{
    return cellSize;
}

Vector3 GetColliderPosition(int num_in)
{
    return colliders[num_in].Position;
}

Vector3 GetColliderUp(int num_in)
{
    return colliders[num_in].Up;
}

Vector3 GetColliderRight(int num_in)
{
    return colliders[num_in].Right;
}

Vector2 GetColliderScale(int num_in)
{
    return colliders[num_in].Scale;
}

Vector3 GetparticlePosition(int num_in)
{
    return particles[num_in]->position;
}

Vector3 GetParticleScale(int num_in)
{
    return particles[num_in]->scale;
}

float GetParticleRadius()
{
    return parameters[parameterID].particleRadius;
}
float GetSmoothingRadius()
{
    return parameters[parameterID].smoothingRadius;
}
float GetSmoothingRadiusSq()
{
    return parameters[parameterID].smoothingRadiusSq;
}
float GetRestDensity()
{
    return parameters[parameterID].restDensity;
}
float GetGravityMultiplier()
{
    return parameters[parameterID].gravityMult;
}
float GetParticleMass()
{
    return parameters[parameterID].particleMass;
}
float GetParticleViscosity()
{
    return parameters[parameterID].particleViscosity;
}
float GetParticleDrag()
{
    return parameters[parameterID].particleDrag;
}

void SetParticleRadius(float val_in)
{
    parameters[parameterID].particleRadius = val_in;
}
void SetSmoothingRadius(float val_in)
{
    parameters[parameterID].smoothingRadius = val_in;
}
void SetSmoothingRadiusSq(float val_in)
{
    parameters[parameterID].smoothingRadiusSq = val_in;
}
void SetRestDensity(float val_in)
{
    parameters[parameterID].restDensity = val_in;
}
void SetGravityMultiplier(float val_in)
{
    parameters[parameterID].gravityMult = val_in;
}
void SetParticleMass(float val_in)
{
    parameters[parameterID].particleMass = val_in;
}
void SetParticleViscosity(float val_in)
{
    parameters[parameterID].particleViscosity = val_in;
}
void SetParticleDrag(float val_in)
{
    parameters[parameterID].particleDrag = val_in;
}

Vector3 V3abs(Vector3 v_in)
{
	v_in.x = std::abs(v_in.x);
	v_in.y = std::abs(v_in.y);
	v_in.z = std::abs(v_in.z);
	return v_in;
}