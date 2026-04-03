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
		auto& cam = registry.GetComponent<CameraComponent>(camEntity);

		//Skip if the target entity doesn't exist or doesn't have a TransformComponent
		if (!registry.HasComponent<TransformComponent>(follow.target))
			continue;

		auto& carTransform = registry.GetComponent<TransformComponent>(follow.target);

		//Car position and orientation
		glm::vec3 forward = -carTransform.GetLocalAxe<RotData::Dir::Forward>();
		glm::vec3 carPos = carTransform.GetPosition();
		glm::vec3 right = carTransform.GetLocalAxe<RotData::Dir::Right>();

		float targetFov = follow.fov;
		float targetDistance = follow.baseDistance;

		if (registry.HasComponent<CarControllerComponent>(follow.target))
		{
			auto& car = registry.GetComponent<CarControllerComponent>(follow.target);

			float speed = glm::abs(car.speed);

			//Distance
			targetDistance = follow.baseDistance - speed * follow.speedDistanceInfluence;
			targetDistance = glm::clamp(targetDistance, follow.minDistance, follow.baseDistance);

			//follow.distance = glm::mix(follow.distance, targetDistance, deltaTime * follow.smooth);

			//Fov
			targetFov = follow.baseFov + speed * follow.speedInfluence;
			targetFov = glm::clamp(targetFov, follow.baseFov, follow.baseFov * 30.0f);
		}

		glm::vec3 targetPos = carPos - forward * targetDistance + glm::vec3(0, follow.height, 0);

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

		//Smoothly change fov
		float currentFov = follow.fov;
		follow.fov = glm::mix(currentFov, targetFov, deltaTime * follow.smooth);
		cam.SetFov(follow.fov);

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