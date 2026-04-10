#pragma once
#include "Core/Window.h"
#include "ECS/Registry.h"
#include "ECS/Entities.h"
//#include "ECS/Component.h"
#include "GameComponents/ScoreComponent.h"


using ecsType = KGR::ECS::Registry<KGR::ECS::Entity::_64, 100>;

struct ScoreSystem
{
	void createScore(ecsType& registry, KGR::RenderWindow& window);
	void Update(ecsType& registry, float deltaTime, KGR::RenderWindow& window);
};

struct Score
{
};
struct Mult
{
};
struct HighScore
{
};
struct MultText
{
};
struct HighScoreText
{
};