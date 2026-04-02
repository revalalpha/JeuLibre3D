#include "GameSystems/CameraSystem.h"
#include "GameComponents/CarCameraComponent.h"
#include "Core/CameraComponent.h"
#include "GameComponents/DriftComponent.h"
#include "GameComponents/CarControllerComponent.h"
#include "Core/TrasformComponent.h"
#include "GameComponents/WheelComponent.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>


void CameraSystem::Update(ecsType& registry, float deltaTime)
{
	auto e = registry.GetAllComponentsView<CarCameraComponent, CameraComponent, TransformComponent>();

	for (auto camEntity : e)
	{
		auto& follow = registry.GetComponent<CarCameraComponent>(camEntity);
		auto& camTransform = registry.GetComponent<TransformComponent>(camEntity);

		//Skip if the target entity doesn't exist or doesn't have a TransformComponent
		if (!registry.HasComponent<TransformComponent>(follow.target))
			continue;

		auto& carTransform = registry.GetComponent<TransformComponent>(follow.target);

		//Car position and orientation
		glm::vec3 carPos = carTransform.GetPosition();
		glm::vec3 forward = -carTransform.GetLocalAxe<RotData::Dir::Forward>();
		glm::vec3 right = carTransform.GetLocalAxe<RotData::Dir::Right>();

		glm::vec3 targetPos = carPos - forward * follow.distance + glm::vec3(0, follow.height, 0);

		//Steering influence
        float steerAngle = 0.0f;
        auto wheels = registry.GetAllComponentsView<WheelComponent>();
        for (auto w : wheels)
        {
            auto& wheel = registry.GetComponent<WheelComponent>(w);
            if (wheel.isSteerable)
                steerAngle = wheel.steerAngle;
        }

		follow.smoothedSteer = glm::mix(follow.smoothedSteer, steerAngle, deltaTime * follow.steerSmooth);

		float angle = 30.0f;
        float maxSteer = glm::radians(angle);
		float steerFactor = follow.smoothedSteer / maxSteer;

        targetPos += right * steerFactor * follow.steerInfluence;

        //Drift influence
        if (registry.HasComponent<DriftComponent>(follow.target))
        {
            auto& drift = registry.GetComponent<DriftComponent>(follow.target);
            targetPos += right * drift.driftFactor * follow.driftInfluence;
        }

		//Smoothly interpolate camera position
		glm::vec3 currentPos = camTransform.GetPosition();
		glm::vec3 newPos = glm::mix(currentPos, targetPos, deltaTime * follow.smooth);
		camTransform.SetPosition(newPos);

		//Calculate look direction and target rotation
		glm::vec3 rawLookDir = glm::normalize(carPos - newPos);
		rawLookDir.x *= 0.98f;
		rawLookDir.y *= 0.78f;
		follow.smoothedLookDir = glm::mix(follow.smoothedLookDir, rawLookDir, deltaTime * follow.lookSmooth);
		follow.smoothedLookDir = glm::normalize(follow.smoothedLookDir);

		glm::quat targetRot = glm::quatLookAt(follow.smoothedLookDir, { 0,1,0 });
		glm::quat currentRot = camTransform.GetOrientation();
		glm::quat newRot = glm::slerp(currentRot, targetRot, deltaTime * follow.lookSmooth * 0.5f);

		camTransform.SetOrientation(newRot);
	}
}