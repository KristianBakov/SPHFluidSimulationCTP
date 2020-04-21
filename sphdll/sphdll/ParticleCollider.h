#pragma once
#include "Vector2.h"
#include "Vector3.h"

class ParticleCollider
{
public:
    Vector3 Position;
    Vector3 Up;
    Vector3 Right;
    Vector2 Scale;

    Vector3 RealPos;
    Vector3 RealRot;
    Vector3 RealScale;
};