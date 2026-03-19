#include "GameSystems/CarControlSystem.h"
#include "GameComponents/CarControllerComponent.h"
#include "Core/TrasformComponent.h"
#include "Core/InputManager.h"

void CarControlSystem::Update(ecsType& registry, KGR::RenderWindow& window, float deltaTime)
{
	auto e = registry.GetAllComponentsView<CarControllerComponent, TransformComponent>();
	auto input = window.GetInputManager();
	for (auto entity : e)
	{
		auto& controller = registry.GetComponent<CarControllerComponent>(entity);
		auto& transform = registry.GetComponent<TransformComponent>(entity);

		float accelerationInput = 0.0f;
		float steeringInput = 0.0f;
		float turnInput = 0.0f;

		//ZQSD control
		if (input->IsKeyDown(KGR::Key::Z))
			accelerationInput = 1.0f;
		if (input->IsKeyDown(KGR::Key::S))
			accelerationInput = -1.0f;
		if (input->IsKeyDown(KGR::Key::Q))
			steeringInput = 1.0f;
		if (input->IsKeyDown(KGR::Key::D))
			steeringInput = -1.0f;
		
		controller.acceleration = accelerationInput;
		controller.steering = steeringInput;
		controller.speed = glm::length(transform.GetLocalAxe<RotData::Dir::Forward>() * controller.acceleration * controller.maxSpeed);
		
		//Rotation movement
		turnInput = steeringInput * controller.turnSpeed * deltaTime * (controller.speed / controller.maxSpeed);
		transform.RotateEuler<RotData::Orientation::Yaw>(turnInput);

		//Forward movement
		glm::vec3 forward = -transform.GetLocalAxe<RotData::Dir::Forward>();
		transform.Translate(forward * controller.speed * deltaTime);
	}
}