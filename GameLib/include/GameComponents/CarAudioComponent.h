#include "Audio\SoundComponent.h"

struct RPMLayer
{
    std::shared_ptr<SoLoud::Wav> wav;
    KGR::Audio::WavComponent sound;

    float rpmMin;       // RPM où ce layer commence à entrer
    float rpmMax;       // RPM où ce layer est au volume max
    float rpmFade;      // RPM où ce layer disparaît complètement
    float smoothPitch = 1.0f;
};


struct CarAudioComponent
{
    //Layers Moteur
    std::vector<RPMLayer> engineLayers;

    /// Wav
    std::unique_ptr<SoLoud::Wav> engineWav;
    std::unique_ptr<SoLoud::Wav> driftWav;
    std::unique_ptr<SoLoud::Wav> turboWav;
    std::vector<std::unique_ptr<SoLoud::Wav>> backfireWavs;
    std::unique_ptr<SoLoud::Wav> brakingWav;
    std::unique_ptr<SoLoud::Wav> RadioWav;


    /// Sound
    KGR::Audio::WavComponent engineSound;
    KGR::Audio::WavComponent driftSound;
    KGR::Audio::WavComponent turboSound;
    std::vector<KGR::Audio::WavComponent> backfireSounds;
    KGR::Audio::WavComponent brakingSound;
    KGR::Audio::WavComponent RadioSound;

    //Engine
    float currentRPM = 800.0f;
    float targetRPM = 800.0f;
    float targetSpeedRatio = 0.0f;
    float minRPM = 800.0f;
    float maxRPM = 8000.0f;
    float rpmSmoothSpeed = 5.0f;
    float lastPitch = 1.0f;

    // Radio state
    bool  radioActive = false;
    float radioVolume = 0.5f;
    float radioVolumeMin = 0.0f;
    float radioVolumeMax = 1.0f;
    float radioVolumeStep = 0.5f;

    float backFireTimer = 0.0f;
    int pendinBackFireIndex = 0;
};