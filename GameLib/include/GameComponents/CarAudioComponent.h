#include "Audio\SoundComponent.h"

enum struct CarState
{
    Idle,
    Accelerating,
    MaxAccelerating,
    Decelerating,
    Drifting,
    Braking
};

struct CarAudioComponent
{
    CarState state = CarState::Idle;

    std::unique_ptr<SoLoud::Wav> idleWav;
    std::unique_ptr<SoLoud::Wav> accelWav;
    std::unique_ptr<SoLoud::Wav> maxAccelWav;
    std::unique_ptr<SoLoud::Wav> decelWav;

    std::unique_ptr<SoLoud::Wav> brakingWav;

    std::unique_ptr<SoLoud::Wav> turboWav;

    KGR::Audio::WavComponent idleSound;
    KGR::Audio::WavComponent accelSound;
    KGR::Audio::WavComponent maxAccelSound;
    KGR::Audio::WavComponent decelSound;

    KGR::Audio::WavComponent brakingSound;

    KGR::Audio::WavComponent turboSound;
};