#pragma once
#include <string>
#include <vector>

#include "Core/Window.h"
#include "ECS/Entities.h"
#include "ECS/Registry.h"
#include "Tools/Chrono.h"

enum class GameState
{
	Playing,
	Paused,
	Menu
};

// to move 
struct ControllerComponent {};

using ecsType = KGR::ECS::Registry<KGR::ECS::Entity::_64, 100>;

struct Game
{
	void Init(const std::string& fp);
	void UpdateGame(float dt);
	void Run(const KGR::Tools::Chrono<float>::Time& fixedTime);
	void Render();

	void CreateMenu();
	void RenderMainMenu();

	GameState state = GameState::Menu;

private:
	std::unique_ptr<KGR::RenderWindow> window;
	ecsType registry = ecsType{};

	KGR::ECS::Entity::_64 menuBG;
	KGR::ECS::Entity::_64 menuCam{};

};