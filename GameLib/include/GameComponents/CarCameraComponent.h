#pragma once
#include "ECS/Entities.h"

/**
 * @brief Component representing a camera that follows a target entity (e.g., the player's car).
 */
struct CarCameraComponent
{
	KGR::ECS::Entity::_64 target;   ///< Target entity to follow the car
	float distance = 6.0f;          ///< distance behind the car
	float baseDistance = 6.0f;
	float minDistance = 3.0f;
	float height = 2.0f;            ///< height above the car
	float smooth = 8.0f;            ///< position smoothing factor
	float lookSmooth = 12.0f;       ///< smoothing factor for camera rotation when looking at the target
	float driftInfluence = 1.5f;    ///< drift influence on camera angle (0 = no influence, 1 = full influence)

	float fov = 45.0f;
	float baseFov = 45.0f;
	float pivotOffset = 2.0f;
	float speedDistanceInfluence = 0.05f;

	float speedInfluence = 0.02f;   ///< dynamic FOV influence based on car speed (0 = no influence, higher = more FOV increase at high speeds)
	float steerInfluence = 0.5f;    ///< influence of steering angle on camera position (0 = no influence, 1 = full influence)
	float smoothedSteer = 0.0f;     ///< smoothed steering angle for camera adjustments
	float steerSmooth = 15.0f;      ///< smoothing factor for steering influence on camera

	glm::vec3 smoothedLookDir = glm::vec3(0, 0, 1);
};