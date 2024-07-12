#include "DebugWindows.h"
#include "Engine/Core/Log.h"
#include "Engine/Platform/Platform.h"
#include "Engine/Platform/CreateProcessSettings.h"
#include "Engine/Level/Level.h"
#include "Engine/Level/Scene/Scene.h"
#include "Engine/Utilities/StringConverter.h"
#include "ImGui/imgui.h"

DebugGeneralToolsWindow::DebugGeneralToolsWindow(const SpawnParams& params)
    : DebugWindow(params)
{
    MenuName = "General/Tools";
}

void DebugGeneralToolsWindow::OnDraw()
{
    ImGui::Begin("Tools");
    if (ImGui::Button("Open log"))
    {
        CreateProcessSettings procSettings;
        procSettings.FileName = Log::Logger::LogFilePath;
        procSettings.ShellExecute = true;
        Platform::CreateProcess(procSettings);
    }
    ImGui::End();
}

DebugSceneTreeWindow::DebugSceneTreeWindow(const SpawnParams& params)
    : DebugWindow(params)
{
    MenuName = "Scene/Tree";
}

void DrawActor(Actor* a, ImGuiTreeNodeFlags flags = 0)
{
    const StringAsANSI<> name(a->GetName().Get(), a->GetName().Length());
    if (a->Children.HasItems())
    {
        if (ImGui::TreeNodeEx(name.Get(), flags))
        {
            for (auto child : a->Children)
                DrawActor(child);
            ImGui::TreePop();
        }
    }
    else
    {
        ImGui::Indent();
        ImGui::Text("%s", name.Get() && a->GetName().HasChars() ? name.Get() : "<empty>");
        ImGui::Unindent();
    }
}

void DebugSceneTreeWindow::OnDraw()
{
    ImGui::Begin("Scene Tree");
    for (auto a : Level::Scenes)
    {
        DrawActor(a, ImGuiTreeNodeFlags_DefaultOpen);
    }
    ImGui::End();
}
