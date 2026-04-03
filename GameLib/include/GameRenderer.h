#pragma once
#include "ECS/Registry.h"
#include "ECS/Entities.h"
#include "Core/Window.h"
#include "DebugRenderer.h"
#include "Pipeline.h"

using ecsType = KGR::ECS::Registry<KGR::ECS::Entity::_64, 100>;

struct GameRenderer
{
	/*GameRenderer(KGR::RenderWindow& window);

	KGR::_Vulkan::DebugRenderer& GetDebug() { return m_debug; }
	vk::raii::CommandBuffer& GetCmd() { return m_cmd; }
	KGR::_Vulkan::Pipeline& GetDebugPipeline() { return m_debugPipeline; }*/

	void RenderMesh(ecsType& registry, KGR::RenderWindow& window);
	/*void RenderUi(ecsType& registry, KGR::RenderWindow& window);*/
	void RenderLight(ecsType& registry, KGR::RenderWindow& window);
	void RenderCam(ecsType& registry, KGR::RenderWindow& window);
private:
	/*KGR::_Vulkan::DebugRenderer m_debug;
	KGR::_Vulkan::Pipeline m_debugPipeline;
	vk::raii::CommandBuffer m_cmd;*/
};