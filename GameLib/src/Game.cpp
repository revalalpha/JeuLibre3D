#include "Game.h"
#include <filesystem>
#include <iostream>

#include "Math/CollisionComponent.h"
#include "Core/CameraComponent.h"
#include "VulkanCore.h"
#include "_GLFW.h"
#include "Core/ManagerImple.h"
#include "Core/Mesh.h"
#include "Core/TrasformComponent.h"
#include "Core/LightComponent.h"
#include "GameComponents/TrackComponent.h"
#include "Core/Spline.h"
#include "Core/Frenet.h"
#include "Core/Texture.h"
#include "Core/Window.h"
#include "ECS/Registry.h"
#include "ECS/Entities.h"
#include "Tools/Random.h"
#include "DebugRenderer.h"
#include "Event.h"
#include "EventBus.h"
#include "Tools/Chrono.h"
#include "Player.h"
#include "Track.h"
#include "GameSystems/CameraSystem.h"
#include "GameSystems/CarControlSystem.h"
#include "GameSystems/CarPhysicsSystem.h"
#include "GameSystems/WheelSystem.h"
#include "GameSystems/DriftSystem.h"
#include "GameSystems/CarCollisionSystem.h"
#include "Core/InputManager.h"
#include "GameRenderer.h"
#include "GameAudio.h"
#include "DebugRenderer.h"

GameAudio gameAudio;

void Game::Init(const std::string& fp)
{
	std::filesystem::path exePath = fp;
	std::filesystem::path projectRoot = exePath.parent_path().parent_path().parent_path().parent_path().parent_path();
	KGR::RenderWindow::Init();
	window = std::make_unique<KGR::RenderWindow>(glm::vec2{ 1920,1080 }, "test", projectRoot / "Ressources");
	window->GetInputManager()->SetMode(GLFW_CURSOR_DISABLED);

	menuCam = registry.CreateEntity();
	registry.AddComponents<CameraComponent, TransformComponent>(menuCam, CameraComponent{}, TransformComponent{});

	auto& cam = registry.GetComponent<CameraComponent>(menuCam);
	auto& tr = registry.GetComponent<TransformComponent>(menuCam);

	cam.SetType(CameraComponent::Type::Ortho);
	tr.SetPosition({ 0,0,1 });

	window->RegisterCam(cam, tr);


	//Player
	Player player;
	player.CreatePlayer(registry, *window);

	//Audio
	//gameAudio.Create(registry);

	state = GameState::Menu;
	CreateMenu();

	//Track
	Track track;
	track.CreateTrack(registry, *window);

	KGR::Tools::Random rd;
	auto yScale = rd.getRandomNumberRange(5.0f, 100.0f, 8);

	auto colorTransform = [](const glm::vec3& color)
		{
			return glm::vec3{ color.x / 255.0f, color.y / 255.0f, color.z / 255.0f };
		};

	{
		auto light = registry.CreateEntity();
		auto lComp = LightComponent<LightData::Type::Directional>::Create(colorTransform({ 255, 240, 200 }), { 1,1,1 }, 1.0f);
		TransformComponent lTransform;
		lTransform.LookAt({ 1,-1,0 });
		registry.AddComponents<LightComponent<LightData::Type::Directional>, TransformComponent>(light, std::move(lComp), std::move(lTransform));
	}
}

void Game::CreateMenu()
{
	menuBG = registry.CreateEntity();

	// UI
	registry.AddComponent<UiComponent>(menuBG);
	auto& ui = registry.GetComponent<UiComponent>(menuBG);
	ui.SetVr({ 1920,1080 });
	ui.SetPos({ 960,540 });
	ui.SetScale({ 1920,1080 });
	ui.SetAnchor(UiComponent::Anchor::Center);

	// Transform
	registry.AddComponent<TransformComponent2d>(menuBG);
	auto& tr = registry.GetComponent<TransformComponent2d>(menuBG);
	tr.SetPosition({ 0,0 });
	tr.SetScale({ 1,1 });

	// Texture
	registry.AddComponent<TextureComponent>(menuBG);
	auto& tex = registry.GetComponent<TextureComponent>(menuBG);
	tex.SetSize(1);
	tex.AddTexture(0, LoadTexture("Textures/Menu.png", window->App()).release());
}

