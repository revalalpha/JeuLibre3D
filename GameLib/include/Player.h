#pragma once
#include "Core/Window.h"
#include "ECS/Registry.h"
#include "ECS/Entities.h"
#include "ECS/Component.h"

struct PlayerComponent
{

};

using ecsType = KGR::ECS::Registry<KGR::ECS::Entity::_64, 100>;

class Player
{
public:
	Player() = default;
	~Player() = default;

	void CreatePlayer(ecsType& registry, KGR::RenderWindow& window);
	void Update(ecsType& registry, float deltaTime);
private:
};
