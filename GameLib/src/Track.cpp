#include "Track.h"
#include "Core/Mesh.h"
#include "Core/TrasformComponent.h"
#include "Core/Texture.h"
#include "Core/ManagerImple.h"
#include "Core/Window.h"

void Track::SpawnRoadPieces(ecsType& registry, KGR::RenderWindow& window, const TrackComponent& track)
{
    for (size_t i = 0; i < track.sampledPoints.size(); ++i)
    {
        auto e = registry.CreateEntity();

        MeshComponent mesh;
        auto* roadMesh = &MeshLoader::Load("Models\\CUBE.obj", window.App());
        mesh.mesh = roadMesh;

        TransformComponent transform;
        transform.SetScale({ track.trackWidth, 0.01f, track.step * 10.0f });
        transform.SetPosition(track.sampledPoints[i]);

        // RMF orientation
        transform.SetOrientation(glm::quatLookAt(track.frames[i].forward, track.frames[i].up));

        TextureComponent texture;
        texture.SetSize(mesh.mesh->GetSubMeshesCount());
        for (int j = 0; j < mesh.mesh->GetSubMeshesCount(); ++j)
            texture.AddTexture(j, &TextureLoader::Load("Textures\\BaseTexture.png", window.App()));

        registry.AddComponents<MeshComponent, TransformComponent, TextureComponent>(
            e, std::move(mesh), std::move(transform), std::move(texture)
        );
    }
}

void Track::CreateTrack(ecsType& registry, KGR::RenderWindow& window)
{
    std::vector<glm::vec3> points{
    { -5,0,5 }, { 10,0,-20 }, { 20,0,-10 }, { 35,0,-15 },
    { 50,0,-25 }, { 60,0,-20 }, { 70,0,-25 }, { 85,0,-35 },
    { 90,0,-50 }, { 70,0,-70 }, { 55,0,-60 }, { 40,0,-50 },
    { 25,0,-60 }, { 20,0,-75 }, { 10,0,-90 }, { 0,0,-100 },
    { -5,0,-105 }
    };

    KGR::ECS::Entity::_64 trackEntity;
    {
        trackEntity = registry.CreateEntity();
        TrackComponent track;

        track.curve = HermitCurve::FromPoints(points, 0);
        track.step = 0.1f;

        float maxT = track.curve.MaxT();
        int sampleCount = static_cast<int>(maxT / track.step) + 1;

        track.sampledPoints.reserve(sampleCount);
        for (int i = 0; i < sampleCount; ++i)
            track.sampledPoints.push_back(track.curve.Compute(i * track.step));

        track.forwardDirs = KGR::RMF::EstimateForwardDirs(track.sampledPoints);
        track.frames = KGR::RMF::BuildFrames(track.sampledPoints, track.forwardDirs);

        registry.AddComponents<TrackComponent>(trackEntity, std::move(track));
    }

    auto& track = registry.GetComponent<TrackComponent>(trackEntity);
    Track MyTrack;
    MyTrack.SpawnRoadPieces(registry, window, track);
}