#include "GamePhysics/CarForces.h"

/**
 * @brief Computes the forces acting on the car based on throttle, brake, mass, and velocity.
 *
 * \param throttle
 * \param brake
 * \param mass
 * \param velocity
 * \return
 */
glm::vec3 CarForces::Compute(float throttle, float brake, float mass, const glm::vec3& velocity)
{
	glm::vec3 force(0);

	force.z += throttle * 8000.0f;                          ///< Traction
	force.z -= brake * 8000.0f * glm::sign(velocity.z);     ///< Braking
	force -= 0.42f * velocity * glm::length(velocity);      ///< Drag
	force.y -= 9.81f * mass;                                ///< Gravity

	return force;
}