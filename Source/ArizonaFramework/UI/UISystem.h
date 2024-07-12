#pragma once

#include "ArizonaFramework/Core/GameSystem.h"
#include "Engine/Core/Collections/Array.h"

/// <summary>
/// Different types of input contexts that can exist in game and affect user interactions.
/// </summary>
API_ENUM(Namespace="ArizonaFramework.UI") enum class InputContextType
{
    /// <summary>
    /// Player Controller uses inputs to control player or other gameplay components.
    /// </summary>
    Gameplay,

    /// <summary>
    /// Pause menu, main menu or other menu in use.
    /// </summary>
    Menu,

    /// <summary>
    /// Debug ImGui tool in use.
    /// </summary>
    DebugImGui,
};

/// <summary>
/// User Interface manager (local UI).
/// </summary>
API_CLASS(Namespace="ArizonaFramework.UI") class ARIZONAFRAMEWORK_API UISystem : public GameSystem
{
    DECLARE_SCRIPTING_TYPE(UISystem);

private:
    Array<InputContextType> _inputContextStack;

public:
    // Gets the current input context from the stack for proper input handling.
    API_PROPERTY() InputContextType GetInputContext() const;

    // Pushes the new UI input context onto the stack.
    API_FUNCTION() void PushInputContext(InputContextType type);

    // Pops the latest input context type from the input stack.
    API_FUNCTION() void PopInputContext();

public:
    /// <summary>
    /// Gets the User Interface system instance.
    /// </summary>
    API_PROPERTY() static UISystem* GetInstance();

    // [GameSystem]
    void Initialize() override;
    void Deinitialize() override;

private:
    void OnInputContextChanged(InputContextType prev);
};
