#pragma once
#include "ECS/Registry.h"
#include "GameComponents/CarControllerComponent.h"
#include "GameComponents/DriftComponent.h"

struct DriftSystem
{
	void Update(float deltaTime);
};