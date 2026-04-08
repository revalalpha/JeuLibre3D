#pragma once
#include "ECS/Registry.h"
#include "ECS/Entities.h"
#include "ECS/Component.h"
#include "Audio/SoundComponent.h"
#include "GameSystems/CarAudioSystem.h"

using ecsType = KGR::ECS::Registry<KGR::ECS::Entity::_64, 100>;

class GameAudio
{
public:
	void Create(ecsType& registry);

	void Update(ecsType& registry, float deltaTime, KGR::RenderWindow& window);

private:
	float timer = 0.0f;
	CarAudioSystem audioSystem;
};