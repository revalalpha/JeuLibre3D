#pragma once

/**
 * @brief Component representing the player's control inputs.
 */
struct ControllerComponent
{
    bool IsAccelerate = false;
    bool IsBrake = false;
    bool IsSteerLeft = false;
    bool IsSteerRight = false;
    bool IsHandbrake = false;

	//Can add input for PS5, XBOX, controller etc if needed
};