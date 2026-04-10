#include "GameSystems/CarCollisionSystem.h"
#include "GameComponents/CarPhysicsComponent.h"
#include "Core/TrasformComponent.h"
#include "Math/CollisionComponent.h"
#include "Math/SAT.h"
#include "Math/Collision.h"
#include "DebugRenderer.h"
#include "GameRenderer.h"
#include "Core/Window.h"

#include <iostream>

void CarCollisionSystem::Update(ecsType& registry, KGR::RenderWindow& window, float dt, bool debugCollision)
{
    auto cars  = registry.GetAllComponentsView<CarPhysicsComponent, TransformComponent, CollisionComp>();
    auto colliders = registry.GetAllComponentsView<CollisionComp, TransformComponent>();

    for (auto carEntity : cars)
    {
        auto& carTr   = registry.GetComponent<TransformComponent>(carEntity);
        auto& carCol  = registry.GetComponent<CollisionComp>(carEntity);
        auto& carPhys = registry.GetComponent<CarPhysicsComponent>(carEntity);

        auto carOBB = carCol.collider->ComputeGlobalOBB(
            carTr.GetScale(),
            carTr.GetPosition(),
            carTr.GetOrientation()
        );

        for (auto otherEntity : colliders)
        {
            if (otherEntity == carEntity)
                continue;

            auto& otherTr  = registry.GetComponent<TransformComponent>(otherEntity);
            auto& otherCol = registry.GetComponent<CollisionComp>(otherEntity);

            auto otherOBB = otherCol.collider->ComputeGlobalOBB(
                otherTr.GetScale(),
                otherTr.GetPosition(),
                otherTr.GetOrientation()
            );

            auto col = KGR::SeparatingAxisTheorem::CheckCollisionOBB3D(carOBB, otherOBB);
            if (!col.IsColliding())
                continue;


            glm::vec3 normal = glm::normalize(col.GetCollisionNormal());
            if (normal.y > 0.8f || normal.y < -0.8f)
                continue;

            float penetration = col.GetPenetration();

            if (glm::dot(carPhys.velocity, normal) > 0.0f)
                normal = -normal;

            glm::vec3 newPos = carTr.GetPosition() + normal * (penetration + 0.02f);
            newPos.y = 0.31f;
            carTr.SetPosition(newPos);

            carOBB = carCol.collider->ComputeGlobalOBB(
                carTr.GetScale(),
                carTr.GetPosition(),
                carTr.GetOrientation()
            );

            float vn = glm::dot(carPhys.velocity, normal);
            if (vn < 0.0f)
            {
                carPhys.velocity -= normal * vn;
                float bounce = 0.2f;
                carPhys.velocity += normal * bounce * (-vn);

                glm::vec3 lateralVel = carPhys.velocity - glm::dot(carPhys.velocity, normal) * normal;
                if (glm::length(lateralVel) > 0.001f)
                {
                    glm::vec3 tangent = glm::normalize(lateralVel);
                    if (glm::dot(tangent, normal) < 0.0f)
                        carPhys.velocity += tangent * 1.0f;
                }
            }

            float maxSpeed = 5.0f;
            float speed = glm::length(carPhys.velocity);
            if (speed > maxSpeed)
                carPhys.velocity = glm::normalize(carPhys.velocity) * maxSpeed;
        }
    }
}
