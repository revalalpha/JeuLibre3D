#pragma once
#include "ECS/Registry.h"
#include "ECS/Entities.h"

using ecsType = KGR::ECS::Registry<KGR::ECS::Entity::_64, 100>;

struct WheelSystem
{
	void Update(ecsType& registry, float deltaTime);
	void Visualize(ecsType& registry, float deltaTime);
};