#include "GameSystems/CarAudioSystem.h"
#include "GameComponents/CarAudioComponent.h"
#include "GameComponents/CarControllerComponent.h"
#include "GameComponents/CarPhysicsComponent.h"
#include "Core/InputManager.h"

#include <print>

float computeLayerVolume(const RPMLayer& layer, float rpm)
{
    if (rpm < layer.rpmMin || rpm >= layer.rpmFade) return 0.0f;

    if (rpm < layer.rpmMax)
        return (rpm - layer.rpmMin) / (layer.rpmMax - layer.rpmMin);

    if (rpm < layer.rpmFade)
        return 1.0f - (rpm - layer.rpmMax) / (layer.rpmFade - layer.rpmMin);

    return 0.0f;
}

void CarAudioSystem::Update(ecsType& registry, float deltaTime, KGR::RenderWindow& window)
{
    auto view = registry.GetAllComponentsView<CarAudioComponent, CarControllerComponent, CarPhysicsComponent, TransformComponent>();

    for (auto e : view)
    {
        auto& audio = registry.GetComponent<CarAudioComponent>(e);
        auto& car = registry.GetComponent<CarControllerComponent>(e);
        auto& physic = registry.GetComponent<CarPhysicsComponent>(e);
        auto& transform = registry.GetComponent<TransformComponent>(e);
        auto input = window.GetInputManager();

        // --- RPM simulé ---
        float speedRatio = glm::clamp(car.speed / 15.0f, 0.0f, 1.79f);
        speedRatio = glm::fract(glm::exp(speedRatio)) * (1.0f - speedRatio * 0.4f) + speedRatio * 0.4f;
        audio.targetSpeedRatio = glm::mix(audio.targetSpeedRatio, speedRatio, 0.15f);

        float throttleBoost = car.acceleration > 0.0f ? car.acceleration * 0.3f : 0.0f;

        audio.targetRPM = glm::mix(audio.minRPM, audio.maxRPM, audio.targetSpeedRatio + throttleBoost);
        audio.targetRPM = glm::clamp(audio.targetRPM, audio.minRPM, audio.maxRPM + 1.f);

        // Montée rapide, descente lente
        float smoothSpeed = audio.currentRPM < audio.targetRPM ? audio.rpmSmoothSpeed : audio.rpmSmoothSpeed * 0.4f;
        audio.currentRPM = glm::mix(audio.currentRPM, audio.targetRPM, 1.0f - glm::exp(-smoothSpeed * deltaTime / (1.0f - speedRatio)));

        // --- Moteur : Multi-Layer ---
        constexpr float kPitchThreshold = 0.005f;

        for (auto& layer : audio.engineLayers)
        {
            float rpmRatio = (audio.currentRPM - audio.minRPM) / (audio.maxRPM - audio.minRPM);

            float vol = computeLayerVolume(layer, audio.currentRPM);
            float pitch = audio.currentRPM / layer.rpmMax;

            layer.smoothPitch = glm::mix(layer.smoothPitch, pitch, 0.05f);

            layer.sound.SetVolume(vol);

            if (glm::abs(pitch - audio.lastPitch) > kPitchThreshold)
            {
                layer.sound.SetPitch(glm::clamp(layer.smoothPitch, 0.1f, 2.0f));
                audio.lastPitch = pitch;
            }
        }

        // --- Drift : glissement latéral ---
        glm::mat4 invRot = glm::inverse(transform.GetRotationMatrix());
        glm::vec3 vLocal = glm::vec3(invRot * glm::vec4(physic.velocity, 0.0f));
        float lateralSlip = glm::abs(vLocal.x);

        float driftThreshold = 2.0f;
        float driftVolume = glm::clamp((lateralSlip - driftThreshold) / 5.5f, 0.0f, 1.0f);
        float driftPitch = glm::mix(0.8f, 1.4f, driftVolume);

        if (driftVolume > 0.01f)
        {
            if (!audio.driftSound.IsPlaying()) audio.driftSound.Play();
            audio.driftSound.SetVolume(driftVolume);
            audio.driftSound.SetPitch(driftPitch);
        }
        else if (audio.driftSound.IsPlaying())
        {
            audio.driftSound.Stop();
        }

        // --- Turbo ---
        //float turboVolume = glm::clamp((rpmRatio - 0.6f) / 0.4f, 0.0f, 1.0f);
        //if (!audio.turboSound.IsPlaying()) audio.turboSound.Play();
        //audio.turboSound.SetVolume(turboVolume);

        // --- Pétarade : décélération brutale depuis haut régime ---
        static bool wasDecelerating = false;
        bool isDecelerating = car.acceleration < 0.01f && car.speed > 2.0f;
        bool isHighRPM = audio.currentRPM > 5500.0f;

        bool anyBackFiringPlaying = std::any_of(
            audio.backfireSounds.begin(), audio.backfireSounds.end(),
            [](const KGR::Audio::WavComponent& s) {return s.IsPlaying();}
        );

        if (isDecelerating && !wasDecelerating && isHighRPM && !anyBackFiringPlaying)
        {
            audio.backFireTimer = glm::mix(0.1f, 0.4f, static_cast<float>(std::rand()) / RAND_MAX);
            audio.pendinBackFireIndex = std::rand() % audio.backfireSounds.size();
        }
        wasDecelerating = isDecelerating;

        if (audio.backFireTimer > 0.0f)
        {
            audio.backFireTimer -= deltaTime;

            if (audio.backFireTimer <= 0.0f && !anyBackFiringPlaying)
            {
                audio.backfireSounds[audio.pendinBackFireIndex].SetVolume(0.1f);
                audio.backfireSounds[audio.pendinBackFireIndex].Play();
                audio.backFireTimer = 0.0f;
            }
        }

        // --- Freinage one-shot ---

        static bool wasBraking = false;
        bool isBraking = car.acceleration < 0.0f;
        if (isBraking && !wasBraking && !audio.brakingSound.IsPlaying())
            audio.brakingSound.Play();
        if (wasBraking && !isBraking && audio.brakingSound.IsPlaying())
            audio.brakingSound.Stop();
        wasBraking = isBraking;

        if (input->IsKeyPressed(KGR::Key::R))
        {
            audio.radioActive = !audio.radioActive;
            audio.RadioSound.SetVolume(audio.radioActive ? audio.radioVolume : 0.0f);
        }

        if (audio.radioActive)
        {
            if (input->IsKeyDown(KGR::Key::O))
            {
                audio.radioVolume = glm::clamp(
                    audio.radioVolume + audio.radioVolumeStep * deltaTime,
                    0.0f, 3.0f
                );
                audio.RadioSound.SetVolume(audio.radioVolume);
            }

            if (input->IsKeyDown(KGR::Key::I))
            {
                audio.radioVolume = glm::clamp(
                    audio.radioVolume - audio.radioVolumeStep * deltaTime,
                    0.0f, 3.0f
                );
                audio.RadioSound.SetVolume(audio.radioVolume);
            }
        }
    }
}