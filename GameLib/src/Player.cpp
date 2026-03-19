#include "Player.h"
#include "Core/CameraComponent.h"
#include "Core/Mesh.h"
#include "Core/TrasformComponent.h"
#include "Core/Texture.h"
#include "Math/CollisionComponent.h"
#include "Core/ManagerImple.h"
#include "Core/Window.h"
#include "ECS/Registry.h"
#include "ECS/Entities.h"
#include "Core/InputManager.h"
#include "GameComponents/ControllerComponent.h"
#include "GameComponents/CarControllerComponent.h"
#include "GameComponents/DriftComponent.h"
#include "GameComponents/CarCameraComponent.h"

void Player::CreatePlayer(ecsType& registry, KGR::RenderWindow& window)
{
	auto player = registry.CreateEntity();

	//Mesh
	MeshComponent meshComp;
	meshComp.mesh = &MeshLoader::Load("Models\\280z.obj", window.App());

	//Transform
	TransformComponent carTransform;
	carTransform.SetPosition({ 0, 0, 0 });
	carTransform.SetRotation({ 0, glm::radians(180.0f), 0 });

	//Texture
	TextureComponent texture;
	texture.SetSize(meshComp.mesh->GetSubMeshesCount());
	for (int i = 0; i < meshComp.mesh->GetSubMeshesCount(); ++i)
		texture.AddTexture(i, &TextureLoader::Load("Textures\\rouge.jpg", window.App()));

	CollisionComp collider;
	collider.collider = &ColliderManager::Load("playerCollider", meshComp.mesh);

	registry.AddComponents<MeshComponent, TransformComponent, TextureComponent, ControllerComponent,
		CarControllerComponent, DriftComponent, PlayerComponent, CollisionComp>
		(player, std::move(meshComp), std::move(carTransform), std::move(texture), ControllerComponent{},
			CarControllerComponent{}, DriftComponent{}, PlayerComponent{}, std::move(collider));

	auto cam = registry.CreateEntity();

	//Camera
	CameraComponent camComp = CameraComponent::Create(45.0f, static_cast<float>(window.GetSize().x), static_cast<float>(window.GetSize().y), 0.01f, 1000000.0f, CameraComponent::Type::Perspective);

	//Transform
	TransformComponent camTransform;
	camTransform.SetPosition({ 0, 3, 5 });

	//Car Camera component
	CarCameraComponent follow;
	follow.target = player;
	follow.distance = 6.0f;
	follow.height = 2.0f;
	follow.smooth = 8.0f;
	follow.lookSmooth = 12.0f;

	registry.AddComponents<CameraComponent, TransformComponent, CarCameraComponent>(cam, std::move(camComp), std::move(camTransform), std::move(follow));
}

void Player::Update(ecsType& registry, float deltaTime)
{
	glm::vec3 playerPos{ 0.0f };
	{
		auto view = registry.GetAllComponentsView<PlayerComponent, TransformComponent>();
		for (auto e : view)
		{
			playerPos = registry.GetComponent<TransformComponent>(e).GetPosition();
			break;
		}
	}
}