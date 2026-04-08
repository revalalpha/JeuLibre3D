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
	struct LayerDef { const char* file; float rpmMin, rpmMax, rpmFade; };

	std::vector<LayerDef> defs = {
		{ "../../Ressources/Musics/Car/Engine/SFX_Idle.wav",    600.f, 1000.f, 1800.f },
		{ "../../Ressources/Musics/Car/Engine/SFX_Low.wav",    1000.f, 1800.f, 3500.f },
		{ "../../Ressources/Musics/Car/Engine/SFX_Mid.wav",    1800.f, 3500.f, 5500.f },
		{ "../../Ressources/Musics/Car/Engine/SFX_High.wav",   3500.f, 5500.f, 8000.f },
		{ "../../Ressources/Musics/Car/Engine/SFX_Redline.wav",7999.f, 8000.f, 8010.f },
	};

	for (auto& def : defs)
	{
		RPMLayer layer;
		layer.wav = KGR::Audio::LoadWav(def.file);
		layer.sound.SetWav(*layer.wav);
		layer.sound.SetLoop(true);
		layer.rpmMin = def.rpmMin;
		layer.rpmMax = def.rpmMax;
		layer.rpmFade = def.rpmFade;
		layer.sound.PlayLooped(0.0f); // tous jouent en permanence, volume à 0
		audio.engineLayers.push_back(std::move(layer));
	}

	// Chargement des fichiers
	audio.driftWav = KGR::Audio::LoadWav("../../Ressources/Musics/Car/Tire/SFX_Tire_Noise.wav");
	audio.turboWav = KGR::Audio::LoadWav("../../Ressources/Musics/Car/Turbo/SFX_Turbo_Charging.wav");
	audio.brakingWav = KGR::Audio::LoadWav("../../Ressources/Musics/Car/Tire/SFX_Tire_Noise.wav");
	audio.RadioWav = KGR::Audio::LoadWav("../../Ressources/Musics/Radio_TurboDrift.mp3");

	audio.backfireWavs.push_back(KGR::Audio::LoadWav("../../Ressources/Musics/Car/Engine/Back Fire/SFX_BackFire_1.wav"));
	audio.backfireWavs.push_back(KGR::Audio::LoadWav("../../Ressources/Musics/Car/Engine/Back Fire/SFX_BackFire_2.wav"));
	audio.backfireWavs.push_back(KGR::Audio::LoadWav("../../Ressources/Musics/Car/Engine/Back Fire/SFX_BackFire_3.wav"));

	//Assignation aux WavComp
	audio.driftSound.SetWav(*audio.driftWav);
	audio.turboSound.SetWav(*audio.turboWav);
	audio.brakingSound.SetWav(*audio.brakingWav);
	audio.RadioSound.SetWav(*audio.RadioWav);

	for (auto& w : audio.backfireWavs)
	{
		KGR::Audio::WavComponent sound;
		sound.SetWav(*w);
		audio.backfireSounds.push_back(std::move(sound));
	}

	audio.driftSound.SetLoop(true);
	audio.brakingSound.SetLoop(true);
	audio.RadioSound.SetLoop(true);


	float randTimer = static_cast<float>(std::rand() % (30*60));
	audio.RadioSound.PlayAt(randTimer);
	audio.RadioSound.SetVolume(0.0f);

	registry.AddComponents<CarAudioComponent>(playerEntity, std::move(audio));
}

void GameAudio::Update(ecsType& registry, float deltaTime, KGR::RenderWindow& window)
{
	auto view = registry.GetAllComponentsView<CarAudioComponent, CarControllerComponent, DriftComponent>();

	for (auto& e : view)
	{
		auto& car = registry.GetComponent<CarControllerComponent>(e);

		if (car.speed <= 0.0f)
			timer = 0.0f;
		else if (glm::abs(car.acceleration > 0.0f))
		{
			timer = glm::max(0.0f, timer - 2.0f * deltaTime);
		}
		else if (car.acceleration > 0.0f)
		{
			if (glm::abs(car.steering) > 0.1f)
				timer = glm::max(0.0f, timer - deltaTime);
			else
				timer = glm::min(16.0f, timer + deltaTime);
		}
		else
			timer = glm::max(0.0f, timer - 2.0f * deltaTime);
	}

	audioSystem.Update(registry, deltaTime, window);
}
