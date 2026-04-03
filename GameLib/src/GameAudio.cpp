#include "GameAudio.h"
#include "GameComponents/CarAudioComponent.h"
#include "GameComponents/CarControllerComponent.h"
#include "GameComponents/DriftComponent.h"

void GameAudio::Create(ecsType& registry)
{
	KGR::Audio::WavComponent::Init("Ressources/Musics/");
	auto player = registry.GetAllComponentsView<CarControllerComponent>();
	auto playerEntity = *player.begin();

	auto entity = registry.CreateEntity();

	CarAudioComponent audio;


	// Chargement des fichiers
	audio.idleWav = KGR::Audio::LoadWav("../../Ressources/Musics/Car/Engine/SFX_Idle.wav");
	audio.accelWav = KGR::Audio::LoadWav("../../Ressources/Musics/Car/Engine/SFX_Acceleration.wav");
	audio.maxAccelWav = KGR::Audio::LoadWav("../../Ressources/Musics/Car/Engine/SFX_MaxAccel.wav");
	audio.decelWav = KGR::Audio::LoadWav("../../Ressources/Musics/Car/Engine/SFX_Deceleration.wav");

	audio.turboWav = KGR::Audio::LoadWav("../../Ressources/Musics/Car/Turbo/SFX_Turbo_Charging.wav");

	//Assignation aux WavComp
	audio.idleSound.SetWav(*audio.idleWav);
	audio.accelSound.SetWav(*audio.accelWav);
	audio.maxAccelSound.SetWav(*audio.maxAccelWav);
	audio.decelSound.SetWav(*audio.decelWav);

	audio.turboSound.SetWav(*audio.turboWav);

	audio.idleSound.SetLoop(true);
	audio.maxAccelSound.SetLoop(true);
	audio.turboSound.SetLoop(false);

	audio.idleSound.Play();

	registry.AddComponents<CarAudioComponent>(playerEntity, std::move(audio));
}

void GameAudio::Update(ecsType& registry, float deltaTime)
{
	auto view = registry.GetAllComponentsView<CarAudioComponent, CarControllerComponent, DriftComponent>();

	for (auto& e : view)
	{
		auto& car = registry.GetComponent<CarControllerComponent>(e);
		auto& audio = registry.GetComponent<CarAudioComponent>(e);
		auto& drift = registry.GetComponent<DriftComponent>(e);

		if (car.acceleration > 0.0f)
			if(timer < 16.0f)
				timer += deltaTime;

		//if (car.speed > 0.0f && car.acceleration == 0.0f)
		if ((car.acceleration == 0.0f && car.speed > 0.0f))
			if (timer > 0.0f)
				timer -= 2 * deltaTime;
			else if (timer <= 0.0f)
				timer = 0.0f;

		if (car.speed <= 0.0f)
			timer = 0.0f;

		//if (drift.driftFactor > 0.0f)
		//	timer -= deltaTime * 0.5f;
	}

	audioSystem.Update(registry, deltaTime, timer);
}