void Game::RenderMainMenu()
{
	if (window->GetInputManager()->IsKeyPressed(KGR::SpecialKey::Enter))
	{
		//Safe delete before destroying
		if (registry.HasComponent<TextureComponent>(menuBG))
		{
			auto& tex = registry.GetComponent<TextureComponent>(menuBG);
			for (auto& t : tex.GetAllTextures())
				delete t;
		}
		registry.DestroyEntity(menuCam);
		registry.DestroyEntity(menuBG);

		window->GetInputManager()->SetMode(GLFW_CURSOR_DISABLED);

		state = GameState::Playing;
		return;
	}

	auto& cam = registry.GetComponent<CameraComponent>(menuCam);
	auto& tr3 = registry.GetComponent<TransformComponent>(menuCam);
	window->RegisterCam(cam, tr3);

	auto& ui = registry.GetComponent<UiComponent>(menuBG);
	auto& tr = registry.GetComponent<TransformComponent2d>(menuBG);
	auto& tex = registry.GetComponent<TextureComponent>(menuBG);

	window->RegisterUi(ui, tr, tex);
	window->Render({ 0,0,0,1 });
}


void Game::UpdateGame(float dt)
{
	//Camera control
	CameraSystem camSystem;
	camSystem.Update(registry, dt);

	//Car control
	CarControlSystem carControl;
	carControl.Update(registry, *window, dt);

	//Player pos
	Player player;
	player.Update(registry, dt);

	//Game Audio
	//gameAudio.Update(registry, dt);

	//Car physics
	CarPhysicsSystem carPhysic;
	carPhysic.Update(registry, dt);

	//Wheel physic
	WheelSystem wheelSystem;
	wheelSystem.Update(registry, dt);
	wheelSystem.Visualize(registry, dt);

	//Drift
	DriftSystem driftSystem;
	driftSystem.Update(registry, dt);

	static bool debugCollision = false;

	if (window->GetInputManager()->IsKeyPressed(KGR::Key::E))
		debugCollision = !debugCollision;

	CarCollisionSystem collisionSystem;
	collisionSystem.Update(registry, *window, dt, debugCollision);

	bool gameOver = false;

	//crash
	if (gameOver)
	{
		window->Render({ 0.2f,0.0f,0.0f,1.0f });
		return;
	}
}

void Game::Run(const KGR::Tools::Chrono<float>::Time& fixedTime)
{
	//initiate the game update clock
	const KGR::Tools::Chrono<float> clock;
	float previous = clock.GetElapsedTime().AsMilliSeconds();
	//first version of frameRate limitation
	float renderFrameDt = clock.GetElapsedTime().AsMilliSeconds();
	// dt
	const float fixTick = fixedTime.AsMilliSeconds();
	// lag between 2 render 
	auto lag = 0.0f;
	while (!window->ShouldClose())
	{
		window->App()->GetDebugRenderer().BeginFrame();

		// lag incrementation 
		const float startframetime = clock.GetElapsedTime().AsMilliSeconds();
		const auto elapsed = startframetime - previous;
		previous = startframetime;
		lag += elapsed;
		//set scene
		//Input 

		if (state == GameState::Menu)
		{
			KGR::RenderWindow::PollEvent();
			window->Update();
			RenderMainMenu();
			continue;
		}

		

		// need improvement if update is too heavy and create lag maybe separate physics and update for animation 
		while (lag >= fixTick)
		{
			KGR::RenderWindow::PollEvent();
			window->Update();
			//Update 
			UpdateGame(fixedTime.AsSeconds());
			lag -= fixTick;
		}
		//Render if the frame rate is in range need more test 
		Render();
	}
	window->Destroy();
	KGR::RenderWindow::End();
}

void Game::Render()
{
	GameRenderer renderer;
	// Render Cam
	renderer.RenderCam(registry, *window);
	// Render Mesh
	renderer.RenderMesh(registry, *window);
	// Render Light
	renderer.RenderLight(registry, *window);

	window->Render({ 0.53f, 0.81f, 0.92f, 1.0f });
}