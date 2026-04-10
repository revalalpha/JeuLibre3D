#pragma once
#include "Core/Scene.h"
#include <filesystem>

#include "Audio/SoundComponent.h"
#include "Core/SceneManager.h"
#include "Core/Scene.h"

#include "Core/Window.h"
#include "ECS/Entities.h"
#include "ECS/Registry.h"

#include "Core/Transform2dComponent.h"
#include "Core/UiComponent.h"


#include "Core/InputManager.h"
#include "Core/CameraComponent.h"
#include "Core/TrasformComponent.h"
#include "Core/Mesh.h"
#include "Core/Texture.h"
#include "Math/Collision2d.h"

#include "Core/Materials.h"
#include "EventBus.h"
#include "Core/Font.h"
using ecsType = KGR::ECS::Registry<KGR::ECS::Entity::_64, 100>;


struct ChangeSceneEvent
{
	std::string targetScene;
};
struct ExitEvent
{
	
};

struct GameSceneManager : public SceneManager
{
	GameSceneManager(const std::filesystem::path& path)
	{
		KGR::RenderWindow::Init();
		m_window = std::make_unique<KGR::RenderWindow>(glm::vec2{ 1920,800 }, "Turbo Drift", path);
		KGR::Audio::WavComponent::Init();
		KGR::EventBus<ChangeSceneEvent>::AddListener(this);
		KGR::EventBus<ChangeSceneEvent>::AddCallBack<GameSceneManager>(&GameSceneManager::ChangeScene);
		KGR::EventBus<ExitEvent>::AddListener(this);
		KGR::EventBus<ExitEvent>::AddCallBack<GameSceneManager>(&GameSceneManager::Close);
	}
	~GameSceneManager() override
	{
		KGR::EventBus<ChangeSceneEvent>::RemoveListener(this);
		KGR::EventBus<ExitEvent>::RemoveListener(this);
	}
	KGR::RenderWindow* GetWindow() const 
	{
		return m_window.get();
	}
	void Close(const ExitEvent&)
	{
		IsOpen = false;
	}
	bool LoopCondition() const override
	{
		if (!IsOpen)
			return false;
		return !m_window->ShouldClose();
	}
	void Destroy() override
	{
		SceneManager::Destroy();
		m_window->Destroy();
		KGR::RenderWindow::End();
	}
	void ChangeScene(const ChangeSceneEvent& event)
	{
		SetCurrentScene(event.targetScene);
	}
private:
	std::unique_ptr<KGR::RenderWindow> m_window;
	bool IsOpen = true;
};

