#include "GameSystems/WheelSystem.h"
#include "GameComponents/WheelComponent.h"
#include "Core/TrasformComponent.h"
#include "GameComponents/CarControllerComponent.h"
#include "GameComponents/CarPhysicsComponent.h"
#include <glm/glm.hpp>
#include "Core/Mesh.h"

void WheelSystem::Update(ecsType& registry, float deltaTime)
{
	auto e = registry.GetAllComponentsView<WheelComponent, TransformComponent>();
	for (auto entity : e)
	{
		auto& wheel = registry.GetComponent<WheelComponent>(entity);
        auto& physic = registry.GetComponent<CarPhysicsComponent>(wheel.carBody);

        float carSpeed = glm::length(physic.velocity);
        wheel.pitchAngulerVelo = carSpeed / wheel.radius;

        if (wheel.isSteerable)
        {
			float angle = 40.0f;
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

        w.rollAngle += w.angularVelocity * deltaTime;
        w.pitchAngle += w.pitchAngulerVelo * deltaTime * 4.0f;

		//Body rotation
        glm::mat4 rotY = bodyTr.GetRotationMatrix();
        rotY = glm::rotate(glm::mat4(1.0f), carRot.y, glm::vec3(0, 1, 0));

		//Wheel position
        glm::vec3 worldOffset = glm::vec3(rotY * glm::vec4(w.visualOffset, 1.0f));
        transform.SetPosition(carPos + worldOffset);

		//Wheel rotation
        glm::mat4 wheelRot = glm::rotate(glm::mat4(1.0f), carRot.y, glm::vec3(0, 1, 0));

        if (w.isSteerable)
        {
            float targetSteer = w.steerAngle;

            if (registry.HasComponent<CarPhysicsComponent>(w.carBody) && registry.HasComponent<CarControllerComponent>(w.carBody))
            {
                auto& physic = registry.GetComponent<CarPhysicsComponent>(w.carBody);
                auto& control = registry.GetComponent<CarControllerComponent>(w.carBody);

                glm::mat4 invRot = glm::inverse(bodyTr.GetRotationMatrix());
                glm::vec3 vLocal = glm::vec3(invRot * glm::vec4(physic.velocity, 0.0f));

                float lateralSlip = vLocal.x;
                float slipThreshold = 2.0f;

                bool isDrifting = glm::abs(lateralSlip) > slipThreshold;
                bool noInput = glm::abs(control.steering) < 0.05f;

                if (isDrifting && noInput)
                {
                    float angle = 45.0f;
                    float maxCounterSteer = glm::radians(angle);

                    targetSteer = glm::sign(lateralSlip) * maxCounterSteer * glm::clamp(glm::abs(lateralSlip) / 10.0f, 0.0f, 1.0f);
                }
            }
            float steerSpeed = 5.0f;
            w.currentVisualSteer = glm::mix(w.currentVisualSteer, targetSteer, 1.0f - glm::exp(-steerSpeed * deltaTime));

            wheelRot = glm::rotate(wheelRot, w.currentVisualSteer, glm::vec3(0, 1, 0));
        }
        glm::mat4 spin = glm::rotate(glm::mat4(1.0f), w.pitchAngle, glm::vec3(1, 0, 0));

        wheelRot = wheelRot * spin;

        transform.SetRotation(glm::eulerAngles(glm::quat(wheelRot)));
    }
}
