#pragma once
#include "ECS/Registry.h"
#include "ECS/Entities.h"
#include "Core/Window.h"

using ecsType = KGR::ECS::Registry<KGR::ECS::Entity::_64, 100>;

struct CarCollisionSystem
{
	void Update(ecsType& registry, KGR::RenderWindow& window, float deltaTime, bool debugCollision);
};