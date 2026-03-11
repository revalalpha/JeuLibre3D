#pragma once

#include "imgui.h"

namespace KGR
{
    namespace Editor
    {
        // Handles the one-time ImGui style setup and the per-frame docking layout.
        // Separating this from KGR_ImGui respects SRP: KGR_ImGui owns the Vulkan/GLFW
        // integration, EditorLayout owns the visual organisation of the editor.
        class Layer
        {
        public:
            // Call once, right after ImGui::CreateContext() and before the first frame.
            static void ApplyStyle();

            // Call at the very beginning of each frame, before any panel Render().
            // It creates an invisible fullscreen window that acts as the docking host,
            // then builds the default dock layout the first time it runs.
            static void BeginDockspace();

            // Call after all panels have been rendered.
            static void EndDockspace();

        private:
            static void BuildDefaultLayout(ImGuiID dockspaceId);

            static bool s_layoutBuilt;
        };
    }
}