struct IGameScene : public Scene
{
	IGameScene(const KGR::Tools::Chrono<float>::Time& time):Scene(time),m_window(nullptr){}
	void Init(SceneManager* manager) override
	{
		auto m = static_cast<GameSceneManager*>(manager);
		m_window = m->GetWindow();
	}
	void Update(float dt) override
	{
		KGR::RenderWindow::PollEvent();
		m_window->Update();
	}
	void Render() override 
	{
		{
			auto es = m_ecs.GetAllComponentsView<CameraComponent, TransformComponent>();
			if (es.size() != 1)
				throw std::runtime_error("need one and one cam");
			for (auto& e : es)
			{
				m_ecs.GetComponent<CameraComponent>(e).UpdateCamera(m_ecs.GetComponent<TransformComponent>(e).GetFullTransform());
				m_ecs.GetComponent<CameraComponent>(e).SetAspect(m_window->GetSize().x, m_window->GetSize().y);
				m_window->RegisterCam(m_ecs.GetComponent<CameraComponent>(e), m_ecs.GetComponent<TransformComponent>(e));
			}
		}


		{
			auto es = m_ecs.GetAllComponentsView<MeshComponent, TransformComponent, MaterialComponent>();
			for (auto& e : es)
			{
				m_window->RegisterRender(
					m_ecs.GetComponent<MeshComponent>(e),
					m_ecs.GetComponent<TransformComponent>(e),
					m_ecs.GetComponent<MaterialComponent>(e));
			}

		}
		{
			auto es = m_ecs.GetAllComponentsView<LightComponent<LightData::Type::Point>, TransformComponent>();
			for (auto& e : es)
				m_window->RegisterLight(m_ecs.GetComponent<LightComponent<LightData::Type::Point>>(e), m_ecs.GetComponent<TransformComponent>(e));
		}
		{
			auto es = m_ecs.GetAllComponentsView<LightComponent<LightData::Type::Spot>, TransformComponent>();
			for (auto& e : es)
				m_window->RegisterLight(m_ecs.GetComponent<LightComponent<LightData::Type::Spot>>(e), m_ecs.GetComponent<TransformComponent>(e));
		}
		{
			auto es = m_ecs.GetAllComponentsView<LightComponent<LightData::Type::Directional>, TransformComponent>();
			for (auto& e : es)
				m_window->RegisterLight(m_ecs.GetComponent<LightComponent<LightData::Type::Directional>>(e), m_ecs.GetComponent<TransformComponent>(e));
		}
		{
			auto es = m_ecs.GetAllComponentsView < TextureComponent, TransformComponent2d, UiComponent >();
			for (auto& e : es)
			{
				auto transform = m_ecs.GetComponent<TransformComponent2d>(e);
				auto ui = m_ecs.GetComponent<UiComponent>(e);
				auto texture = m_ecs.GetComponent<TextureComponent>(e);
				m_window->RegisterUi(ui, transform, texture);
			}
		}
		{
			auto es = m_ecs.GetAllComponentsView < TextComp, TransformComponent2d, UiComponent >();
			for (auto& e : es)
			{
				auto& transform = m_ecs.GetComponent<TransformComponent2d>(e);
				auto& ui = m_ecs.GetComponent<UiComponent>(e);
				auto& text = m_ecs.GetComponent<TextComp>(e);
				m_window->RegisterText(ui, transform, text);
			}
		}
		m_window->Render({ 0.53f, 0.81f, 0.92f, 1.0f });
	}
protected:
	ecsType m_ecs;
	KGR::RenderWindow* m_window;
};

struct control
{
	
};


struct GameScene : public IGameScene
{
	
	GameScene(const KGR::Tools::Chrono<float>::Time& time) :IGameScene(time){}
	void Init(SceneManager* manager) override;
	
	void Update(float dt) override;
	
	void Render() override
	{
		
		IGameScene::Render();
	}
};

struct CSComp
{
	std::string targetScene;
};
struct ExitComp
{
};

struct MenuScene : public IGameScene
{

	MenuScene(const KGR::Tools::Chrono<float>::Time& time) :IGameScene(time) {}

