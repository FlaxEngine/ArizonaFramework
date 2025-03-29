#pragma once

#include "Engine/Core/Config/Settings.h"
#include "Engine/Core/Collections/Array.h"
#include "Engine/Scripting/SoftTypeReference.h"

class DebugWindow;

/// <summary>
/// The settings for debug tools in game.
/// </summary>
API_CLASS(NoConstructor, Namespace="ArizonaFramework.Debug") class ARIZONAFRAMEWORK_API DebugSettings : public SettingsBase
{
    API_AUTO_SERIALIZATION();
    DECLARE_SCRIPTING_TYPE_MINIMAL(DebugSettings);
    DECLARE_SETTINGS_GETTER(DebugSettings);
public:
    /// <summary>
    /// Input action that opens debug window (defined in Input Settings).
    /// </summary>
    API_FIELD(Attributes="EditorOrder(50), EditorDisplay(\"ImGui\"), CustomEditorAlias(\"FlaxEditor.CustomEditors.Editors.InputEventEditor\")")
    String DebugMenuOpen;

    /// <summary>
    /// Input action that opens and focuses or hides console window (defined in Input Settings).
    /// </summary>
    API_FIELD(Attributes="EditorOrder(51), EditorDisplay(\"ImGui\"), CustomEditorAlias(\"FlaxEditor.CustomEditors.Editors.InputEventEditor\")")
    String DebugConsoleOpen;

    /// <summary>
    /// List of debug windows to display in debug menu.
    /// </summary>
    API_FIELD(Attributes="EditorOrder(100), EditorDisplay(\"ImGui\"), TypeReference(typeof(ArizonaFramework.Debug.DebugWindow))")
    Array<SoftTypeReference<DebugWindow>> DebugWindows;
};
