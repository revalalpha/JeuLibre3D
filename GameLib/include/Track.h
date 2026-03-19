#pragma once
#include "ECS/Registry.h"
#include "ECS/Entities.h"
#include "Core/Window.h"
#include "GameComponents/TrackComponent.h"

using ecsType = KGR::ECS::Registry<KGR::ECS::Entity::_64, 100>;

class Track
{
public:
	void SpawnRoadPieces(ecsType& registry, KGR::RenderWindow& window, const TrackComponent& track);
	void CreateTrack(ecsType& registry, KGR::RenderWindow& window);
private:
};