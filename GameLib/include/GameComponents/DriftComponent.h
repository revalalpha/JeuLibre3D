#pragma once 

/**
 * @brief Component representing the car's drift state.
 */
struct DriftComponent
{
	float driftFactor = 0.0f;     ///< 0 = total grip, 1 = total drift
	float driftDecay = 2.0f;      ///< speed at which the drift factor decays back to 0 when not drifting
	float driftBoost = 1.5f;      ///< speed bonus multiplier when drifting
};
