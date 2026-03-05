#include <filesystem>
#include <iostream>
#include <cmath>
#include "Core/CameraComponent.h"
#include "VulkanCore.h"
#include "_GLFW.h"
#include "Core/ManagerImple.h"
#include "Core/Mesh.h"
#include "Core/TrasformComponent.h"
#include "Core/LightComponent.h"
#include "Core/Spline.h"
#include "Core/Frenet.h"
#include "Core/Texture.h"
#include "Core/Window.h"
#include "ECS/Registry.h"
#include "ECS/Entities.h"
#include <glm/gtc/constants.hpp>

// to move
struct ControllerComponent {};

// ---------------------------------------------------------------------------
// Génère la grille aplatie de vertices du tube.
// Index d'un sommet : i * tubeSegments + j
//   i = position le long de la courbe  (0 .. sampleCount-1)
//   j = position autour de l'anneau    (0 .. tubeSegments-1)
//
// La normale est la direction radiale pure (cos*right + sin*up).
// Les UVs : u tourne autour de l'anneau, v avance le long de la courbe.
// ---------------------------------------------------------------------------
static std::vector<Vertex> BuildTubeVertices(
	const std::vector<glm::vec3>& curvePoints,
	const std::vector<KGR::CurveFrame>& curveFrames,
	float radius,
	int   tubeSegments)
{
	const int sampleCount = static_cast<int>(curvePoints.size());
	std::vector<Vertex> vertices;
	vertices.reserve(sampleCount * tubeSegments);

	for (int i = 0; i < sampleCount; ++i)
	{
		for (int j = 0; j < tubeSegments; ++j)
		{
			float angle = (static_cast<float>(j) / static_cast<float>(tubeSegments))
				* glm::two_pi<float>();

			glm::vec3 radialDir = curveFrames[i].right * std::cos(angle)
				+ curveFrames[i].up * std::sin(angle);

			Vertex v;
			v.pos = curvePoints[i] + radialDir * radius;
			v.normal = radialDir;
			v.uv = { static_cast<float>(j) / static_cast<float>(tubeSegments),
						 static_cast<float>(i) / static_cast<float>(sampleCount - 1) };
			v.color = glm::vec4(1.0f);

			vertices.push_back(v);
		}
	}
	return vertices;
}

// ---------------------------------------------------------------------------
// Génère les indices (triangles) pour une grille torique sampleCount x tubeSegments.
// Chaque quad est découpé en 2 triangles. Les modulos referment les deux axes
// pour former un tore fermé (le dernier anneau se reboucle sur le premier).
// ---------------------------------------------------------------------------
static std::vector<uint32_t> BuildTubeIndices(int sampleCount, int tubeSegments)
{
	std::vector<uint32_t> indices;
	indices.reserve(sampleCount * tubeSegments * 6);

	auto idx = [&](int i, int j) -> uint32_t
		{
			return static_cast<uint32_t>((i % sampleCount) * tubeSegments + (j % tubeSegments));
		};

	for (int i = 0; i < sampleCount; ++i)
	{
		for (int j = 0; j < tubeSegments; ++j)
		{
			// Les quatre coins du quad courant :
			//   v00 --- v01
			//    |       |
			//   v10 --- v11
			uint32_t v00 = idx(i, j);
			uint32_t v10 = idx(i + 1, j);
			uint32_t v01 = idx(i, j + 1);
			uint32_t v11 = idx(i + 1, j + 1);

			indices.push_back(v00);
			indices.push_back(v10);
			indices.push_back(v11);

			indices.push_back(v00);
			indices.push_back(v11);
			indices.push_back(v01);
		}
	}
	return indices;
}

