#include "UISystem.h"
#include "ArizonaFramework/Core/GameInstance.h"
#include "ImGui/ImGuiPlugin.h"
#include "Engine/Engine/Engine.h"
#include "Engine/Engine/Screen.h"
#define UI_DEBUG 0
#if UI_DEBUG
#include "Engine/Core/Log.h"
#include "Engine/Scripting/Enums.h"
#endif

UISystem::UISystem(const SpawnParams& params)
    : GameSystem(params)
{
}

InputContextType UISystem::GetInputContext() const
{
    return _inputContextStack.Peek();
}

void UISystem::PushInputContext(InputContextType type)
{
    auto prev = GetInputContext();
    _inputContextStack.Push(type);
    OnInputContextChanged(prev);
}

void UISystem::PopInputContext()
{
    auto prev = GetInputContext();
    _inputContextStack.Pop();
    OnInputContextChanged(prev);
}

void UISystem::OnInputContextChanged(InputContextType prev)
{
    auto curr = GetInputContext();

    switch (curr)
    {
    case InputContextType::Menu:
    case InputContextType::DebugImGui:
        // Ensure mouse is visible in menus
        Screen::SetCursorLock(CursorLockMode::None);
        Screen::SetCursorVisible(true);
        break;
    }

    // Update ImGui inputs reading
    if (auto* imGui = ImGuiPlugin::GetInstance())
    {
        if (prev == InputContextType::DebugImGui)
            imGui->EnableInput = false;
        else if (curr == InputContextType::DebugImGui)
            imGui->EnableInput = true;
    }

    // Auto-focus viewport when using menus (esp. for gamepad navigation to work)
    Engine::FocusGameViewport();

#if UI_DEBUG
    LOG(Info, "Push input context {} -> {}", ScriptingEnum::ToString(prev), ScriptingEnum::ToString(curr));
#endif
}

UISystem* UISystem::GetInstance()
{
    if (auto* instance = GameInstance::GetInstance())
        return instance->GetGameSystem<UISystem>();
    return nullptr;
}

void UISystem::Initialize()
{
    // Start with normal game as default
    _inputContextStack.Clear();
    _inputContextStack.Add(InputContextType::Gameplay);
    if (auto* imGui = ImGuiPlugin::GetInstance())
        imGui->EnableInput = false;
}

void UISystem::Deinitialize()
{
}
