#include "GameRenderer.h"
#include "ECS/Registry.h"
#include "Core/Window.h"
#include "Core/CameraComponent.h"

void GameRenderer::RenderMesh(ecsType& registry, KGR::RenderWindow& window)
{
	auto es = registry.GetAllComponentsView<MeshComponent, TransformComponent, TextureComponent>();
	for (auto& e : es)
		window.RegisterRender(
			registry.GetComponent<MeshComponent>(e),
			registry.GetComponent<TransformComponent>(e),
			registry.GetComponent<TextureComponent>(e));
}

void GameRenderer::RenderLight(ecsType& registry, KGR::RenderWindow& window)
{
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
}

void GameRenderer::RenderCam(ecsType& registry, KGR::RenderWindow& window)
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