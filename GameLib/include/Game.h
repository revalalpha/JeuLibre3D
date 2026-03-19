//Code Your Game Here
#pragma once
#include <string>
#include <vector>

#include "Core/Window.h"
#include "ECS/Entities.h"
#include "ECS/Registry.h"
#include "Tools/Chrono.h"

// to move 
struct ControllerComponent {};

using ecsType = KGR::ECS::Registry<KGR::ECS::Entity::_64, 100>;

struct Game
{
	void Init(const std::string& fp);
	void UpdateGame(float dt);
	void Run(const KGR::Tools::Chrono<float>::Time& fixedTime);
	void Render();
private:
	std::unique_ptr<KGR::RenderWindow> window;
	ecsType registry = ecsType{};
};