	void Init(SceneManager* manager) override
	{
		IGameScene::Init(manager);

		{
			// a calera need a cameraComponent that can be orthographic or perspective and a transform

			// create the camera with the fov , the size of the window (must be updated ) and the far and near rendering and the mode 
			CameraComponent cam = CameraComponent::Create(glm::radians(45.0f), m_window->GetSize().x, m_window->GetSize().y, 0.01f, 100.0f, CameraComponent::Type::Perspective);
			TransformComponent transform;
			// create a transform and set pos and dir 
			transform.SetPosition({ 0,3,5 });
			transform.LookAt({ 0,0,0 });
			// now create an entity , an alias here std::uint64_t
			auto e = m_ecs.CreateEntity();

			// now move the component into the ecs
			m_ecs.AddComponents(e, std::move(cam), std::move(transform));
		}
		{
			// you need texture transform and ui component
			// for the transform it only use for the rotation 
			TransformComponent2d transform;
			// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
			//transform.SetRotation(glm::radians(-45.0f));
			// create your ui with a virtual resolution and an anchor default center
			UiComponent ui({ 1920,1080 }, UiComponent::Anchor::Center);
			// here set the position in the virtual resolution
			ui.SetPos({ 1920.0f/2.0f, 1080.0f/2.0f });
			// here the scale
			ui.SetScale({ 800,1100 });
			// create a texture but be aware that only the first texture in the component will be use 
			TextureComponent texture;
			texture.texture = &TextureLoader::Load("Textures/Menu.png", m_window->App());
			// same as always 
			auto e = m_ecs.CreateEntity();

			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(texture), std::move(CollisionComp2d{}));

		}
		{
			// you need texture transform and ui component
			// for the transform it only use for the rotation 
			TransformComponent2d transform;
			// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
			//transform.SetRotation(glm::radians(-45.0f));
			// create your ui with a virtual resolution and an anchor default center
			UiComponent ui({ 1920,1080 }, UiComponent::Anchor::Center);
			// here set the position in the virtual resolution
			ui.SetPos({ 1920.0f / 2.0f, 1080.0f / 2.0f });
			// here the scale
			ui.SetScale({ 400,200 });
			// create a texture but be aware that only the first texture in the component will be use 
			TextureComponent texture;
			texture.texture = &TextureLoader::Load("Textures/jouer_violet.png", m_window->App());
			CSComp comp;
			comp.targetScene = "Game";
			// same as always 
			auto e = m_ecs.CreateEntity();

			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(texture), std::move(CollisionComp2d{}), std::move(comp));

		}
		{
			// you need texture transform and ui component
			// for the transform it only use for the rotation 
			TransformComponent2d transform;
			// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
			//transform.SetRotation(glm::radians(-45.0f));
			// create your ui with a virtual resolution and an anchor default center
			UiComponent ui({ 1920,1080 }, UiComponent::Anchor::Center);
			// here set the position in the virtual resolution
			ui.SetPos({ 1920.0f / 2.0f, 1080.0f / 2.0f + 300.f});
			// here the scale
			ui.SetScale({ 400,200 });
			// create a texture but be aware that only the first texture in the component will be use 
			TextureComponent texture;
			texture.texture = &TextureLoader::Load("Textures/quitter_violet.png", m_window->App());
			ExitComp comp;
			
			// same as always 
			auto e = m_ecs.CreateEntity();

			m_ecs.AddComponents(e, std::move(transform), std::move(ui), std::move(texture), std::move(CollisionComp2d{}), std::move(comp));

		}
	}
	void Update(float dt) override
	{
		IGameScene::Update(dt);
		// TODO click on button

		{

			auto mousePos = m_window->GetInputManager()->GetMousePosition();
			float aspectRatio = static_cast<float>(m_window->GetSize().x) / static_cast<float>(m_window->GetSize().y);
			auto mouseinAR = UiComponent::VrToNdc(mousePos, m_window->GetSize(), aspectRatio, false);

			auto play = m_ecs.GetAllComponentsView<CollisionComp2d, UiComponent,CSComp>();
			for (auto p : play)
			{
				auto& t = m_ecs.GetComponent<CollisionComp2d>(p);
				auto& u = m_ecs.GetComponent<UiComponent>(p);
				t.Update(u.GetPosNdc(aspectRatio), u.GetScaleNdc(aspectRatio));

				if (t.aabb.IsColliding(mouseinAR))
				{
					u.SetColor({ 1.f,1.f,1.f,1 });
					if (m_window->GetInputManager()->IsMousePressed(KGR::Mouse::Left))
						KGR::EventBus<ChangeSceneEvent>::Notify(ChangeSceneEvent{m_ecs.GetComponent<CSComp>(p).targetScene});
				}
				else
					u.SetColor({ 0.5f,0.5f,0.5f,1 });
			}
			auto es = m_ecs.GetAllComponentsView<CollisionComp2d, UiComponent, ExitComp>();
			for (auto e : es)
			{
				auto& t = m_ecs.GetComponent<CollisionComp2d>(e);
				auto& u = m_ecs.GetComponent<UiComponent>(e);
				t.Update(u.GetPosNdc(aspectRatio), u.GetScaleNdc(aspectRatio));

				if (t.aabb.IsColliding(mouseinAR))
				{
					u.SetColor({ 1.f,1.f,1.f,1 });
					if (m_window->GetInputManager()->IsMousePressed(KGR::Mouse::Left))
						KGR::EventBus<ExitEvent>::Notify(ExitEvent{});
				}
				else
					u.SetColor({ 0.5f,0.5f,0.5f,1 });
			}
			
		}
	}
};