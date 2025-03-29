#pragma once

#include "DebugWindow.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Collections/Array.h"

// General utilities window (log file opening, etc.).
API_CLASS(Namespace="ArizonaFramework.Debug") class ARIZONAFRAMEWORK_API DebugGeneralToolsWindow : public DebugWindow
{
    DECLARE_SCRIPTING_TYPE(DebugGeneralToolsWindow);
    void OnDraw() override;
};

#if FLAX_1_10_OR_NEWER

// Console output and command line access.
API_CLASS(Namespace="ArizonaFramework.Debug") class ARIZONAFRAMEWORK_API DebugGeneralConsoleWindow : public DebugWindow
{
    DECLARE_SCRIPTING_TYPE(DebugGeneralConsoleWindow);
    ~DebugGeneralConsoleWindow();
    void OnDraw() override;
    void OnActivated() override;
private:
    struct Entry
    {
        LogType Type;
        StringAnsi Message;
    };
    CriticalSection _locker;
    Array<Entry> _entries;
    bool _autoScroll = true;
    bool _scrollToBottom = false;
    int32 _historyPos = -1;
    Array<StringAnsi> _history;
    char _inputBuffer[512];

    void AddLog(StringAnsi&& msg);
    void OnCommand(const char* command);
    void OnMessage(LogType type, const StringView& msg);
    static int OnTextEditCallbackStub(struct ImGuiInputTextCallbackData* data);
    int OnTextEditCallback(ImGuiInputTextCallbackData* data);
};

#endif

// Scene hierarchy debugging window.
API_CLASS(Namespace="ArizonaFramework.Debug") class ARIZONAFRAMEWORK_API DebugSceneTreeWindow : public DebugWindow
{
    DECLARE_SCRIPTING_TYPE(DebugSceneTreeWindow);
    void OnDraw() override;
};
