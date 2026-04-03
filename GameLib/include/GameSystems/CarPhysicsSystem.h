#pragma once
#include "ECS/Registry.h"
#include "ECS/Entities.h"

//All the physics of the car

using ecsType = KGR::ECS::Registry<KGR::ECS::Entity::_64, 100>;

struct CarPhysicsSystem
{
	void Update(ecsType& registry, float deltaTime);
};