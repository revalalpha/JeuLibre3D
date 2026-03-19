#pragma once
#include "ECS/Registry.h"
#include "ECS/Entities.h"
#include "Core/Window.h"

using ecsType = KGR::ECS::Registry<KGR::ECS::Entity::_64, 100>;

struct GameRenderer
{
	void RenderMesh(ecsType& registry, KGR::RenderWindow& window);
	/*void RenderUi(ecsType& registry, KGR::RenderWindow& window);*/
	void RenderLight(ecsType& registry, KGR::RenderWindow& window);
	void RenderCam(ecsType& registry, KGR::RenderWindow& window);
};