int main(int argc, char** argv)
{
	std::filesystem::path exePath = argv[0];
	std::filesystem::path projectRoot = exePath.parent_path().parent_path().parent_path().parent_path().parent_path();
	KGR::RenderWindow::Init();
	KGR::RenderWindow window{ {1000,1000},"test",projectRoot / "Ressources" };
	window.GetInputManager()->SetMode(GLFW_CURSOR_DISABLED);
	using ecsType = KGR::ECS::Registry<KGR::ECS::Entity::_64, 100>;
	auto registry = ecsType{};

	// Cam
	{
		auto cam = registry.CreateEntity();
		CameraComponent camComp = CameraComponent::Create(45.0f, static_cast<float>(window.GetSize().x), static_cast<float>(window.GetSize().y), 0.01f, 1000.0f, CameraComponent::Type::Perspective);
		TransformComponent transform;
		transform.SetPosition({ 0,3,5 });
		registry.AddComponents<CameraComponent, TransformComponent, ControllerComponent>(cam, std::move(camComp), std::move(transform), std::move(ControllerComponent{}));
	}

	// Mesh animé le long de la courbe
	{
		auto mesh = registry.CreateEntity();
		MeshComponent meshComp;
		meshComp.mesh = &MeshLoader::Load("Models\\briet_claire_decorsfantasy_grpB.obj", window.App());
		TransformComponent transform;
		TextureComponent texture;
		texture.SetSize(meshComp.mesh->GetSubMeshesCount());
		for (int i = 0; i < meshComp.mesh->GetSubMeshesCount(); ++i)
			texture.AddTexture(i, &TextureLoader::Load("Textures\\BaseTexture.png", window.App()));
		registry.AddComponents<MeshComponent, TransformComponent, TextureComponent, ControllerComponent>(mesh, std::move(meshComp), std::move(transform), std::move(texture), std::move(ControllerComponent{}));
	}

	auto colorTransform = [](const glm::vec3& color)
		{
			return glm::vec3{ color.x / 255.0f, color.y / 255.0f, color.z / 255.0f };
		};

	{
		auto light = registry.CreateEntity();
		auto lComp = LightComponent<LightData::Type::Directional>::Create(colorTransform({ 154,36,69 }), { 1,1,1 }, 1.0f);
		TransformComponent lTransform;
		lTransform.LookAt({ 0,-1,0 });
		registry.AddComponents<LightComponent<LightData::Type::Directional>, TransformComponent>(light, std::move(lComp), std::move(lTransform));
	}

	// Courbe de Hermite
	std::vector<glm::vec3> points
	{
		{ -3.0f, 0.0f,  4.0f},
		{  0.0f, 0.0f,  0.0f},
		{  3.0f, 0.0f,  4.0f},
		{  6.0f, 0.0f,  0.0f},
		{  3.0f, 0.0f, -4.0f},
		{  0.0f, 0.0f,  0.0f},
		{ -3.0f, 0.0f,  4.0f},
		{ -6.0f, 0.0f,  0.0f},
		{ -3.0f, 0.0f, -4.0f},
		{  0.0f, 0.0f,  0.0f},
		{  3.0f, 0.0f,  4.0f},
		{  6.0f, 0.0f,  0.0f},
		{  3.0f, 0.0f, -4.0f},
		{  0.0f, 0.0f,  0.0f},
		{ -3.0f, 0.0f, -4.0f},
		{ -6.0f, 0.0f,  0.0f},
		{ -3.0f, 0.0f,  4.0f},
		{  0.0f, 0.0f,  0.0f},
		{  3.0f, 0.0f,  4.0f},
	};

	HermitCurve curve = HermitCurve::FromPoints(points, 0);

	const int   sampleCount = 500;
	const float maxT = curve.MaxT();

	std::vector<glm::vec3> sampledPositions;
	sampledPositions.reserve(sampleCount);
	for (int i = 0; i < sampleCount; ++i)
	{
		float t = static_cast<float>(i) / static_cast<float>(sampleCount - 1) * maxT;
		sampledPositions.push_back(curve.Compute(t));
	}

	auto tangents = KGR::RMF::EstimateForwardDirs(sampledPositions);
	auto curveFrames = KGR::RMF::BuildFrames(sampledPositions, tangents);

	// ---------------------------------------------------------------------------
	// tubeMesh est déclaré dans le scope de main pour survivre toute la boucle.
	// MeshComponent ne stocke qu'un pointeur brut, donc si tubeMesh était dans un
	// bloc intérieur il serait détruit avant la fin du rendu.
	// ---------------------------------------------------------------------------
	Mesh tubeMesh;
	{
		const int   tubeSegments = 16;
		const float tubeRadius = 0.18f;

		auto tubeVertices = BuildTubeVertices(sampledPositions, curveFrames, tubeRadius, tubeSegments);
		auto tubeIndices = BuildTubeIndices(sampleCount, tubeSegments);

		tubeMesh.AddSubMesh(std::make_unique<SubMeshes>(tubeVertices, tubeIndices, "tube", window.App()));
	}

	// Entité ECS pour le tube.
	// Pas de ControllerComponent : il ne bouge pas et ne doit pas être
	// sélectionné par la query de mise à jour du mesh animé.
	{
		auto tubeEntity = registry.CreateEntity();

		MeshComponent meshComp;
		meshComp.mesh = &tubeMesh;   // pointe vers le Mesh déclaré dans le scope de main

		TransformComponent transform;  // identité : les vertices sont déjà en world space

		TextureComponent texture;
		texture.SetSize(1);
		texture.AddTexture(0, &TextureLoader::Load("Textures\\BaseTexture.png", window.App()));

		registry.AddComponents<MeshComponent, TransformComponent, TextureComponent>(
			tubeEntity, std::move(meshComp), std::move(transform), std::move(texture));
	}

	static float curveT = 0.0f;
	do
	{
		KGR::RenderWindow::PollEvent();
		window.Update();

		static auto lastTime = std::chrono::high_resolution_clock::now();
		static float angle = 0.0f;
		const float rotationSpeed = 1.0f;

		auto currentTime = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
		lastTime = currentTime;

		angle += deltaTime * rotationSpeed;

		// Caméra FPS
		{
			auto es = registry.GetAllComponentsView<ControllerComponent, TransformComponent, CameraComponent>();
			auto* inputData = window.GetInputManager();
			for (auto& e : es)
			{
				auto& transform = registry.GetComponent<TransformComponent>(e);
				glm::vec3 dir = { 0.0f, 0.0f, 0.0f };

				if (inputData->IsKeyDown(KGR::Key::Z))       
					dir.z = -1;
				if (inputData->IsKeyDown(KGR::Key::S))   
					dir.z = 1;
				if (inputData->IsKeyDown(KGR::Key::Q))  
					dir.x = -1;
				if (inputData->IsKeyDown(KGR::Key::D))  
					dir.x = 1;
				if (inputData->IsKeyDown(KGR::SpecialKey::Space)) 
					dir.y = 1;
				if (inputData->IsKeyDown(KGR::SpecialKey::Shift)) 
					dir.y = -1;

				auto delta = inputData->GetMouseDelta();
				transform.RotateEuler<RotData::Orientation::Pitch>(-glm::radians(delta.y * deltaTime * 100));
				transform.RotateEuler<RotData::Orientation::Yaw>(-glm::radians(delta.x * deltaTime * 100));
				transform.Translate(dir * deltaTime);
			}
		}

		// Mesh animé le long de la courbe
		// La query inclut ControllerComponent pour ne cibler QUE ce mesh,
		// pas le tube qui n'a pas ce composant.
		{
			auto es = registry.GetAllComponentsView<ControllerComponent, TransformComponent, MeshComponent, TransformComponent>();
			for (auto& e : es)
			{
				auto& transform = registry.GetComponent<TransformComponent>(e);

				transform.SetPosition(curve.Compute(curveT));

				float frameIndex = (curveT / maxT) * static_cast<float>(sampleCount - 1);
				int   lowerIdx = glm::clamp(static_cast<int>(frameIndex), 0, sampleCount - 2);
				float t = frameIndex - static_cast<float>(lowerIdx);

				KGR::CurveFrame frame = KGR::RMF::InterpolateFrame(curveFrames[lowerIdx], curveFrames[lowerIdx + 1], t);
				transform.SetOrientation(glm::quatLookAt(frame.forward, frame.up));
			}
		}

		curveT += 0.001f;
		if (curveT > maxT)
			curveT = 0.0f;

		// Update caméra
		{
			auto es = registry.GetAllComponentsView<CameraComponent, TransformComponent>();
			if (es.Size() != 1)
				throw std::runtime_error("need one and one cam");
			for (auto& e : es)
			{
				registry.GetComponent<CameraComponent>(e).UpdateCamera(registry.GetComponent<TransformComponent>(e).GetFullTransform());
				window.RegisterCam(registry.GetComponent<CameraComponent>(e), registry.GetComponent<TransformComponent>(e));
			}
		}

		// Render — toutes les entités MeshComponent + TransformComponent + TextureComponent,
		// y compris le tube qui est maintenant une entité ECS comme les autres.
		{
			auto es = registry.GetAllComponentsView<MeshComponent, TransformComponent, TextureComponent>();
			for (auto& e : es)
				window.RegisterRender(
					registry.GetComponent<MeshComponent>(e),
					registry.GetComponent<TransformComponent>(e),
					registry.GetComponent<TextureComponent>(e));
		}

		{
			auto es = registry.GetAllComponentsView<LightComponent<LightData::Type::Point>, TransformComponent>();
			for (auto& e : es)
				window.RegisterLight(registry.GetComponent<LightComponent<LightData::Type::Point>>(e), registry.GetComponent<TransformComponent>(e));
		}
		{
			auto es = registry.GetAllComponentsView<LightComponent<LightData::Type::Spot>, TransformComponent>();
			for (auto& e : es)
				window.RegisterLight(registry.GetComponent<LightComponent<LightData::Type::Spot>>(e), registry.GetComponent<TransformComponent>(e));
		}
		{
			auto es = registry.GetAllComponentsView<LightComponent<LightData::Type::Directional>, TransformComponent>();
			for (auto& e : es)
				window.RegisterLight(registry.GetComponent<LightComponent<LightData::Type::Directional>>(e), registry.GetComponent<TransformComponent>(e));
		}

		window.Render({ 0.53f, 0.81f, 0.92f, 1.0f });

	} while (!window.ShouldClose());

	window.Destroy();
	KGR::RenderWindow::End();
}