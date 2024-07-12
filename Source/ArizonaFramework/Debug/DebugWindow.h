#pragma once

#include "Engine/Scripting/ScriptingObject.h"

/// <summary>
/// Base class for debug windows used to inspect gameplay and provide additional development utilities inside the game.
/// </summary>
API_CLASS(Abstract, Namespace="ArizonaFramework.Debug") class ARIZONAFRAMEWORK_API DebugWindow : public ScriptingObject
{
    DECLARE_SCRIPTING_TYPE(DebugWindow);
    friend class DebugSystem;

private:
    bool _active = false;

public:
    // The name of the window in the menu bar. Can contain slashes for groupping. Eg. 'Tools/Profiler'.
    API_FIELD() StringAnsi MenuName;

public:
    // Called when window gets shown.
    API_FUNCTION() virtual void OnActivated() {}

    // Called when window gets hidden.
    API_FUNCTION() virtual void OnDeactivated() {}

    // Called when window is active and can draw it's contents.
    API_FUNCTION() virtual void OnDraw() {}
};
