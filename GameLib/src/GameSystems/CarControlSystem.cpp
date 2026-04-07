#include "GameSystems/CarControlSystem.h"
#include "GameComponents/CarControllerComponent.h"
#include "Core/TrasformComponent.h"
#include "Core/InputManager.h"
#include "GameComponents/WheelComponent.h"
#include "GameComponents/CarPhysicsComponent.h"

#include <print>

static constexpr float kSteerDamping = 0.2f;

void CarControlSystem::Update(ecsType& registry, KGR::RenderWindow& window, float deltaTime)
{
	auto e = registry.GetAllComponentsView<CarControllerComponent>();
	auto input = window.GetInputManager();
	for (auto entity : e)
	{
		auto& controller = registry.GetComponent<CarControllerComponent>(entity);

		float accelerationInput = 0.0f;
		float steeringInput = 0.0f;

		//ZQSD and brake control
		if (input->IsKeyDown(KGR::Key::Z))
		{
			accelerationInput += 1.0f;
		}
		if (input->IsKeyDown(KGR::Key::S))
		{
			accelerationInput = -1.0f;
		}
		if (input->IsKeyDown(KGR::SpecialKey::Space))
		{
			if(controller.speed > 0.0f)
				accelerationInput -= 1.0f;
		}
		if (input->IsKeyDown(KGR::Key::Q))
		{
			steeringInput = 1.0f;
		}
		if (input->IsKeyDown(KGR::Key::D))
		{
			steeringInput = -1.0f;
		}

		controller.acceleration = accelerationInput;
		controller.steering = glm::mix(controller.steering, steeringInput, kSteerDamping);
		//std::println("steeringInput : {}, controller.steering : {}", steeringInput, controller.steering);

		auto& phys = registry.GetComponent<CarPhysicsComponent>(entity);

		for (auto w : phys.wheels)
		{
			auto& wheel = registry.GetComponent<WheelComponent>(w);

			if (wheel.isSteerable)
				wheel.steerInput = glm::mix(wheel.steerInput, steeringInput, kSteerDamping);
		}
	}
}