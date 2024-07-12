#pragma once

#include "DebugWindow.h"

// General utilities window (log file opening, etc.).
API_CLASS(Namespace="ArizonaFramework.Debug") class ARIZONAFRAMEWORK_API DebugGeneralToolsWindow : public DebugWindow
{
    DECLARE_SCRIPTING_TYPE(DebugGeneralToolsWindow);
    void OnDraw() override;
};

// Scene hierarchy debugging window.
API_CLASS(Namespace="ArizonaFramework.Debug") class ARIZONAFRAMEWORK_API DebugSceneTreeWindow : public DebugWindow
{
    DECLARE_SCRIPTING_TYPE(DebugSceneTreeWindow);
    void OnDraw() override;
};
