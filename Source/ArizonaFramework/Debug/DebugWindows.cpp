#include "DebugWindows.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Collections/ChunkedArray.h"
#include "Engine/Platform/Platform.h"
#include "Engine/Platform/CreateProcessSettings.h"
#include "Engine/Level/Level.h"
#include "Engine/Level/Scene/Scene.h"
#include "Engine/Utilities/StringConverter.h"
#include "Engine/Debug/DebugCommands.h"
#include "ImGui/imgui.h"

DebugGeneralToolsWindow::DebugGeneralToolsWindow(const SpawnParams& params)
    : DebugWindow(params)
{
    MenuName = "General/Tools";
}

void DebugGeneralToolsWindow::OnDraw()
{
    if (!ImGui::Begin("Tools", &_active))
        return;
    if (ImGui::Button("Open log"))
    {
        CreateProcessSettings procSettings;
        procSettings.FileName = Log::Logger::LogFilePath;
        procSettings.ShellExecute = true;
        Platform::CreateProcess(procSettings);
    }
    ImGui::End();
}

DebugGeneralConsoleWindow::DebugGeneralConsoleWindow(const SpawnParams& params)
    : DebugWindow(params)
{
    MenuName = "General/Console";
    strcpy(_inputBuffer, "");
    Log::Logger::OnMessage.Bind<DebugGeneralConsoleWindow, &DebugGeneralConsoleWindow::OnMessage>(this);
}

DebugGeneralConsoleWindow::~DebugGeneralConsoleWindow()
{
    Log::Logger::OnMessage.Unbind<DebugGeneralConsoleWindow, &DebugGeneralConsoleWindow::OnMessage>(this);
}

void DebugGeneralConsoleWindow::OnDraw()
{
    if (!ImGui::Begin("Console", &_active))
        return;
    ScopeLock lock(_locker);

    // Context menu
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Close"))
            _active = false;
        ImGui::EndPopup();
    }

    // Options
    if (ImGui::SmallButton("Clear"))
        _entries.Clear();
    ImGui::SameLine();
    if (ImGui::SmallButton("Scroll"))
        _scrollToBottom = true;

    ImGui::Separator();
    const float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerHeight), false, ImGuiWindowFlags_HorizontalScrollbar))
    {
        // Context menu
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::Selectable("Clear"))
                _entries.Clear();
            ImGui::Checkbox("Auto-scroll", &_autoScroll);
            ImGui::EndPopup();
        }

        // Tighten spacing
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

        // Display all log items
        for (const auto& e : _entries)
        {
            ImVec4 color;
            bool hasColor = false;
            if (e.Type == LogType::Error)
            { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); hasColor = true; }
            else if (e.Type == LogType::Warning)
            { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); hasColor = true; }
            else if (e.Message.StartsWith("> "))
            { color = ImVec4(0.8f, 0.8f, 0.8f, 1.0f); hasColor = true; }
            if (hasColor)
                ImGui::PushStyleColor(ImGuiCol_Text, color);

            ImGui::TextUnformatted(e.Message.Get());

            if (hasColor)
                ImGui::PopStyleColor();
        }

        // Auto-scroll
        if (_scrollToBottom || (_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
            ImGui::SetScrollHereY(1.0f);
        _scrollToBottom = false;

        ImGui::PopStyleVar();
    }
    ImGui::EndChild();
    ImGui::Separator();

    // Command-line
    bool getFocus = false;
    const ImGuiInputTextFlags commandFlags = 
        ImGuiInputTextFlags_EnterReturnsTrue |
        ImGuiInputTextFlags_EscapeClearsAll |
        ImGuiInputTextFlags_CallbackCompletion |
        ImGuiInputTextFlags_CallbackHistory;
    if (ImGui::InputText("Command", _inputBuffer, ARRAY_COUNT(_inputBuffer), commandFlags, &OnTextEditCallbackStub, (void*)this))
    {
        OnCommand(_inputBuffer);
        strcpy(_inputBuffer, "");
        getFocus = true;
    }

    // Auto-focus on window apparition
    ImGui::SetItemDefaultFocus();
    if (getFocus)
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

    ImGui::End();
}

void DebugGeneralConsoleWindow::OnCommand(const char* command)
{
    if (!command || !*command)
        return;

    // Insert command into the log and scroll down
    _entries.Add({ LogType::Info, StringAnsi::Format("> {}", command) });
    _scrollToBottom = true;

    // Add into history (remove if already added previously)
    _historyPos = -1;
    for (int32 i = _history.Count() - 1; i >= 0; i--)
    {
        if (StringUtils::CompareIgnoreCase(*_history[i], command) == 0)
        {
            _history.RemoveAtKeepOrder(i);
            break;
        }
    }
    _history.Add(command);

    // Process command
    if (StringUtils::CompareIgnoreCase(command, "clear") == 0)
    {
        _entries.Clear();
        return;
    }
    String commandStr(command);
    DebugCommands::Execute(commandStr);
}

