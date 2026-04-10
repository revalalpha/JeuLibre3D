#include "IGameScene.h"
#include "Tools/Random.h"
#include "GameAudio.h"
#include "Player.h"
#include "Track.h"
#include "GameSystems/CameraSystem.h"
#include "GameSystems/CarControlSystem.h"
#include "GameSystems/CarPhysicsSystem.h"
#include "GameSystems/WheelSystem.h"
#include "GameSystems/DriftSystem.h"
#include "GameSystems/CarCollisionSystem.h"

#include <iostream>

GameAudio gameAudio;

void GameScene::Init(SceneManager* manager)
{
	IGameScene::Init(manager);
	//m_window->GetInputManager()->SetMode(GLFW_CURSOR_DISABLED);

	//Player
	Player player;
	player.CreatePlayer(m_ecs, *m_window);

	//Audio
	gameAudio.Create(m_ecs);

	//Track
	Track track;
	track.CreateMap(m_ecs, *m_window);

	KGR::Tools::Random rd;
	auto yScale = rd.getRandomNumberRange(5.0f, 100.0f, 8);

	auto colorTransform = [](const glm::vec3& color)
		{
			return glm::vec3{ color.x / 255.0f, color.y / 255.0f, color.z / 255.0f };
		};

	{
		auto light = m_ecs.CreateEntity();
		auto lComp = LightComponent<LightData::Type::Directional>::Create(colorTransform({ 255, 240, 200 }), { 1,1,1 }, 1.0f);
		TransformComponent lTransform;
		lTransform.LookAt({ 1,-1,0 });
		m_ecs.AddComponents<LightComponent<LightData::Type::Directional>, TransformComponent>(light, std::move(lComp), std::move(lTransform));
	}

}

void GameScene::Update(float dt)
{
	IGameScene::Update(dt);


	//Camera control
	CameraSystem camSystem;
	camSystem.Update(m_ecs, dt);

	//Car control
	CarControlSystem carControl;
	carControl.Update(m_ecs, *m_window, dt);

	//Player pos
	Player player;
	player.Update(m_ecs, dt);

	//Game Audio
	gameAudio.Update(m_ecs, dt, *m_window);

	//Car physics
	CarPhysicsSystem carPhysic;
	carPhysic.Update(m_ecs, dt);

	//Wheel physic
	WheelSystem wheelSystem;
	wheelSystem.Update(m_ecs, dt);
	wheelSystem.Visualize(m_ecs, dt);

	//Drift
	DriftSystem driftSystem;
	driftSystem.Update(m_ecs, dt);

	static bool debugCollision = false;

	if (m_window->GetInputManager()->IsKeyPressed(KGR::Key::E))
		debugCollision = !debugCollision;

	CarCollisionSystem collisionSystem;
	collisionSystem.Update(m_ecs, *m_window, dt, debugCollision);

	bool gameOver = false;

	//crash
	if (gameOver)
	{
		m_window->Render({ 0.2f,0.0f,0.0f,1.0f });
		return;
	}

	{
		auto input = m_window->GetInputManager();
		if (input->IsKeyDown(KGR::Key::P))
			KGR::EventBus<ChangeSceneEvent>::Notify(ChangeSceneEvent{"Menu"});
	}
}
