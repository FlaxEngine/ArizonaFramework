#include "DebugSystem.h"
#include "DebugWindow.h"
#include "DebugSettings.h"
#include "DebugWindows.h"
#include "ArizonaFramework/UI/UISystem.h"
#include "Engine/Core/Config/GameSettings.h"
#include "Engine/Core/Collections/Sorting.h"
#include "Engine/Engine/Engine.h"
#include "Engine/Input/Input.h"
#include "Engine/Content/Content.h"
#include "Engine/Content/JsonAsset.h"
#include "Engine/Core/Collections/ArrayExtensions.h"
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

void DebugSystem::SetActive(bool active)
{
    if (_menuActive == active)
        return;
    _menuActive = active;
    if (auto* ui = UISystem::GetInstance())
    {
        if (_menuActive)
            ui->PushInputContext(InputContextType::DebugImGui);
        else
            ui->PopInputContext();
    }
}

void DebugSystem::Initialize()
{
    Scripting::Update.Bind<DebugSystem, &DebugSystem::OnUpdate>(this);
    _menuActive = false;
}

void DebugSystem::Deinitialize()
{
    Scripting::Update.Unbind<DebugSystem, &DebugSystem::OnUpdate>(this);
    _windows.ClearDelete();
}

void DebugSystem::OnUpdate()
{
    PROFILE_CPU();
    const auto& debugSettings = *DebugSettings::Get();

    // Toggle menu visibility via input action
    if (Input::GetAction(debugSettings.DebugMenuOpen))
    {
        SetActive(!_menuActive);
    }

    // Toggle console for quick debug commands access
    bool openConsole = false, closeConsole = false;
    if (Input::GetAction(debugSettings.DebugConsoleOpen))
    {
        if (!_menuActive)
        {
            SetActive(true);
            openConsole = true;
        }
        else
        {
            closeConsole = true;
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

        // Console toggle
        if (openConsole || closeConsole)
        {
            const Function<bool(const DebugWindow*)> findConsole = [](const DebugWindow* window) -> bool { return window->GetTypeHandle() == DebugGeneralConsoleWindow::TypeInitializer; };
            if (auto* console = ArrayExtensions::First(_windows, findConsole))
            {
                if (openConsole)
                {
                    if (!console->_active)
                    {
                        // Open console
                        console->_active = true;
                        console->OnActivated();

                        // TODO: dock console in bottom of the game viewport

                        // Skip input processing this frame as user pressed/released DebugConsoleOpen action
                        return;
                    }
                }
                else
                {
                    if (console->_active)
                    {
                        // Close console
                        console->_active = false;
                        console->OnDeactivated();
                        const Function<bool(const DebugWindow*)> allInactive = [](const DebugWindow* window) -> bool { return !window->_active; };
                        if (ArrayExtensions::All(_windows, allInactive))
                        {
                            // Hide menu if none other window is in use (eg. user used console button again)
                            SetActive(false);

                            // Skip further processing once hidden
                            return;
                        }
                    }
                }
            }
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
