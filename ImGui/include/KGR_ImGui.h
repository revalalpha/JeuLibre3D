#pragma once

#include "VulkanCore.h"
#include "_GLFW.h"
#include "Backends/imgui_impl_vulkan.h"
#include "Core/ManagerImple.h"
#include <ImGuizmo.h>

struct ImGuiContext;

namespace KGR
{
	namespace _ImGui
	{
		enum class ContextTarget
		{
			Engine,
			Game
		};

		enum class ButtonType
		{
			Object,
			Light,
			Camera,
			Scene,
			Load
		};

		class ImGuiCore
		{
		public:

			void InitImGui(KGR::_Vulkan::VulkanCore* vulkanCore, KGR::_GLFW::Window* engineWindow);
			void InitContext(ImGuiContext*& context, KGR::_Vulkan::VulkanCore* vulkanCore, KGR::_GLFW::Window* window);

			void CreateObject();
			bool IsButton(ButtonType type);
			void AddObject();
			void LoadObject();
			bool LoadMesh(MeshComponent& meshComponent, std::string& path, _Vulkan::VulkanCore& vkCore);

			void BeginFrame(ContextTarget target);
			void EndFrame(ContextTarget target, VkCommandBuffer commandBuffer);
			void Render(ContextTarget target);
			
			void SetContext(ContextTarget target);

			void Destroy();

			static void SetWindow(const ImVec2& position, const ImVec2& size, const char* name);
			static std::string OpenFile();

		private:

			void InitInfo();

			template<typename ReturnType, typename WrapperType>
			ReturnType Get(WrapperType& type)
			{
				return static_cast<ReturnType>(*type.Get());
			}

			KGR::_Vulkan::VulkanCore* m_VulkanCore = nullptr;
			ImGuiContext* m_EngineContext		   = nullptr;
			ImGuiContext* m_GameContext			   = nullptr;
			ImGui_ImplVulkan_InitInfo m_InitInfo   = {};

			char		m_ObjFilePath[512] = "";
			std::string m_LoadedObjName    = "";
			bool		m_LoadSuccess	   = false;
			bool		m_LoadError		   = false;
		};
	}
}
