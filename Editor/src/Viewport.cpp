#include "Viewport.h"
#include "Backends/imgui_impl_vulkan.h"
#include <glm/gtc/type_ptr.hpp>

namespace KGR
{
    namespace Editor
    {
        Viewport::Viewport(KGR::_ImGui::ImGuiCore& imgui, KGR::_Vulkan::VulkanCore& vulkanCore)
            : m_ImGui(imgui), m_VulkanCore(vulkanCore)
        {
        }

        void Viewport::Render(SceneEntity selectedEntity, Scene* scene, CameraComponent* cam)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::Begin("Viewport");

            m_IsFocused = ImGui::IsWindowFocused();
            m_IsHovered = ImGui::IsWindowHovered();

            ImVec2 contentSize = ImGui::GetContentRegionAvail();
            glm::vec2 newSize = { contentSize.x, contentSize.y };

            // Detect resize so the caller can recreate the Offscreen
            // We only flag it if the size actually changed AND is non-zero
            // (ImGui returns 0 for a collapsed/docked window).
            m_WasResized = (newSize != m_ViewportSizePrev && newSize.x > 1.0f && newSize.y > 1.0f);
            if (m_WasResized)
            {
                m_ViewportSize = newSize;
                m_ViewportSizePrev = newSize;
            }

            ImVec2 cursorPos = ImGui::GetCursorScreenPos();
            m_ViewportPos = { cursorPos.x, cursorPos.y };

            // Gizmo mode shortcuts — only active when the viewport has keyboard focus.
            if (m_IsFocused)
            {
                if (ImGui::IsKeyPressed(ImGuiKey_W)) m_GizmoMode = GizmoMode::Translate;
                if (ImGui::IsKeyPressed(ImGuiKey_R)) m_GizmoMode = GizmoMode::Rotate;
                if (ImGui::IsKeyPressed(ImGuiKey_C)) m_GizmoMode = GizmoMode::Scale;
            }

            if (m_sceneDescriptor != VK_NULL_HANDLE)
            {
                // The offscreen target is in SHADER_READ_ONLY_OPTIMAL at this point
                // because VulkanCore::Render() transitioned it before ImGui rendering.
                // ImGui::Image() simply emits a textured quad into the draw list.
                ImGui::Image(reinterpret_cast<ImTextureID>(m_sceneDescriptor),
                    ImVec2(m_ViewportSize.x, m_ViewportSize.y));
            }
            else
            {
                // Fallback while the offscreen target is not yet set up.
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                drawList->AddRectFilled(
                    cursorPos,
                    ImVec2(cursorPos.x + contentSize.x, cursorPos.y + contentSize.y),
                    IM_COL32(30, 30, 30, 255));
                drawList->AddText(
                    ImVec2(cursorPos.x + 10, cursorPos.y + 10),
                    IM_COL32(180, 180, 180, 255),
                    "Waiting for offscreen target...");
            }

            // Mode indicator overlay — drawn on top of the scene image.
            {
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                const char* modeText = "W: Translate | E: Rotate | R: Scale";
                drawList->AddText(ImVec2(cursorPos.x + 10, cursorPos.y + 10),
                    IM_COL32(180, 180, 180, 200), modeText);

                const char* currentMode =
                    m_GizmoMode == GizmoMode::Translate ? "[Translate]" :
                    m_GizmoMode == GizmoMode::Rotate ? "[Rotate]" : "[Scale]";
                drawList->AddText(ImVec2(cursorPos.x + 10, cursorPos.y + 28),
                    IM_COL32(255, 200, 50, 255), currentMode);
            }

            DrawGizmo(selectedEntity, scene, cam);

            ImGui::End();
            ImGui::PopStyleVar();
        }

        void Viewport::DrawGizmo(SceneEntity e, Scene* scene, CameraComponent* cam)
        {
            if (!scene || !cam || e == NullEntity)
                return;

            SceneRegistry& reg = scene->GetRegistry();
            if (!reg.HasComponent<TransformComponent>(e))
                return;

            auto& transform = reg.GetComponent<TransformComponent>(e);

            glm::mat4 view = cam->GetView();
            glm::mat4 proj = cam->GetProj();
            glm::mat4 model = transform.GetFullTransform();

            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(m_ViewportPos.x, m_ViewportPos.y, m_ViewportSize.x, m_ViewportSize.y);

            ImGuizmo::OPERATION op = ImGuizmo::TRANSLATE;
            switch (m_GizmoMode)
            {
            case GizmoMode::Translate: op = ImGuizmo::TRANSLATE; break;
            case GizmoMode::Rotate:    op = ImGuizmo::ROTATE;    break;
            case GizmoMode::Scale:     op = ImGuizmo::SCALE;     break;
            }

            float matrixFloat[16];
            memcpy(matrixFloat, glm::value_ptr(model), sizeof(float) * 16);

            if (ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(proj),
                op, ImGuizmo::LOCAL, matrixFloat))
            {
                float translation[3], rotation[3], scale[3];
                ImGuizmo::DecomposeMatrixToComponents(matrixFloat, translation, rotation, scale);

                if (op == ImGuizmo::TRANSLATE)
                    transform.SetPosition({ translation[0], translation[1], translation[2] });

                if (op == ImGuizmo::ROTATE)
                    transform.SetRotation(glm::radians(glm::vec3(rotation[0], rotation[1], rotation[2])));

                if (op == ImGuizmo::SCALE)
                    transform.SetScale(glm::vec3({ scale[0], scale[1], scale[2] }) * 2.0f);
            }
        }
    }
}
