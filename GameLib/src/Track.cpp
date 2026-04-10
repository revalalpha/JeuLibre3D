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
    const std::string& path, const std::string& texturePath)
{
    auto e = registry.CreateEntity();

    //Mesh
    MeshComponent m;
    m.mesh = &MeshLoader::Load(path, window.App());
    registry.AddComponent<MeshComponent>(e, std::move(m));

    //Transform
    TransformComponent tr;
    tr.SetPosition({ 0,0,0 });
    tr.SetScale({ 1 / 2.0f,1 / 2.0f,1 / 2.0f });
    tr.SetRotation({ 0,0,0 });
    registry.AddComponent<TransformComponent>(e, std::move(tr));

    MaterialComponent mat;
    mat.materials.resize(m.mesh->GetSubMeshesCount());

    for (int i = 0; i < m.mesh->GetSubMeshesCount(); ++i)
    {
        Material material;
        material.baseColor = &TextureLoader::Load(texturePath, window.App());
        mat.materials[i] = material;
    }

    registry.AddComponent<MaterialComponent>(e, std::move(mat));

    bool isSol = path.find("Models/Map/sol.obj") != std::string::npos;
    bool isParking = path.find("Models/Map/place_de_parking.obj") != std::string::npos;
	bool isRondPoint = path.find("round_about.obj") != std::string::npos;
	bool isFleche = path.find("fleches.obj") != std::string::npos;
	bool isFilsPoteaux = path.find("fils_poteaux.obj") != std::string::npos;
	bool isPoteaux = path.find("poteaux.obj") != std::string::npos;

    if (!isSol && !isParking && !isRondPoint && !isFleche && !isFilsPoteaux && !isPoteaux)
    {
        CollisionComp col;
        col.collider = &ColliderManager::Load(path, m.mesh);

        registry.AddComponent<CollisionComp>(e, std::move(col));
    }

    return e;
}

void Track::CreateMap(ecsType& registry, KGR::RenderWindow& window)
{
    CreateStaticMesh(registry, window, "Models/Map/sol.obj", "Textures/Map/texture_sol.jpg");
    CreateStaticMesh(registry, window, "Models/Map/trottoir.obj", "Textures/Map/trottoir_texture.jpg");
    CreateStaticMesh(registry, window, "Models/Map/place_de_parking.obj", "Textures/Map/parkings.png");
    CreateStaticMesh(registry, window, "Models/Map/round_about.obj", "Textures/Map/rond_point_texture.png");
    CreateStaticMesh(registry, window, "Models/Map/fleches.obj", "Textures/Map/fleche_1_texture.png");
	CreateStaticMesh(registry, window, "Models/Map/fils_poteaux.obj", "Textures/Map/trim_sheet.png");
	CreateStaticMesh(registry, window, "Models/Map/poteaux.obj", "Textures/Map/trim_sheet.png");

    /*ALL BARRELS*/
    for (size_t i = 1; i <= 63; ++i)
    {
        if(i == 24 || i == 25)
			continue;
        if (i >= 46 && i <= 50)
			continue;
        if (i >= 53 && i <= 55)
            continue;

        CreateStaticMesh(registry, window, std::format("Models/Map/Barrels/Cylinder_{:03d}.obj", i), "Textures/Map/trim_sheet.png");
	}

    //ALL BARRERS
    for (size_t i = 1; i <= 146; ++i)
    {
        if (i == 2 || i == 4 || i == 12 || i == 45)
            continue;
        if (i >= 52 && i <= 54)
            continue;
        if (i >= 94 && i <= 95)
            continue;

        CreateStaticMesh(registry, window, std::format("Models/Map/Barrers/barriere55_{:03d}.obj", i), "Textures/Map/trim_sheet.png");
    }
}