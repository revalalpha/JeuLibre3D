#pragma once
#include "ECS/Registry.h"
#include "Physic/MyPhysic.h"
#include "Math/MyMath.h"

//All the physics of the car
struct PhysicsSystem
{
	void Update(float deltaTime);
};