void DebugGeneralConsoleWindow::AddLog(StringAnsi&& msg)
{
    ScopeLock lock(_locker);
    _entries.Add({ LogType::Info, MoveTemp(msg) });
}

void DebugGeneralConsoleWindow::OnMessage(LogType type, const StringView& msg)
{
    ScopeLock lock(_locker);
    _entries.Add({ type, StringAnsi(msg) });
}

int DebugGeneralConsoleWindow::OnTextEditCallbackStub(ImGuiInputTextCallbackData* data)
{
    return ((DebugGeneralConsoleWindow*)data->UserData)->OnTextEditCallback(data);
}

int DebugGeneralConsoleWindow::OnTextEditCallback(ImGuiInputTextCallbackData* data)
{
    switch (data->EventFlag)
    {
    case ImGuiInputTextFlags_CallbackCompletion:
    {
        const char* wordEnd = data->Buf + data->CursorPos;
        const char* wordStart = wordEnd;
        while (wordStart > data->Buf)
        {
            const char c = wordStart[-1];
            if (c == ' ' || c == '\t' || c == ',' || c == ';')
                break;
            wordStart--;
        }
        ImVector<const char*> candidates;
        const StringAnsiView word(wordStart, (int)(wordEnd - wordStart));
        if (StringAnsiView("clear").StartsWith(word, StringSearchCase::IgnoreCase))
            candidates.push_back("clear");
        int32 cmdIndex = 0;
        StringAsUTF16<> wordUTF16(word.Get(), word.Length());
        StringView wordUTF16View(wordUTF16.Get(), wordUTF16.Length());
        ChunkedArray<StringAnsi, 256> candidatesCache;
        while (DebugCommands::Iterate(wordUTF16View, cmdIndex))
        {
            StringAnsi& candidate = candidatesCache.AddOne();
            candidate = StringAnsi(DebugCommands::GetCommandName(cmdIndex));
            candidates.push_back(candidate.Get());
            cmdIndex++;
        }
        if (word.IsEmpty())
        {
            // Ignore
        }
        else if (candidates.Size == 0)
        {
            //AddLog(StringAnsi::Format("No match for \"{}\"", word));
        }
        else if (candidates.Size == 1)
        {
            data->DeleteChars((int)(wordStart - data->Buf), (int)(wordEnd - wordStart));
            data->InsertChars(data->CursorPos, candidates[0]);
        }
        else
        {
            int matchLen = (int)(wordEnd - wordStart);
            for (;;)
            {
                int c = 0;
                bool allCandidatesMatches = true;
                for (int i = 0; i < candidates.Size && allCandidatesMatches; i++)
                {
                    if (i == 0)
                        c = StringUtils::ToUpper(candidates[i][matchLen]);
                    else if (c == 0 || c != StringUtils::ToUpper(candidates[i][matchLen]))
                        allCandidatesMatches = false;
                }
                if (!allCandidatesMatches)
                    break;
                matchLen++;
            }
            if (matchLen > 0)
            {
                data->DeleteChars((int)(wordStart - data->Buf), (int)(wordEnd - wordStart));
                data->InsertChars(data->CursorPos, candidates[0], candidates[0] + matchLen);
            }
            AddLog("Possible matches:");
            for (int i = 0; i < candidates.Size; i++)
                AddLog(StringAnsi::Format("- {}", candidates[i]));
        }
        break;
    }
    case ImGuiInputTextFlags_CallbackHistory:
    {
        // Filter commands history
        const int prevHistoryPos = _historyPos;
        if (data->EventKey == ImGuiKey_UpArrow)
        {
            if (_historyPos == -1)
                _historyPos = _history.Count() - 1;
            else if (_historyPos > 0)
                _historyPos--;
        }
        else if (data->EventKey == ImGuiKey_DownArrow)
        {
            if (_historyPos != -1 && ++_historyPos >= _history.Count())
                _historyPos = -1;
        }
        if (prevHistoryPos != _historyPos)
        {
            const char* historyStr = _historyPos >= 0 ? _history[_historyPos].Get() : "";
            data->DeleteChars(0, data->BufTextLen);
            data->InsertChars(0, historyStr);
        }
        break;
    }
    }
    return 0;
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
    if (!ImGui::Begin("Scene Tree", &_active))
        return;
    for (auto a : Level::Scenes)
    {
        DrawActor(a, ImGuiTreeNodeFlags_DefaultOpen);
    }
    ImGui::End();
}
