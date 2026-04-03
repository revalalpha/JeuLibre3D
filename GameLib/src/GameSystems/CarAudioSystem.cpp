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

		// Transition de son uniquement si l'état change
		if (newState != audio.state)
		{
			// Stopper le son actuel
			switch (audio.state)
			{
			case CarState::Idle : audio.idleSound.Stop(); break;

			case CarState::Accelerating :
				audio.accelSound.Stop();
				//audio.ExhaustedTurboSound.Stop();
				break;

			case CarState::Decelerating :
				audio.decelSound.Stop();
				break;

			case CarState::Braking : audio.brakingSound.Stop(); break;
			}

			// Jouer le nouveau son
			audio.state = newState;
			switch (audio.state)
			{
			case CarState::Idle : audio.idleSound.Play(); break;

			case CarState::Accelerating :
				audio.accelSound.PlayAt(timer);
				//audio.ChargingTurboSound.Play();
				break;

			case CarState::Decelerating :
				audio.decelSound.Play();
				audio.ExhaustedTurboSound.Play();
				break;

			case CarState::Braking : audio.brakingSound.Play(); break;
			}
		}
	}
}