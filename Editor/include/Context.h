#pragma once

#include "Scene/Scene.h"
#include "Core/CameraComponent.h"
#include "Core/TrasformComponent.h"
#include "KGR_ImGui.h"
#include "VulkanCore.h"
#include "../../Editor/include/HierarchyPanel.h"
#include "../../Editor/include/InspectorPanel.h"
#include "../../Editor/include/Toolbar.h"
#include "../../Editor/include/Viewport.h"
#include "../../Editor/include/Offscreen.h"
#include "../../Editor/include/Layer.h"

#include <memory>

namespace KGR
{
	namespace Editor
	{
		class Context
		{
		public:
			Context(KGR::_ImGui::ImGuiCore& imgui, KGR::_Vulkan::VulkanCore& vulkan);

			// Call after Vulkan and ImGui
			void Init(uint32_t viewportWidth, uint32_t viewportHeight);

			// Draw all editor panels. Must be called between ImGui BeginFrame() and EndFrame()
			void Render();

			// Must be called after Render() and befoire app.Render()
			void HandleResize();

			// Call before VulkanCore is destroyed
			void Destroy();

			Scene* GetActiveScene() const;
			SceneEntity GetSelectedEntity() const;
			CameraComponent& GetCamera();
			TransformComponent& GetCamTransform();
			Offscreen& GetOffscreen();
			
			glm::vec2 GetViewportPos() const;
			glm::vec2 GetViewportSize() const;

		private:

			void RegisterClone();
			void RegisterInspector();
			KGR::_ImGui::ImGuiCore& m_imGui;
			KGR::_Vulkan::VulkanCore& m_vulkan;

			Scene m_editorScene;

			std::unique_ptr<Toolbar> m_toolbar;
			std::unique_ptr<HierarchyPanel> m_hierarchy;
			std::unique_ptr<InspectorPanel> m_inspector;
			std::unique_ptr<Viewport> m_viewport;

			Offscreen m_offscreen;
			CameraComponent m_camera;
			TransformComponent m_cameraTransform;
		};
	}
}
