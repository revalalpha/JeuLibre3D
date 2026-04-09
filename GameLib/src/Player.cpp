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
#include "GameComponents/CarPhysicsComponent.h"
#include "GameComponents/WheelComponent.h"

void Player::CreatePlayer(ecsType& registry, KGR::RenderWindow& window)
{
	auto player = registry.CreateEntity();

	//Mesh
	MeshComponent mesh;
	mesh.mesh = &MeshLoader::Load("Models/Car/Body/celica_body.obj", window.App());

	//Transform
	TransformComponent carTransform;
	carTransform.SetPosition({ 0, 0.2f, 0 });
	carTransform.SetRotation({ 0, glm::radians(180.0f), 0 });
	
	////Texture
	//TextureComponent texture;
	//texture.SetSize(meshComp.mesh->GetSubMeshesCount());
	//for (int i = 0; i < meshComp.mesh->GetSubMeshesCount(); ++i)
	//{
	//	texture.AddTexture(i, &TextureLoader::Load("Textures/Car/Car_Base_Color.png", window.App()));
	//	//texture.AddTexture(i, &TextureLoader::Load("Textures/Car/Car_Metallic.png", window.App()));
	//	//texture.AddTexture(i, &TextureLoader::Load("Textures/Car/Car_Roughness.png", window.App()));
	//}
	
	// create a texture 
	MaterialComponent text;
	// allocate the size of the texture must be the same as the number of submeshes 
	text.materials.resize(mesh.mesh->GetSubMeshesCount());
	// then fill the texture ( this system need to be refact but for now you need to do it like that
	for (int i = 0; i < mesh.mesh->GetSubMeshesCount(); ++i)
	{
		Material mat;
		mat.baseColor = &TextureLoader::Load("Textures/Car/Car_Base_Color.png", window.App());
		//mat.emissive = &TextureLoader::Load("Textures/bloc_BaseColor_Emissive.png", window.App());
		//mat.normalMap = &TextureLoader::Load("Textures/bloc_Normal.png", window.App());
		//mat.pbrMap = &TextureLoader::Load("Textures/bloc_ORM.png", window.App());
		text.materials[i] = mat;
	}
	

	CollisionComp collider;
	collider.collider = new Collider();

	//TODO TOFIX collider is very dirty, might take the vertices model to make the collider
	collider.collider->localBox.m_min = glm::vec3(-0.905f, -0.505f, -2.215f);
	collider.collider->localBox.m_max = glm::vec3(0.905f, 0.505f, 2.215f);


	CarPhysicsComponent carPhysic;
	carPhysic.wheels.reserve(4);

	registry.AddComponents<MeshComponent, TransformComponent, MaterialComponent, ControllerComponent,
		CarControllerComponent, DriftComponent, CarPhysicsComponent, PlayerComponent, CollisionComp>
		(player, std::move(mesh), std::move(carTransform), std::move(text), ControllerComponent{},
			CarControllerComponent{}, DriftComponent{}, std::move(carPhysic), PlayerComponent{}, std::move(collider));

	auto createWheel = [&](const std::string& path, glm::vec3 offset, bool driven, bool steerable)
		{
			auto w = registry.CreateEntity();

			MeshComponent mesh;
			mesh.mesh = &MeshLoader::Load(path, window.App());

			TransformComponent transform;
			transform.SetPosition(offset);

			//TextureComponent texture;
			//texture.SetSize(mesh.mesh->GetSubMeshesCount());
			//for (int i = 0; i < mesh.mesh->GetSubMeshesCount(); ++i)
			//	texture.AddTexture(i, &TextureLoader::Load("Textures/Car/texture voiture.png", window.App()));

			// create a texture 
			MaterialComponent text;
			// allocate the size of the texture must be the same as the number of submeshes 
			text.materials.resize(mesh.mesh->GetSubMeshesCount());
			// then fill the texture ( this system need to be refact but for now you need to do it like that
			for (int i = 0; i < mesh.mesh->GetSubMeshesCount(); ++i)
			{
				Material mat;
				mat.baseColor = &TextureLoader::Load("Textures/Car/texture voiture.png", window.App());
				//mat.emissive = &TextureLoader::Load("Textures/bloc_BaseColor_Emissive.png", window.App());
				//mat.normalMap = &TextureLoader::Load("Textures/bloc_Normal.png", window.App());
				//mat.pbrMap = &TextureLoader::Load("Textures/bloc_ORM.png", window.App());
				text.materials[i] = mat;
			}

			WheelComponent wheel;
			wheel.isDriven = driven;
			wheel.isSteerable = steerable;
			wheel.visualOffset = offset;
			wheel.carBody = player;

			registry.AddComponents<
				MeshComponent, TransformComponent, MaterialComponent, WheelComponent>
				(w, std::move(mesh), std::move(transform), std::move(text), std::move(wheel));

			return w;
		};

	//Create the four wheels with appropriate offsets and properties
	auto frontLeft = createWheel("Models/Car/Left_FrontWheel.obj", { 0.4f, 0.0f, .65f }, false, true);
	auto frontRight = createWheel("Models/Car/Right_FrontWheel.obj", { -0.4f, 0.0f, 0.65f }, false, true);
	auto backWheels = createWheel("Models/Car/BackWheels.obj", { -0.029f, 0.0f, -.59f }, true, false);

	//Link wheels to the car physics component
	auto& phys = registry.GetComponent<CarPhysicsComponent>(player);
	phys.wheels.push_back(frontLeft);
	phys.wheels.push_back(frontRight);
	phys.wheels.push_back(backWheels);

	auto cam = registry.CreateEntity();

	//Camera
	CameraComponent camComp = CameraComponent::Create(45.0f, static_cast<float>(window.GetSize().x), static_cast<float>(window.GetSize().y), 0.01f, 1000000.0f, CameraComponent::Type::Perspective);

	//Transform
	TransformComponent camTransform;
	camTransform.SetPosition({ 0, 3, 5 });

	//Car Camera component
	CarCameraComponent follow;
	follow.target = player;
	follow.distance = 2.5f;
	follow.baseDistance = 2.5f;
	follow.height = 1.2f;
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