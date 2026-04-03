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

#include "GameComponents/NameTagComponent.h"

#include "GameComponents/CarPhysicsComponent.h"
#include "GameComponents/WheelComponent.h"


void Player::CreatePlayer(ecsType& registry, KGR::RenderWindow& window)
{
	auto player = registry.CreateEntity();

	//Mesh
	MeshComponent meshComp;

	meshComp.mesh = &MeshLoader::Load("Models\\Car\\Body\\celicaBody.obj", window.App());

	//MeshComponent L_FrontWheels;
	//L_FrontWheels.mesh = &MeshLoader::Load("Models\\Car\\", window.App());

	meshComp.mesh = &MeshLoader::Load("Models\\Car\\celicaBody.obj", window.App());


	//Transform
	TransformComponent carTransform;
	carTransform.SetPosition({ 0.f, 0.3f, 0.f });
	carTransform.SetRotation({ 0, glm::radians(180.0f), 0 });

	//Texture
	TextureComponent texture;
	texture.SetSize(meshComp.mesh->GetSubMeshesCount());
	for (int i = 0; i < meshComp.mesh->GetSubMeshesCount(); ++i)
		texture.AddTexture(i, &TextureLoader::Load("Textures\\rouge.jpg", window.App()));

	CollisionComp collider;
	collider.collider = new Collider();

	collider.collider->localBox.m_min = glm::vec3(-0.905f, -0.505f, -2.215f);
	collider.collider->localBox.m_max = glm::vec3(0.905f, 0.505f, 2.215f);


	CarPhysicsComponent carPhysic;
	carPhysic.wheels.reserve(4);

	registry.AddComponents<MeshComponent, TransformComponent, TextureComponent, ControllerComponent,
		CarControllerComponent, DriftComponent, PlayerComponent, CollisionComp, TagComponent>
		(player, std::move(meshComp), std::move(carTransform), std::move(texture), ControllerComponent{},
			CarControllerComponent{}, DriftComponent{}, PlayerComponent{}, std::move(collider), TagComponent{ EntityTag::Body });


	auto backWheelsEntity = registry.CreateEntity();

	MeshComponent backWheels;
	backWheels.mesh = &MeshLoader::Load("Models\\Car\\Wheels\\BackWheels.obj", window.App());

	TransformComponent bWheelsTransform;
	bWheelsTransform.SetPosition({ 0.f, 0.f, 0.f });
	bWheelsTransform.SetRotation({ 0, glm::radians(180.0f), 0});

	TextureComponent WheelsTexture;
	WheelsTexture.SetSize(backWheels.mesh->GetSubMeshesCount());
	for (int i = 0; i < backWheels.mesh->GetSubMeshesCount(); ++i)
		WheelsTexture.AddTexture(i, &TextureLoader::Load("Textures\\rouge.jpg", window.App()));

	registry.AddComponents<MeshComponent, TransformComponent, TextureComponent, ControllerComponent,
		CarControllerComponent, DriftComponent, PlayerComponent, TagComponent>
		(backWheelsEntity, std::move(backWheels), std::move(bWheelsTransform), std::move(WheelsTexture), ControllerComponent{},
			CarControllerComponent{}, DriftComponent{}, PlayerComponent{}, TagComponent{ EntityTag::BackWheels });


	auto rightWheelEntity = registry.CreateEntity();

	MeshComponent R_FrontWheel;
	R_FrontWheel.mesh = &MeshLoader::Load("Models\\Car\\Wheels\\Right_FrontWheel.obj", window.App());

	TransformComponent rWheelTransform;
	rWheelTransform.SetPosition({ 0.f, 0.f, 0.f });
	rWheelTransform.SetRotation({ 0, glm::radians(180.0f), 0 });

	TextureComponent RWheelsTexture;
	RWheelsTexture.SetSize(R_FrontWheel.mesh->GetSubMeshesCount());
	for (int i = 0; i < R_FrontWheel.mesh->GetSubMeshesCount(); ++i)
		RWheelsTexture.AddTexture(i, &TextureLoader::Load("Textures\\rouge.jpg", window.App()));

	registry.AddComponents<MeshComponent, TransformComponent, TextureComponent, ControllerComponent,
		CarControllerComponent, DriftComponent, PlayerComponent, TagComponent>
		(rightWheelEntity, std::move(R_FrontWheel), std::move(rWheelTransform), std::move(RWheelsTexture), ControllerComponent{},
			CarControllerComponent{}, DriftComponent{}, PlayerComponent{}, TagComponent{ EntityTag::Right_FrontWheel });


	auto leftWheelEntity = registry.CreateEntity();

	MeshComponent L_FrontWheel;
	L_FrontWheel.mesh = &MeshLoader::Load("Models\\Car\\Wheels\\Left_FrontWheel.obj", window.App());

	TransformComponent lWheelTransform;
	lWheelTransform.SetPosition({ 0.f, 0.f, 0.f });
	lWheelTransform.SetRotation({ 0, glm::radians(180.0f), 0 });

	TextureComponent LWheelsTexture;
	LWheelsTexture.SetSize(L_FrontWheel.mesh->GetSubMeshesCount());
	for (int i = 0; i < L_FrontWheel.mesh->GetSubMeshesCount(); ++i)
		LWheelsTexture.AddTexture(i, &TextureLoader::Load("Textures\\rouge.jpg", window.App()));

	registry.AddComponents<MeshComponent, TransformComponent, TextureComponent, ControllerComponent,
		CarControllerComponent, DriftComponent, PlayerComponent, TagComponent>
		(leftWheelEntity, std::move(L_FrontWheel), std::move(lWheelTransform), std::move(LWheelsTexture), ControllerComponent{},
			CarControllerComponent{}, DriftComponent{}, PlayerComponent{}, TagComponent{ EntityTag::Left_FrontWheel });

	CarControllerComponent, DriftComponent, CarPhysicsComponent, PlayerComponent, CollisionComp>
		(player, std::move(meshComp), std::move(carTransform), std::move(texture), ControllerComponent{},
		CarControllerComponent{}, DriftComponent{}, std::move(carPhysic), PlayerComponent{}, std::move(collider));

	auto createWheel = [&](const std::string& path, glm::vec3 offset, bool driven, bool steerable)
		{
			auto w = registry.CreateEntity();

			MeshComponent mesh;
			mesh.mesh = &MeshLoader::Load(path, window.App());

			TransformComponent transform;
			transform.SetPosition(offset);

			TextureComponent texture;
			texture.SetSize(mesh.mesh->GetSubMeshesCount());
			for (int i = 0; i < mesh.mesh->GetSubMeshesCount(); ++i)
				texture.AddTexture(i, &TextureLoader::Load("Textures/rouge.jpg", window.App()));

			WheelComponent wheel;
			wheel.isDriven = driven;
			wheel.isSteerable = steerable;
			wheel.visualOffset = offset;
			wheel.carBody = player;

			registry.AddComponents<
				MeshComponent, TransformComponent, TextureComponent, WheelComponent>
				(w, std::move(mesh), std::move(transform), std::move(texture), std::move(wheel));

			return w;
		};

	//Create the four wheels with appropriate offsets and properties
	auto frontLeft = createWheel("Models/Car/Left_FrontWheel.obj", { 0.4f, 0.0f, 0.0f }, false, true);
	auto frontRight = createWheel("Models/Car/Right_FrontWheel.obj", { -0.4f, 0.0f, 0.0f }, false, true);
	auto backWheels = createWheel("Models/Car/BackWheels.obj", { -0.04f, 0.0f, -1.25f }, true, false);

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
	follow.distance = 4.0f;
	follow.baseDistance = 4.0f;
	follow.minDistance = 1.0f;
	follow.height = 1.5f;
	follow.smooth = 8.0f;
	follow.lookSmooth = 12.0f;
	follow.fov = 45.0f;
	follow.baseFov = 45.0f;
	follow.speedInfluence = 0.01f;
	follow.speedDistanceInfluence = 0.09f;

	registry.AddComponents<CameraComponent, TransformComponent, CarCameraComponent>(cam, std::move(camComp), std::move(camTransform), std::move(follow));
}

