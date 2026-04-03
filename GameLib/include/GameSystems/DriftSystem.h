#pragma once
#include "ECS/Registry.h"
#include "ECS/Entities.h"
#include "GameComponents/CarControllerComponent.h"
#include "GameComponents/DriftComponent.h"

using ecsType = KGR::ECS::Registry<KGR::ECS::Entity::_64, 100>;

struct DriftSystem
{
	void Update(ecsType& registry, float deltaTime);
};