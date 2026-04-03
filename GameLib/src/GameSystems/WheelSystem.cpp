#include "GameSystems/WheelSystem.h"
#include "GameComponents/WheelComponent.h"
#include "Core/TrasformComponent.h"
#include <glm/glm.hpp>
#include "Core/Mesh.h"

void WheelSystem::Update(ecsType& registry, float deltaTime)
{
	auto e = registry.GetAllComponentsView<WheelComponent, TransformComponent>();
	for (auto entity : e)
	{
		auto& wheel = registry.GetComponent<WheelComponent>(entity);

        if (wheel.isSteerable)
        {
			float angle = 30.0f;
            float maxSteer = glm::radians(angle);
            wheel.steerAngle = wheel.steerInput * maxSteer;
        }

        wheel.angularVelocity += (wheel.motorTorque * deltaTime);
        wheel.angularVelocity -= (wheel.brakeTorque * deltaTime);
        wheel.angularVelocity *= (1.0f - wheel.rollingResistance * deltaTime);

        wheel.speed = wheel.angularVelocity * wheel.radius;
        wheel.slipRatio = (wheel.speed - wheel.carSpeed) / std::max(std::abs(wheel.carSpeed), 0.1f);
	}
}

void WheelSystem::Visualize(ecsType& registry, float deltaTime)
{
    auto wheels = registry.GetAllComponentsView<WheelComponent, TransformComponent>();

    for (auto e : wheels)
    {
        auto& w = registry.GetComponent<WheelComponent>(e);
        auto& transform = registry.GetComponent<TransformComponent>(e);

        auto& bodyTr = registry.GetComponent<TransformComponent>(w.carBody);

        glm::vec3 carPos = bodyTr.GetPosition();
        glm::vec3 carRot = bodyTr.GetRotation();

		//Body rotation
        glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), carRot.y, glm::vec3(0, 1, 0));

		//Wheel position
        glm::vec3 worldOffset = glm::vec3(rotY * glm::vec4(w.visualOffset, 1.0f));
        transform.SetPosition(carPos + worldOffset);

		//Wheel rotation
        glm::mat4 wheelRot = rotY;

        if (w.isSteerable)
            wheelRot = glm::rotate(wheelRot, w.steerAngle, glm::vec3(0, 1, 0));

        wheelRot = glm::rotate(wheelRot, w.angularVelocity * deltaTime, glm::vec3(1, 0, 0));

        transform.SetRotation(glm::eulerAngles(glm::quat(wheelRot)));
    }
}
