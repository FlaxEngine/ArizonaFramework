#include "DebugSystem.h"
#include "DebugWindow.h"
#include "DebugSettings.h"
#include "ArizonaFramework/UI/UISystem.h"
#include "Engine/Core/Config/GameSettings.h"
#include "Engine/Core/Collections/Sorting.h"
#include "Engine/Engine/Engine.h"
#include "Engine/Input/Input.h"
#include "Engine/Content/Content.h"
#include "Engine/Content/JsonAsset.h"
#include "Engine/Profiler/ProfilerCPU.h"
#include "ImGui/imgui.h"

IMPLEMENT_GAME_SETTINGS_GETTER(DebugSettings, "Debug");

bool SortDebugWindows(DebugWindow* const& a, DebugWindow* const& b)
{
    return a->MenuName < b->MenuName;
}

DebugWindow::DebugWindow(const SpawnParams& params)
    : ScriptingObject(params)
{
    MenuName = params.Type.GetType().GetName();
}

DebugSystem::DebugSystem(const SpawnParams& params)
    : GameSystem(params)
{
}

void DebugSystem::Initialize()
{
    Engine::Update.Bind<DebugSystem, &DebugSystem::OnUpdate>(this);
    _menuActive = false;
}

void DebugSystem::Deinitialize()
{
    Engine::Update.Unbind<DebugSystem, &DebugSystem::OnUpdate>(this);
    _windows.ClearDelete();
}

void DebugSystem::OnUpdate()
{
    PROFILE_CPU();
    const auto& debugSettings = *DebugSettings::Get();

    // Toggle menu visibility via input action
    if (Input::GetAction(debugSettings.DebugMenuOpen))
    {
        _menuActive = !_menuActive;
        if (auto* ui = UISystem::GetInstance())
        {
            if (_menuActive)
                ui->PushInputContext(InputContextType::DebugImGui);
            else
                ui->PopInputContext();
        }
    }

    if (_menuActive)
    {
        // Init windows
        if (_windows.Count() != debugSettings.DebugWindows.Count())
        {
            _windows.ClearDelete();
            for (const auto& e : debugSettings.DebugWindows)
            {
                if (auto* window = e.NewObject())
                {
                    _windows.Add(window);
                }
            }
            Sorting::QuickSort(_windows.Get(), _windows.Count(), &SortDebugWindows);
        }

        // Draw debug menu
        if (ImGui::BeginMainMenuBar())
        {
            StringAnsi currentMenu;
            bool currentMenuOpen = false;
            for (auto window : _windows)
            {
                StringAnsi menu;
                int32 menuSize = window->MenuName.Find('/');
                if (menuSize != -1)
                    menu = window->MenuName.Left(menuSize);
                if (menu != currentMenu)
                {
                    if (currentMenuOpen && currentMenu.HasChars())
                        ImGui::EndMenu();
                    currentMenuOpen = ImGui::BeginMenu(*menu);
                    currentMenu = menu;
                }
                if (currentMenuOpen)
                {
                    StringAnsi item = window->MenuName.Substring(menuSize + 1);
                    const bool wasActive = window->_active;
                    if (ImGui::MenuItem(*item, nullptr, &window->_active))
                    {
                    }
                    if (wasActive && !window->_active)
                        window->OnDeactivated();
                    else if (window->_active && !wasActive)
                        window->OnActivated();
                }
            }
            if (currentMenuOpen && currentMenu.HasChars())
                ImGui::EndMenu();
            ImGui::EndMainMenuBar();
        }

        // Draw active windows
        for (auto window : _windows)
        {
            if (window->_active)
            {
                window->OnDraw();
                if (!window->_active)
                    window->OnDeactivated();
            }
        }
    }
}
