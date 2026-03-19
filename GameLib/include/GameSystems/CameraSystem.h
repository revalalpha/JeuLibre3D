#pragma once
#include "ECS/Registry.h"
#include "ECS/Entities.h"
#include "Core/Window.h"
#include "ECS/Component.h"

using ecsType = KGR::ECS::Registry<KGR::ECS::Entity::_64, 100>;

struct CameraSystem
{
	void Update(ecsType& registry, float deltaTime);
};
