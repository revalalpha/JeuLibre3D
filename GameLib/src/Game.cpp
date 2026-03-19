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
#include "GameRenderer.h"

void Game::Init(const std::string& fp)
{
	std::filesystem::path exePath = fp;
	std::filesystem::path projectRoot = exePath.parent_path().parent_path().parent_path().parent_path().parent_path();
	KGR::RenderWindow::Init();
	window = std::make_unique<KGR::RenderWindow>(glm::vec2{ 1920,1080 }, "test", projectRoot / "Ressources");
	window->GetInputManager()->SetMode(GLFW_CURSOR_DISABLED);

	//Player
	Player player;
	player.CreatePlayer(registry, *window);

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
		// lag incrementation 
		const float startframetime = clock.GetElapsedTime().AsMilliSeconds();
		const auto elapsed = startframetime - previous;
		previous = startframetime;
		lag += elapsed;
		//set scene
		//Input 


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