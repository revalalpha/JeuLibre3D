#pragma once
#include "ECS/Registry.h"
#include "ECS/Entities.h"
#include "Core/Window.h"
#include "Audio/SoundComponent.h"

using ecsType = KGR::ECS::Registry<KGR::ECS::Entity::_64, 100>;

struct CarAudioSystem
{
	void Update(ecsType& registry, float deltaTime, KGR::RenderWindow& window);
};