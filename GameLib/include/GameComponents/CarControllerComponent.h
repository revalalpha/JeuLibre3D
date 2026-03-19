#pragma once 

/**
 * @brief Component representing the car's control inputs.
 */
struct CarControllerComponent
{
	float acceleration = 0.0f;   ///< Acceleration input (0 = no throttle, 1 = full throttle)
	float steering = 0.0f;       ///< Steering input (-1 = full left, 0 = straight, 1 = full right)
	float maxSpeed = 10.0f;      ///< The maximum speed the car can reach
	float speed = 0.0f;          ///< The current speed of the car, updated by the physics system
	float turnSpeed = 1.5f;      ///< Turning speed multiplier for steering input (higher = more responsive steering)
};
