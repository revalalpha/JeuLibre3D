#include "GamePhysics/TireModel.h"

/**
 * @brief Computes the tire state based on the normal velocity, slip velocity, steering angle, and axle angle.
 * 
 * \param Vnormal
 * \param Vvar
 * \param steer
 * \param axle
 * \return tireState containing slip ratio, slip angle, adherence, and generated forces
 */
TireState TireModel::Compute(float Vnormal, float Vvar, float steer, float axle)
{
	// Simple tire model based on slip angle and slip ratio
	TireState tireState{};

	// Slip ratio is the relative difference between the slip velocity and the normal velocity
	tireState.slipRatio = (Vvar - Vnormal) / std::max(std::abs(Vnormal), 0.1f);
	// Adherence decreases with slip ratio, clamped to [0, 1]
	tireState.adherence = glm::clamp(std::abs(tireState.slipRatio), 0.0f, 1.0f);
	// Slip angle is influenced by steering input and axle position (front vs rear)
	tireState.slipAngle = steer * (0.5f + 0.05f * axle);

	// Longitudinal force
	float Fy = tireState.slipAngle * tireState.adherence * 10000.0f;
	// Lateral force
	float Fx = tireState.slipRatio * 6000.0f;

	// Combine forces into a single vector (x = longitudinal, y = lateral, z = vertical)
	tireState.force = glm::vec3(Fy, 0, Fx);

	return tireState;
}