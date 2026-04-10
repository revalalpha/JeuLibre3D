
#include <string>


#include "GameSystems/ScoreSystem.h"
#include "GameComponents/DriftComponent.h"
#include "GameComponents/CarPhysicsComponent.h"
#include "Math/CollisionComponent.h"
#include "Core/Transform2dComponent.h"
#include "Core/TextComponent.h"
#include "Core/UiComponent.h"
#include "Math/SAT.h"
#include "Math/Collision.h"



void ScoreSystem::createScore(ecsType& registry, KGR::RenderWindow& window)
{
	
	{
		//Score
		Score s;
		ScoreComponent m_score;

		TransformComponent2d transform;
		// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
		//transform.SetRotation(glm::radians(-45.0f));
		// create your ui with a virtual resolution and an anchor default center
		UiComponent ui({ 1920,1080 }, UiComponent::Anchor::Center);
		// here set the position in the virtual resolution
		ui.SetPos({ 1920.0f / 2.0f, 1080.0f / 2.0f - 470.f });
		// here the scale
		ui.SetScale({ 120,70 });
		// same as always 
		auto e = registry.CreateEntity();
		TextComp text;
		text.text.font = &FontLoader::Load("Fonts/JeuLibre_Regular_1.ttf", window.App(), 30.f);
		text.text.SetText("0");
		text.text.textTexture = &TextureLoader::Load("Textures/font.png", window.App());
		text.text.SetAlign(Text::Align::Center);


		registry.AddComponents(e, std::move(transform), std::move(text), std::move(ui), std::move(m_score), std::move(s));

	}
	{
		//Mult x text
		MultText multText;
		TransformComponent2d transform;
		// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
		//transform.SetRotation(glm::radians(-45.0f));
		// create your ui with a virtual resolution and an anchor default center
		UiComponent ui({ 1920,1080 }, UiComponent::Anchor::Center);
		// here set the position in the virtual resolution
		ui.SetPos({ 1920.0f / 2.0f - 15.f , 1080.0f / 2.0f - 410.f });
		// here the scale
		ui.SetScale({ 100,50 });
		// same as always 
		auto e = registry.CreateEntity();
		TextComp text;
		text.text.font = &FontLoader::Load("Fonts/Roasthink.ttf", window.App(), 30.f);
		text.text.SetText("mult x");
		text.text.textTexture = &TextureLoader::Load("Textures/rouge.jpg", window.App());
		text.text.SetAlign(Text::Align::Center);

		registry.AddComponents(e, std::move(transform), std::move(text), std::move(ui),std::move(multText));

	}
	{

		//Multiplier
		Mult mult;
		TransformComponent2d transform;
		// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
		//transform.SetRotation(glm::radians(-45.0f));
		// create your ui with a virtual resolution and an anchor default center
		UiComponent ui({ 1920,1080 }, UiComponent::Anchor::Center);
		// here set the position in the virtual resolution
		ui.SetPos({ 1920.0f / 2.0f + 50.f , 1080.0f / 2.0f - 410.f });
		// here the scale
		ui.SetScale({ 25,35 });
		// same as always 
		auto e = registry.CreateEntity();
		TextComp text;
		text.text.font = &FontLoader::Load("Fonts/JeuLibre_Regular_1.ttf", window.App(), 30.f);
		text.text.SetText("1");
		text.text.textTexture = &TextureLoader::Load("Textures/rouge.jpg", window.App());
		text.text.SetAlign(Text::Align::Center);

		registry.AddComponents(e, std::move(transform), std::move(text), std::move(ui), std::move(mult));

	}
	{
		//highScoreText
		HighScoreText highScoreText;
		TransformComponent2d transform;
		// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
		//transform.SetRotation(glm::radians(-45.0f));
		// create your ui with a virtual resolution and an anchor default center
		UiComponent ui({ 1920,1080 }, UiComponent::Anchor::Center);
		// here set the position in the virtual resolution
		ui.SetPos({ 1920.0f / 2.0f + 720.f , 1080.0f / 2.0f - 455.f });
		// here the scale
		ui.SetScale({ 110,80 });
		// same as always 
		auto e = registry.CreateEntity();
		TextComp text;
		text.text.font = &FontLoader::Load("Fonts/Roasthink.ttf", window.App(), 30.f);
		text.text.SetText("High :");
		text.text.textTexture = &TextureLoader::Load("Textures/font.png", window.App());
		text.text.SetAlign(Text::Align::Center);

		registry.AddComponents(e, std::move(transform), std::move(text), std::move(ui), std::move(highScoreText));

	}
	{
		//highScore
		HighScore highScore;
		TransformComponent2d transform;
		// here you can set a rotation ( ROTATION FROM THE CENTER OF THE MESH )
		//transform.SetRotation(glm::radians(-45.0f));
		// create your ui with a virtual resolution and an anchor default center
		UiComponent ui({ 1920,1080 }, UiComponent::Anchor::Center);
		// here set the position in the virtual resolution
		ui.SetPos({ 1920.0f / 2.0f +820.f , 1080.0f / 2.0f - 450.f });
		// here the scale
		ui.SetScale({ 80,50 });
		// same as always 
		auto e = registry.CreateEntity();
		TextComp text;
		text.text.font = &FontLoader::Load("Fonts/JeuLibre_Regular_1.ttf", window.App(), 30.f);
		text.text.SetText("0");
		text.text.textTexture = &TextureLoader::Load("Textures/font.png", window.App());
		text.text.SetAlign(Text::Align::Center);

		registry.AddComponents(e, std::move(transform), std::move(text), std::move(ui),std::move(highScore));

	}
	
}
void ScoreSystem::Update(ecsType& registry, float deltaTime, KGR::RenderWindow& window)
{
	auto cars = registry.GetAllComponentsView < DriftComponent, CarPhysicsComponent, TransformComponent, CollisionComp>();

	auto walls = registry.GetAllComponentsView<CollisionComp, TransformComponent>();

	for (auto c : cars)
	{
		auto& drift = registry.GetComponent<DriftComponent>(c);
		auto& physic = registry.GetComponent<CarPhysicsComponent>(c);
		auto& carCol = registry.GetComponent<CollisionComp>(c);
		auto& carTr = registry.GetComponent<TransformComponent>(c);

		glm::mat4 invRot = glm::inverse(carTr.GetRotationMatrix());
		glm::vec3 vLocal = glm::vec3(invRot * glm::vec4(physic.velocity, 0.0f));

		auto carOBB = carCol.collider->ComputeGlobalOBB(
			carTr.GetScale(),
			carTr.GetPosition(),
			carTr.GetOrientation());

		
		auto score = registry.GetAllComponentsView < TransformComponent2d, TextComp, UiComponent, ScoreComponent, Score>();
		for (auto s : score)
		{
			auto& TextScore = registry.GetComponent<TextComp>(s);
			auto& ScoreComp = registry.GetComponent<ScoreComponent>(s);

			ScoreComp.m_scoreManager.Run(vLocal.x, glm::length(physic.velocity), deltaTime);
			for (auto w : walls)
			{

				if (w == c)
					continue;

				auto& wallTr = registry.GetComponent<TransformComponent>(w);
				auto& wallCol = registry.GetComponent<CollisionComp>(w);

				//Global OBB for the wall
				auto wallOBB = wallCol.collider->ComputeGlobalOBB(
					wallTr.GetScale(),
					wallTr.GetPosition(),
					wallTr.GetOrientation());


				//OBB-OBB collision test
				KGR::Collision3D col = KGR::SeparatingAxisTheorem::CheckCollisionOBB3D(carOBB, wallOBB);

				if (col.IsColliding())
				{
					ScoreComp.m_scoreManager.wallHit(glm::length(physic.velocity));
				}
			}
			std::string S = " ";
			if (ScoreComp.m_scoreManager.GetScore() <= 0)
			{
				TextScore.text.font = &FontLoader::Load("Fonts/arial.ttf", window.App(), 30.f);
			}
			else
			{
				TextScore.text.font = &FontLoader::Load("Fonts/JeuLibre_Regular_1.ttf", window.App(), 30.f);
				S = std::to_string(ScoreComp.m_scoreManager.GetScore());
			}
			TextScore.text.SetText(S);

			auto multtext = registry.GetAllComponentsView < TransformComponent2d, TextComp, UiComponent, MultText>();
			for (auto mt : multtext)
			{
				auto& TextMult = registry.GetComponent<TextComp>(mt);

				std::string S = " ";
				if (ScoreComp.m_scoreManager.GetMultiplier() <= 1)
				{
					TextMult.text.font = &FontLoader::Load("Fonts/arial.ttf", window.App(), 30.f);
				}
				else
				{
					TextMult.text.font = &FontLoader::Load("Fonts/Roasthink.ttf", window.App(), 30.f);
					S = "Mult x";
				}
				TextMult.text.SetText(S);
			}

			auto mult = registry.GetAllComponentsView < TransformComponent2d, TextComp, UiComponent, Mult>();
			for (auto m : mult)
			{
				auto& TextMult = registry.GetComponent<TextComp>(m);

				std::string S = " ";
				if (ScoreComp.m_scoreManager.GetMultiplier() <= 1)
				{
					TextMult.text.font = &FontLoader::Load("Fonts/arial.ttf", window.App(), 30.f);
				}
				else
				{
					TextMult.text.font = &FontLoader::Load("Fonts/JeuLibre_Regular_1.ttf", window.App(), 30.f);
					S = std::to_string(ScoreComp.m_scoreManager.GetMultiplier());
				}
				TextMult.text.SetText(S);
			}


			auto highScore = registry.GetAllComponentsView < TransformComponent2d, TextComp, UiComponent, HighScore>();
			for (auto h : highScore)
			{
				auto& TextHighScore = registry.GetComponent<TextComp>(h);
				auto S = std::to_string(ScoreComp.m_scoreManager.GetHighScore());
				TextHighScore.text.SetText(S);
			}
		}
		
		
		
		
	}
	

}
