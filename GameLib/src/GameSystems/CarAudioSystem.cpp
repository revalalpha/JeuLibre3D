#include "GameSystems/CarAudioSystem.h"
#include "GameComponents/CarAudioComponent.h"
#include "GameComponents/CarControllerComponent.h"

void CarAudioSystem::Update(ecsType& registry, float deltaTime, float timer)
{
	auto entity = registry.GetAllComponentsView<CarAudioComponent, CarControllerComponent>();

	for (auto e : entity)
	{
		auto& audio = registry.GetComponent<CarAudioComponent>(e);
		auto& car = registry.GetComponent<CarControllerComponent>(e);

		CarState newState = CarState::Idle;

		if (car.acceleration > 0.0f)
			newState = CarState::Accelerating;
		else if (car.speed > 0.0f)
			newState = CarState::Decelerating;
		else if (car.speed <= 0.0f && car.acceleration <= 0.0f)
			newState = CarState::Idle;
		else if (car.acceleration < 0.0f)
			newState = CarState::Braking;

		if (timer >= 16.0f)
			newState = CarState::MaxAccelerating;

		// Transition de son uniquement si l'état change
		if (newState != audio.state)
		{
			// Stopper le son actuel
			switch (audio.state)
			{
			case CarState::Idle : audio.idleSound.Stop(); break;

			case CarState::Accelerating :
				audio.accelSound.Stop();

				//if(audio.turboSound.IsPlaying())
				//	audio.turboSound.Stop();
				break;

			case CarState::Decelerating :
				audio.decelSound.Stop();
				break;

			case CarState::Braking : audio.brakingSound.Stop(); break;

			case CarState::MaxAccelerating: audio.maxAccelSound.Stop(); break;
			}

			// Jouer le nouveau son
			audio.state = newState;
			switch (audio.state)
			{
			case CarState::Idle : audio.idleSound.Play(); break;

			case CarState::Accelerating :
				audio.accelSound.PlayAt(timer);
				//if(!audio.turboSound.IsPlaying())
				//	audio.turboSound.Play();
				break;

			case CarState::Decelerating :
				audio.decelSound.SetVolume(car.speed / 10.f);
				audio.decelSound.Play();
				break;

			case CarState::Braking : audio.brakingSound.Play(); break;

			case CarState::MaxAccelerating: audio.maxAccelSound.Play(); break;
			}
		}
	}
}