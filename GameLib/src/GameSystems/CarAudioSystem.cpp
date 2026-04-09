#include "GameSystems/CarAudioSystem.h"
#include "GameComponents/CarAudioComponent.h"
#include "GameComponents/CarControllerComponent.h"
#include "GameComponents/CarPhysicsComponent.h"
#include "Core/InputManager.h"

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
        float speedRatio = glm::clamp(car.speed / 100.0f, 0.0f, 1.0f);
        float throttleBoost = car.acceleration > 0.0f ? car.acceleration * 0.3f : 0.0f;

        audio.targetRPM = glm::mix(audio.minRPM, audio.maxRPM, speedRatio + throttleBoost);
        audio.targetRPM = glm::clamp(audio.targetRPM, audio.minRPM, audio.maxRPM);

        // Montée rapide, descente lente
        float smoothSpeed = audio.currentRPM < audio.targetRPM ? audio.rpmSmoothSpeed : audio.rpmSmoothSpeed * 0.4f;
        audio.currentRPM = glm::mix(audio.currentRPM, audio.targetRPM, 1.0f - glm::exp(-smoothSpeed * deltaTime));

        float rpmRatio = (audio.currentRPM - audio.minRPM) / (audio.maxRPM - audio.minRPM);

        // --- Moteur : Multi-Layer ---
        float globalPitch = glm::mix(0.85f, 1.15f, rpmRatio);

        constexpr float kPitchThreshold = 0.005f;
        bool pitchChanged = glm::abs(globalPitch - audio.lastPitch) > kPitchThreshold;

        for (auto& layer : audio.engineLayers)
        {
            float vol = computeLayerVolume(layer, audio.currentRPM);
            layer.sound.SetVolume(vol);

            if (pitchChanged)
                layer.sound.SetPitch(globalPitch);
        }

        if (pitchChanged)
            audio.lastPitch = globalPitch;

        // --- Drift : glissement latéral ---
        glm::mat4 invRot = glm::inverse(transform.GetRotationMatrix());
        glm::vec3 vLocal = glm::vec3(invRot * glm::vec4(physic.velocity, 0.0f));
        float lateralSlip = glm::abs(vLocal.x);

        float driftThreshold = 2.0f;
        float driftVolume = glm::clamp((lateralSlip - driftThreshold) / 8.0f, 0.0f, 1.0f);
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
        float turboVolume = glm::clamp((rpmRatio - 0.6f) / 0.4f, 0.0f, 1.0f);
        if (!audio.turboSound.IsPlaying()) audio.turboSound.Play();
        audio.turboSound.SetVolume(turboVolume);

        // --- Pétarade : décélération brutale depuis haut régime ---
        bool isDecelerating = car.acceleration < 0.01f && car.speed > 20.0f;
        bool isHighRPM = rpmRatio > 0.7f;

        if (isDecelerating && isHighRPM && !audio.backfireSound.IsPlaying())
            audio.backfireSound.Play();

        // --- Freinage one-shot ---
        static bool wasBraking = false;
        bool isBraking = car.acceleration < 0.0f;
        if (isBraking && !wasBraking && !audio.brakingSound.IsPlaying())
            audio.brakingSound.Play();
        wasBraking = isBraking;

        if (input->IsKeyPressed(KGR::Key::R))
        {
            if (!audio.RadioSound.IsPlaying())
            {
                audio.RadioSound.Play();
                audio.radioActive = true;
            }
                
            else if (audio.RadioSound.IsPlaying())
            {
                audio.RadioSound.Stop();
                audio.radioActive = false;
            }   
            if (audio.radioActive)
                audio.RadioSound.SetVolume(audio.radioVolume);
            else
                audio.RadioSound.SetVolume(0.0f);
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

            if (input->IsKeyDown(KGR::Key::P))
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