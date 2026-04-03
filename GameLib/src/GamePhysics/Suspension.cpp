#include "GamePhysics/Suspension.h"

/**
 * @brief Computes the suspension state given the axle position, angle, and angular velocity.
 * 
 * \param axle
 * \param alpha
 * \param dAlpha
 * \return 
 */
SuspensionState Suspension::Compute(float axle, float alpha, float dAlpha)
{
    SuspensionState suspension{};
	//Compression is a combination of the axle position, angle, and angular velocity.
    suspension.compression = 0.5f * axle + 0.05f * alpha + 0.05f * dAlpha;
	suspension.force = 30000.0f * suspension.compression;  ///< Simple linear spring force model (N)
    return suspension;
}