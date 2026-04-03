#include "GameSystems/DriftSystem.h"
#include "GameComponents/DriftComponent.h"
#include "GameComponents/CarPhysicsComponent.h"
#include "GameComponents/WheelComponent.h"
#include "Core/TrasformComponent.h"
#include "glm/glm.hpp"

void DriftSystem::Update(ecsType& registry, float dt)
{
    auto view = registry.GetAllComponentsView<DriftComponent, CarPhysicsComponent, TransformComponent>();

    for (auto e : view)
    {
        auto& drift = registry.GetComponent<DriftComponent>(e);
        auto& physic = registry.GetComponent<CarPhysicsComponent>(e);
        auto& transform = registry.GetComponent<TransformComponent>(e);

		//Convert the velocity to local space
        glm::mat4 invRot = glm::inverse(transform.GetRotationMatrix());
        glm::vec3 vLocal = glm::vec3(invRot * glm::vec4(physic.velocity, 0.0f));

		//Calculate the speed in the local XZ plane
        float speed = glm::length(glm::vec2(vLocal.x, vLocal.z));

		//Stteer angle contribution
        float angle = 30.0f;
        float maxSteerAngle = glm::radians(angle);
        float steerSigned = 0.0f;

        auto wheels = registry.GetAllComponentsView<WheelComponent>();
        for (auto w : wheels)
        {
            auto& wheel = registry.GetComponent<WheelComponent>(w);
            if (wheel.isSteerable)
            {
                steerSigned = wheel.steerAngle / maxSteerAngle;
                break;
            }
        }

		//Angle difference contribution
        float targetAngle = std::atan2(physic.velocity.x, physic.velocity.z);
        float currentAngle = transform.GetRotation().y;

		//Calculate the signed angle difference in the range
        float delta = std::atan2(std::sin(targetAngle - currentAngle),
            std::cos(targetAngle - currentAngle));

        float angleSigned = glm::clamp(delta / 1.2f, -1.0f, 1.0f);

        float slipSigned = glm::clamp(vLocal.x / 8.0f, -1.0f, 1.0f);

        float driftRaw = steerSigned * 0.4f + angleSigned * 0.4f + slipSigned * 0.6f;

        drift.driftFactor = glm::clamp(driftRaw, -1.0f, 1.0f);

		//Apply drift boost to the car's velocity
        if (drift.driftFactor > 0)
            drift.driftFactor = glm::max(0.0f, drift.driftFactor - drift.driftDecay * dt);
        else
            drift.driftFactor = glm::min(0.0f, drift.driftFactor + drift.driftDecay * dt);
    }
}