void Player::Update(ecsType& registry, float deltaTime)
{
	glm::vec3 carPos{ 0.0f };
	glm::vec3 carRot{ 0.0f };
	float targetRoll = 0.0f;

	bwheelRollingAngle += deltaTime * 10.0f;
	fwheelRollingAngle += deltaTime * 5.0f;

	{
		auto view = registry.GetAllComponentsView<PlayerComponent, TransformComponent, TagComponent>();
		for (auto e : view)
		{
			if (registry.HasComponent<CollisionComp>(e))
			{
				auto& carController = registry.GetComponent<CarControllerComponent>(e);
				auto& carTransform = registry.GetComponent<TransformComponent>(e);

				carPos = carTransform.GetPosition();
				carRot = carTransform.GetRotation();
				glm::vec3 currentRot = carTransform.GetRotation();

				float maxRollAngle = glm::radians(5.0f);

				targetRoll = -carController.steering * maxRollAngle;

				targetRoll = glm::mix(currentRot.z, targetRoll, deltaTime * 5.0f);

				break;
			}
		}

		for (auto e : view)
		{
			if (!registry.HasComponent<CollisionComp>(e))
			{
				auto& tag = registry.GetComponent<TagComponent>(e).tag;
				auto& wheelTr = registry.GetComponent<TransformComponent>(e);
				auto& carController = registry.GetComponent<CarControllerComponent>(e);

				glm::vec3 LocalOffset{ 0.0f };
				float steerAngle = 0.0f;

				if (tag == EntityTag::BackWheels)
				{
					LocalOffset = { -0.04f, 0.0f, -1.25f };
				}

				if (tag == EntityTag::Right_FrontWheel)
				{
					LocalOffset = { -0.4f, 0.0f, 0.0f };
				}
				if (tag == EntityTag::Left_FrontWheel)
				{
					LocalOffset = { 0.4f, 0.0f, 0.0f };
				}

				if (tag == EntityTag::Right_FrontWheel || tag == EntityTag::Left_FrontWheel)
				{
					steerAngle = std::clamp(carController.steering, -0.7f, 0.7f);
				}
				
				glm::mat4 rotationMatrix = glm::mat4(1.0f);
				rotationMatrix = glm::rotate(rotationMatrix, carRot.y, glm::vec3(0, 1, 0));
				rotationMatrix = glm::rotate(rotationMatrix, carRot.x, glm::vec3(1, 0, 0));
				rotationMatrix = glm::rotate(rotationMatrix, carRot.z, glm::vec3(0, 0, 1));

				glm::vec3 rotateOffset = glm::vec3(rotationMatrix * glm::vec4(LocalOffset, 1.0f));

				wheelTr.SetPosition(carPos + rotateOffset);

				glm::mat4 combinedRotation = rotationMatrix;

				if (steerAngle != 0.0f) combinedRotation = glm::rotate(combinedRotation, steerAngle, { 0,1,0 });

				combinedRotation = glm::rotate(combinedRotation, bwheelRollingAngle, glm::vec3(1, 0, 0));

				glm::quat qRotation = glm::quat(combinedRotation);
				glm::vec3 finalRotation = glm::eulerAngles(qRotation);

				wheelTr.SetRotation(finalRotation);
			}
		}
	}
}