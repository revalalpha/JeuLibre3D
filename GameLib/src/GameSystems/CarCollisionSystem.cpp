#include "GameSystems/CarCollisionSystem.h"
#include "GameComponents/CarPhysicsComponent.h"
#include "Core/TrasformComponent.h"
#include "Math/CollisionComponent.h"
#include "Math/SAT.h"
#include "Math/Collision.h"
#include "DebugRenderer.h"
#include "GameRenderer.h"
#include "Core/Window.h"

void CarCollisionSystem::Update(ecsType& registry, KGR::RenderWindow& window, float dt, bool debugCollision)
{
    auto cars = registry.GetAllComponentsView<CarPhysicsComponent, TransformComponent, CollisionComp>();
    auto walls = registry.GetAllComponentsView<CollisionComp, TransformComponent>();

    for (auto carEntity : cars)
    {
        auto& carTr = registry.GetComponent<TransformComponent>(carEntity);
        auto& carCol = registry.GetComponent<CollisionComp>(carEntity);
        auto& carPhys = registry.GetComponent<CarPhysicsComponent>(carEntity);

        //OBB generation
        auto carOBB = carCol.collider->ComputeGlobalOBB(
            carTr.GetScale(),
            carTr.GetPosition(),
            carTr.GetOrientation());

		//Debug
        auto& dbg = window.App()->GetDebugRenderer();
		

        for (auto wallEntity : walls)
        {
            if (wallEntity == carEntity)
                continue;

            auto& wallTr = registry.GetComponent<TransformComponent>(wallEntity);
            auto& wallCol = registry.GetComponent<CollisionComp>(wallEntity);

			//Global OBB for the wall
            auto wallOBB = wallCol.collider->ComputeGlobalOBB(
                wallTr.GetScale(),
                wallTr.GetPosition(),
                wallTr.GetOrientation());

            if (debugCollision)
            {
                dbg.DrawOBB(carOBB, { 1,0,0 });
                dbg.DrawOBB(wallOBB, { 0,1,0 });
            }

			//OBB-OBB collision test
            KGR::Collision3D col = KGR::SeparatingAxisTheorem::CheckCollisionOBB3D(carOBB, wallOBB);

            if (col.IsColliding())
            {
				//Collision response
                glm::vec3 normal = glm::normalize(col.GetCollisionNormal());
                float penetration = col.GetPenetration();

				//Ensure the normal points from the wall to the car
                if (glm::dot(carPhys.velocity, normal) > 0.0f)
                    normal = -normal;

                float extra = 0.02f;
				//Push the car out of the wall
                carTr.SetPosition(carTr.GetPosition() + normal * (penetration + extra));

                carOBB = carCol.collider->ComputeGlobalOBB(
                    carTr.GetScale(),
                    carTr.GetPosition(),
                    carTr.GetOrientation());

				//Calculate the velocity component along the collision normal
                float vn = glm::dot(carPhys.velocity, normal);

				//If the car is moving towards the wall, apply a collision response
                if (vn < 0.0f)
                {
                    carPhys.velocity -= normal * vn;

					//Apply a bounce factor
                    float bounce = 0.5f;
                    carPhys.velocity += normal * bounce * (-vn);

					//Apply a slide boost along the tangent to encourage sliding along the wall
                    glm::vec3 tangent = glm::normalize(carPhys.velocity - glm::dot(carPhys.velocity, normal) * normal);

                    float slideBoost = 1.0f;

					//Only apply the slide boost if the tangent is in the opposite direction of the normal
                    if (glm::dot(tangent, normal) < 0.0f)
                        carPhys.velocity += tangent * slideBoost;
                }

				//Clamp the velocity to prevent excessive speeds after collision response
                float maxSpeedAfterCollision = 5.0f;
                float speed = glm::length(carPhys.velocity);
                if (speed > maxSpeedAfterCollision)
                    carPhys.velocity = glm::normalize(carPhys.velocity) * maxSpeedAfterCollision;
            }
        }
    }
}