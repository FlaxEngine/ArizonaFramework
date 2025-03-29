#pragma once

#include "ArizonaFramework/Core/GameSystem.h"
#include "Engine/Core/Collections/Array.h"

/// <summary>
/// Gameplay debugging system.
/// </summary>
API_CLASS(Namespace="ArizonaFramework.Debug") class ARIZONAFRAMEWORK_API DebugSystem : public GameSystem
{
    DECLARE_SCRIPTING_TYPE(DebugSystem);

private:
    bool _menuActive = false;
    Array<class DebugWindow*> _windows;

public:
    void SetActive(bool active);

public:
    // [GameSystem]
    void Initialize() override;
    void Deinitialize() override;

private:
    void OnUpdate();
};
