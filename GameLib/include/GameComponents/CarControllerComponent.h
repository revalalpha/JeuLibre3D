#pragma once 

/**
 * @brief Component representing the car's control inputs.
 */
struct CarControllerComponent
{
	float acceleration = 0.0f;   ///< Acceleration input (0 = no throttle, 1 = full throttle)
	float steering = 0.0f;       ///< Steering input (-1 = full left, 0 = straight, 1 = full right)

	float maxSpeed = 20.0f;      ///< The maximum speed the car can reach
	float speed = 0.0f;          ///< The current speed of the car, updated by the physics system
	float turnSpeed = 1.5f;      ///< Turning speed multiplier for steering input (higher = more responsive steering)

	float engine_Braking = 0.01f;
	float coefAccel_1 = 1.0f;
	float coefAccel_2 = 1.0f;
	float backWheel_Speed = 0.0f;
	float reverse_Speed = -5.0f;

	float currentVisualSteer = 0.0f;

	float maxSpeed = 0.0f;       ///< The maximum speed the car can reach
	float speed = 0.0f;          ///< The current speed of the car, updated by the physics system
	float turnSpeed = 2.0f;      ///< Turning speed multiplier for steering input (higher = more responsive steering)
};