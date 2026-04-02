#pragma once
#include "ECS/Entities.h"
#include <glm/glm.hpp>

/**
 * @brief Component storing the physical state of the car.
 */
struct CarPhysicsComponent
{
    float mass = 1200.0f;          ///< kg
    float drag = 0.4257f;          ///< aerodynamic drag coefficient
    float rollingFriction = 12.0f; ///< rolling resistance

    glm::vec3 velocity = glm::vec3(0.0f); ///< world velocity

    // Inputs
    float throttle = 0.0f; ///< -1..1
    float brake = 0.0f;    ///<  0..1
    float steer = 0.0f;    ///< -1..1

    // Wheels linked to this car
    std::vector<KGR::ECS::Entity::_64> wheels;
};