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
        float reverseForce = 12000.0f;

        if (!control.handBraking)
        {
            if (physic.throttle > 0.0f)
                totalForce += forward * (physic.throttle * tractionForce);
            else if (physic.throttle < 0.0f)
                totalForce += forward * (physic.throttle * reverseForce);
        }

		//Engine braking
        if (physic.throttle <  -0.01f)
        {   
            float speed = glm::length(physic.velocity);
            float minBrake = 2.0f;
            float maxBrake = 18.0f;

            float speedFactor = glm::clamp(speed / 40.0f, 0.0f, 0.1f);

            float engineBrake = glm::mix(minBrake, maxBrake, speedFactor);

            float forwardSpeed = vLocal.z;

            if(glm::abs(forwardSpeed)> 0.5f)
                totalForce -= forward * (engineBrake * glm::sign(forwardSpeed));
        }

        if (physic.throttle >= -0.01f && physic.throttle <= 0.01f)
        {
            float coastBrake = 1.5f;
            if(glm::abs(vLocal.z) > 0.5f)
                totalForce -= forward * (coastBrake * glm::sign(vLocal.z));
        }

        if (control.handBraking)
        {
            float handBrakeForce = 8000.0f;
            totalForce.z -= glm::sign(vLocal.z) * handBrakeForce;
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
                physic.slipAccumulator += dt * 3.0f;
        }
        else if (!control.handBraking)
        {
            physic.slipAccumulator = glm::max(0.0f, physic.slipAccumulator - dt * 4.0f);
        }
        else
        {
            physic.slipAccumulator = 0.0f;
        }

        if (control.handBraking)
        {
            physic.slipAccumulator += dt * 5.0f;
        }

        //Friction
        bool isReversing = vLocal.z < -0.5f && physic.throttle < 0.0f;

        float slipExponent = 1.0f - glm::exp(-physic.slipAccumulator * 8.f);
        float frictionX = glm::mix(12.0f, 0.5f, slipExponent);
        if (control.handBraking)
            frictionX *= 0.15f;
        if (isReversing)
            frictionX *= 0.4f;

        float frictionZ = 18.0f;

        totalForce.x -= vLocal.x * frictionX;
        totalForce.z -= vLocal.z * frictionZ;

		//Gravity and normal force
        glm::vec3 accel = totalForce / physic.mass;

		//Limit acceleration to prevent instability
        float maxAccel = 50.0f;
        float maxSpeed = 200.0f;
        if (glm::length(accel) > maxAccel)
            accel = glm::normalize(accel) * maxAccel;

        float speed = glm::length(physic.velocity);
        float speedRatio = glm::clamp(speed / maxSpeed, 0.0f, 1.0f);
        float accelFactor = glm::pow(1.0f - (speed / maxSpeed), 0.4f);

        if (physic.throttle >= 0.0f)
        {
            float kickIn = glm::smoothstep(0.0f, 0.5f, speedRatio);
            float highEnd = glm::pow(1.0f - speedRatio, 1.2f);

            accelFactor = glm::mix(1.2f, highEnd, kickIn);
        }
        else
        {
            accelFactor = glm::pow(1.0f - glm::clamp(-speed / (maxSpeed * 0.5f), 0.0f, 1.0f), 0.4f);
        }  
        accelFactor = glm::clamp(accelFactor, 0.0f, 1.0f);
        accel *= accelFactor;
        physic.velocity += accel * dt;

        //Damping
        float damping = glm::mix(0.995f, 0.97f, speedRatio * speedRatio);
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
        float minSteer = 0.0028f;
        float maxSteer = 0.06f;
        float steerFactor = glm::clamp(1.0f - (speed / 60.0f), 0.0f, 1.0f);
        float steerAngleMax = glm::mix(minSteer, maxSteer, steerFactor);

        float smoothStep = glm::abs(control.steering) > 0.0001f ? 2.25f : 0.75f;
        physic.smoothSteering = glm::mix(physic.smoothSteering, control.steering, dt * smoothStep);

        float currentYaw = transform.GetRotation().y;

        float targetAngle = std::atan2(physic.velocity.x, physic.velocity.z);
        float delta = std::atan2(std::sin(targetAngle - currentYaw), std::cos(targetAngle - currentYaw));

        float alignFactor = 1.0f;

        if(!isReversing)
            alignFactor  = 1.0f - glm::clamp(glm::abs(delta) * 0.03f, 0.0f, 1.0f);

        float speedSteerFactor = glm::smoothstep(0.0f, 5.0f, speed);

        float reverseSign = (isReversing) ? -1.0f : 1.0f;
        float yaw = physic.smoothSteering * steerAngleMax * 0.62f * alignFactor * speedSteerFactor * reverseSign;
        
        currentYaw += yaw;

        if (speed > 2.0f)
        {
            float lateralSlip = vLocal.x;
            float angle = 9.0f;

            float oversteerThreshold = glm::radians(angle);
            float oversteerAmount = glm::min(
                glm::max(0.0f, glm::abs(delta) - oversteerThreshold),
                0.4f
            );

            float overSteerStrength = 1.8f;
            if (control.handBraking)
                overSteerStrength = 2.5f;

            currentYaw -= glm::sign(delta) * oversteerAmount * overSteerStrength * dt;
        }

        transform.SetRotation(glm::vec3(0, currentYaw, 0));

        control.speed = glm::length(physic.velocity);
    }
}