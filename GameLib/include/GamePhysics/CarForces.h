#pragma once
#include <glm/glm.hpp>

class CarForces
{
public:
    static glm::vec3 Compute(float throttle, float brake, float mass, const glm::vec3& velocity);
};