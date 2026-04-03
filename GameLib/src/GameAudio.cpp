#include "GameAudio.h"
#include "GameComponents/CarAudioComponent.h"
#include "GameComponents/CarControllerComponent.h"

void GameAudio::Create(ecsType& registry)
{
	KGR::Audio::WavComponent::Init("Ressources/Musics/");
	auto player = registry.GetAllComponentsView<CarControllerComponent>();
	auto playerEntity = *player.begin();

	auto entity = registry.CreateEntity();

	CarAudioComponent audio;


	// Chargement des fichiers
	audio.idleWav = KGR::Audio::LoadWav("../../Ressources/Musics/Car/Engine/SFX_Idle_Test.wav");
	audio.accelWav = KGR::Audio::LoadWav("../../Ressources/Musics/Car/Engine/SFX_Acceleration.wav");
	audio.decelWav = KGR::Audio::LoadWav("../../Ressources/Musics/Car/Engine/SFX_Deceleration.wav");

	audio.ChargTurboWav = KGR::Audio::LoadWav("../../Ressources/Musics/Car/Turbo/SFX_Turbo_Charging.wav");
	audio.ExhTurboWav = KGR::Audio::LoadWav("../../Ressources/Musics/Car/Turbo/SFX_Turbo_Exhausted.wav");

	//Assignation aux WavComp
	audio.idleSound.SetWav(*audio.idleWav);
	audio.accelSound.SetWav(*audio.accelWav);
	audio.decelSound.SetWav(*audio.decelWav);

	audio.ChargingTurboSound.SetWav(*audio.ChargTurboWav);
	audio.ExhaustedTurboSound.SetWav(*audio.ExhTurboWav);

	audio.idleSound.SetLoop(true);
	audio.accelSound.SetLoop(true);
	audio.decelSound.SetLoop(true);

	audio.idleSound.Play();

	registry.AddComponents<CarAudioComponent>(playerEntity, std::move(audio));
}

void GameAudio::Update(ecsType& registry, float deltaTime)
{
	auto view = registry.GetAllComponentsView<CarAudioComponent, CarControllerComponent>();

	for (auto& e : view)
	{
		auto& car = registry.GetComponent<CarControllerComponent>(e);
		auto& audio = registry.GetComponent<CarAudioComponent>(e);

		if (car.acceleration > 0.0f)
			if(timer < 16.0f)
				timer += 2 * deltaTime;

		//if (car.speed > 0.0f && car.acceleration == 0.0f)
		if (car.acceleration == -1.0f)
			if(timer > 0.0f)
				timer -= 2 * deltaTime;
	}

	audioSystem.Update(registry, deltaTime, timer);
}
