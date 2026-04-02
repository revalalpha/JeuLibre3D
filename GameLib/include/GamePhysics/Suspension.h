#pragma once

/**
 * @brief Struct representing the state of a suspension system at a given moment.
 */
struct SuspensionState
{
	float compression; ///< Suspension compression ratio (0.0 = fully extended, 1.0 = fully compressed).
	float force;       ///< Force exerted by the suspension in Newtons.
}; 

/**
 * @brief Class responsible for computing the suspension state based on axle position and dynamics.
 */
class Suspension
{
public:
	/**
	 * @brief Computes the suspension state given the axle position, angle, and angular velocity.
	 * 
	 * \param axle
	 * \param alpha
	 * \param dAlpha
	 * \return 
	 */
    static SuspensionState Compute(float axle, float alpha, float dAlpha);
};