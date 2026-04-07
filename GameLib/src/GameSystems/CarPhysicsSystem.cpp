#include "GameSystems/CarPhysicsSystem.h"
#include "GameComponents/CarPhysicsComponent.h"
#include "GameComponents/CarControllerComponent.h"
#include "GameComponents/WheelComponent.h"
#include "Core/TrasformComponent.h"
#include "glm/glm.hpp"

#include "GamePhysics/CarForces.h"
#include "GamePhysics/TireModel.h"
#include "GamePhysics/Suspension.h"

void CarPhysicsSystem::Update(ecsType& registry, float dt)
{
    auto view = registry.GetAllComponentsView<CarPhysicsComponent, CarControllerComponent, TransformComponent>();

    for (auto e : view)
    {
        auto& physic = registry.GetComponent<CarPhysicsComponent>(e);
        auto& control = registry.GetComponent<CarControllerComponent>(e);
        auto& transform = registry.GetComponent<TransformComponent>(e);

        //Inputs
        physic.throttle = control.acceleration;
        physic.steer = control.steering;
        physic.brake = (control.acceleration < 0.0f) ? -control.acceleration : 0.0f;

        //Local velocity
        glm::mat4 invRot = glm::inverse(transform.GetRotationMatrix());
        glm::vec3 vLocal = glm::vec3(invRot * glm::vec4(physic.velocity, 0.0f));

        glm::vec3 totalForce(0.0f);

        glm::vec3 forward = -transform.GetLocalAxe<RotData::Dir::Forward>();

        float tractionForce = 17000.0f;
        totalForce += forward * (physic.throttle * tractionForce);

		//Engine braking
        if (physic.throttle == 0.0f)
        {
            float engineBrake = 250.0f;
            totalForce -= forward * (engineBrake * glm::sign(vLocal.z));
        }

        float steerAngle = 0.0f;
        float steerInput = 0.0f;

        auto wheels = registry.GetAllComponentsView<WheelComponent>();
        for (auto& w : wheels)
        {
            auto& wheel = registry.GetComponent<WheelComponent>(w);
            if (wheel.isSteerable)
            {
                steerAngle = glm::abs(wheel.steerAngle);
                steerInput = wheel.steerAngle;
                break;
            }
        }

        if (glm::abs(steerInput) > 0.01f)
        {
            if (physic.slipAccumulator != 0.0f && glm::sign(steerInput) != glm::sign(physic.slipAccumulator))
                physic.slipAccumulator = 0.0f;
            else
                physic.slipAccumulator += dt;
        }
        else
        {
            physic.slipAccumulator = 0.0f;
        }

        //Friction
        float slipExponent = 1.0f - glm::exp(-physic.slipAccumulator * 8.f);
        float frictionX = glm::mix(12.0f, 2.0f, slipExponent);

        float frictionZ = 20.0f;

        totalForce.x -= vLocal.x * frictionX;
        totalForce.z -= vLocal.z * frictionZ;

		//Gravity and normal force
        glm::vec3 accel = totalForce / physic.mass;

		//Limit acceleration to prevent instability
        float maxAccel = 20.0f;
        float maxSpeed = 100.0f;
        if (glm::length(accel) > maxAccel)
            accel = glm::normalize(accel) * maxAccel;

        float speed = glm::length(physic.velocity);
        float accelFactor = 1.0f - (speed / maxSpeed);
        accelFactor = glm::clamp(accelFactor, 0.0f, 1.0f);

        accel *= accelFactor;

        physic.velocity += accel * dt;

        //Damping
        float damping = 0.999f;
        physic.velocity -= physic.velocity * damping * dt;

		//Limit speed
        if (speed > maxSpeed)
            physic.velocity = glm::normalize(physic.velocity) * maxSpeed;

		//Stop the car if it's very slow and no throttle is applied
        if (speed < 1.f && physic.throttle == 0.0f)
            physic.velocity = glm::vec3(0.0f);
        speed = glm::length(physic.velocity);

        physic.velocity.y = 0.0f;

        transform.Translate(physic.velocity * dt);

        //Rotate
        

        
        //Update
        float steerSpeed = glm::clamp(speed, 10.0f, 140.0f);
        float steerFactor = 1.0f - (steerSpeed / 140.0f);

        float smoothStep = glm::abs(control.steering) > 0.0001f ? 2.25f : 16.f;
        physic.smoothSteering = glm::mix(physic.smoothSteering, control.steering, dt * smoothStep);

        float currentYaw = transform.GetRotation().y;

        float targetAngle = std::atan2(physic.velocity.x, physic.velocity.z);
        float delta = std::atan2(std::sin(targetAngle - currentYaw), std::cos(targetAngle - currentYaw));

        float alignFactor  = 1.0f - glm::clamp(glm::abs(delta) * 0.57f, 0.0f, 1.0f);

        float yaw = physic.smoothSteering * steerFactor * 0.02f * alignFactor;
        
        currentYaw += yaw;

        if (speed > 2.0f)
        {
            float lateralSlip = vLocal.x;

            float oversteerThreshold = glm::radians(20.0f);
            float oversteerAmount = glm::min(
                glm::max(0.0f, glm::abs(delta) - oversteerThreshold),
                0.3f
            );

            float overSteerStrength = 1.8f;

            currentYaw -= glm::sign(delta) * oversteerAmount * overSteerStrength * dt;
        }

        transform.SetRotation(glm::vec3(0, currentYaw, 0));

        control.speed = glm::length(physic.velocity);
    }
}