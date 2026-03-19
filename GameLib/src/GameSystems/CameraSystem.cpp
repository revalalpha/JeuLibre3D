#include "GameSystems/CameraSystem.h"
#include "GameComponents/CarCameraComponent.h"
#include "Core/CameraComponent.h"
#include "GameComponents/DriftComponent.h"
#include "GameComponents/CarControllerComponent.h"
#include "Core/TrasformComponent.h"
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

		//Drift influence to camera position
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
		glm::vec3 lookDir = glm::normalize(carPos - newPos);
		glm::quat targetRot = glm::quatLookAt(lookDir, { 0,1,0 });

		//Smoothly interpolate camera rotation
		glm::quat currentRot = camTransform.GetOrientation();
		glm::quat newRot = glm::slerp(currentRot, targetRot, deltaTime * follow.lookSmooth);
		camTransform.SetOrientation(newRot);
	}
}