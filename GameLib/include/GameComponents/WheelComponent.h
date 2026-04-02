#pragma once
#include "glm/vec3.hpp"
#include "ECS/Entities.h"

/**
 * @brief Component representing a wheel in the car physics system.
 */
struct WheelComponent
{
	float angularVelocity = 0.0f;     ///< radians per second
	float radius = 0.35f;             ///< meters
	float speed = 0.0f;               ///< Current linear speed of the wheel in m/s (calculated from angular velocity and radius)


	float motorTorque = 0.0f;         ///< Motor torque applied to the wheel in N·m
	float brakeTorque = 0.0f;         ///< Brake torque applied to the wheel in N·m
	float rollingResistance = 0.015f; ///< Coefficient of rolling resistance

	bool isDriven = false;            ///< True if this wheel is powered by the engine
	bool isSteerable = false;         ///< Only the front wheel can steer

	float steerAngle = 0.0f;          ///< Current steering angle in radians (for steerable wheels)
	float steerInput = 0.0f;          ///< Steering input from the controller (-1 to 1, where -1 is full left and 1 is full right)

	float slipRatio = 0.0f;           ///< Slip ratio for longitudinal slip (acceleration/braking)
	float carSpeed = 0.0f;            ///< Speed of the car at the wheel's position in m/s (used for slip calculations)
	KGR::ECS::Entity::_64 carBody;    ///< Entity ID of the car body this wheel is attached to

	glm::vec3 visualOffset{ 0 };
};