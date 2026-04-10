#include "Track.h"
#include "Core/Mesh.h"
#include "Core/TrasformComponent.h"
#include "Core/Texture.h"
#include "Core/ManagerImple.h"
#include "Core/Window.h"
#include "Math/Collision.h"
#include "Math/CollisionComponent.h"

#include <iostream>

KGR::ECS::Entity::_64 Track::CreateStaticMesh(
    ecsType& registry,
    KGR::RenderWindow& window,
    const std::string& path)
{
    auto e = registry.CreateEntity();

    //Mesh
    MeshComponent m;
    m.mesh = &MeshLoader::Load(path, window.App());
    registry.AddComponent<MeshComponent>(e, std::move(m));

    //Transform
    TransformComponent tr;
    tr.SetPosition({ 0,0,0 });
    tr.SetScale({ 1,1,1 });
    tr.SetRotation({ 0,0,0 });
    registry.AddComponent<TransformComponent>(e, std::move(tr));

    MaterialComponent mat;
    mat.materials.resize(m.mesh->GetSubMeshesCount());
    registry.AddComponent<MaterialComponent>(e, std::move(mat));

    bool isSol = path.find("sol") != std::string::npos;
    bool isParking = path.find("place_de_parking") != std::string::npos;

    if (!isSol && !isParking)
    {
        CollisionComp col;
        col.collider = &ColliderManager::Load(path, m.mesh);

        registry.AddComponent<CollisionComp>(e, std::move(col));
    }

    return e;
}

KGR::ECS::Entity::_64 Track::CreateCollisionMesh(
    ecsType& registry,
    KGR::RenderWindow& window,
    const std::string& path)
{
    auto e = registry.CreateEntity();

    TransformComponent tr;
    tr.SetPosition({ 0,0,0 });
    tr.SetScale({ 1,1,1 });
    tr.SetRotation({ 0,0,0 });
    registry.AddComponent<TransformComponent>(e, std::move(tr));

    Mesh* mesh = &MeshLoader::Load(path, window.App());

    CollisionComp col;
    col.collider = &ColliderManager::Load(path, mesh);

    registry.AddComponent<CollisionComp>(e, std::move(col));
    return e;
}

void Track::CreateMap(ecsType& registry, KGR::RenderWindow& window)
{
    CreateStaticMesh(registry, window, "Models/Map/sol.obj");
    CreateStaticMesh(registry, window, "Models/Map/trottoir.obj");
    CreateStaticMesh(registry, window, "Models/Map/place_de_parking.obj");
    CreateStaticMesh(registry, window, "Models/Map/ronds_points.obj");
    CreateStaticMesh(registry, window, "Models/Map/barrieres.obj");
    CreateStaticMesh(registry, window, "Models/Map/fleches.obj");
	CreateStaticMesh(registry, window, "Models/Map/fils_poteaux.obj");
	CreateStaticMesh(registry, window, "Models/Map/barrels.obj");
	CreateStaticMesh(registry, window, "Models/Map/poteaux.obj");
	CreateStaticMesh(registry, window, "Models/Map/plots.obj");

    CreateCollisionMesh(registry, window, "Models/Map/collision.obj");
}