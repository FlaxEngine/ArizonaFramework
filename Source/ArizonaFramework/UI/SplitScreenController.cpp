// Copyright (c) Wojciech Figat. All rights reserved.

#include "SplitScreenController.h"
#include "ArizonaFramework/Core/PlayerPawn.h"
#include "ArizonaFramework/Core/PlayerState.h"
#include "ArizonaFramework/Core/PlayerUI.h"
#include "ArizonaFramework/Core/GameInstance.h"
#include "Engine/Core/Types/Span.h"
#include "Engine/Core/Math/Vector4.h"
#include "Engine/Level/Actors/Camera.h"
#include "Engine/Graphics/RenderTask.h"

SceneRenderTask* PlayerPawn::GetPlayerRenderTask() const
{
    SceneRenderTask* task = MainRenderTask::Instance;
    auto* instance = GameInstance::GetInstance();
    if (instance && instance->_splitScreen)
    {
        for (auto& e : instance->_splitScreen->_splitScreens)
        {
            if (e.Player == _playerState)
            {
                task = e.RenderTask;
                break;
            }
        }
    }
    return task;
}

SplitScreenController::SplitScreenController(const SpawnParams& params)
    : ScriptingObject(params)
{
}

void SplitScreenController::PreUpdate(Span<PlayerState*> localPlayers)
{
    // Remove rendering tasks for players that are no longer present
    for (int32 i = _splitScreens.Count() - 1; i >= 0; i--)
    {
        if (SpanContains(localPlayers, _splitScreens[i].Player))
        {
            if (_splitScreens[i].RenderTask != MainRenderTask::Instance)
                Delete(_splitScreens[i].RenderTask);
            _splitScreens.RemoveAtKeepOrder(i);
        }
    }
    _mainUsed = false;
}

void SplitScreenController::Tick()
{
    for (auto& e : _splitScreens)
    {
        if (e.RenderTask != MainRenderTask::Instance)
        {
            // Sync with main render task output
            e.RenderTask->SwapChain = MainRenderTask::Instance->SwapChain;
            e.RenderTask->Output = MainRenderTask::Instance->Output;
        }
    }
}

SplitScreenController::~SplitScreenController()
{
    for (auto& e : _splitScreens)
    {
        if (e.RenderTask != MainRenderTask::Instance)
            Delete(e.RenderTask);
    }
}

void SplitScreenController::Update(Span<PlayerState*> localPlayers)
{
    for (int32 i = 0; i < localPlayers.Length(); i++)
    {
        Float4 viewportRect(0, 0, 1, 1);

        // Divide screen into equal parts based on the number of local players
        if (localPlayers.Length() == 1)
        {
            // Fullscreen
        }
        else if (localPlayers.Length() == 2)
        {
            // Vertical split
            if (i == 0)
                viewportRect = Float4(0, 0, 0.5f, 1);
            else
                viewportRect = Float4(0.5f, 0, 0.5f, 1);
        }
        else if (localPlayers.Length() == 3)
        {
            // 3 players: 2 on left, 1 on right
            if (i == 0)
                viewportRect = Float4(0, 0, 0.5f, 0.5f);
            else if (i == 1)
                viewportRect = Float4(0, 0.5f, 0.5f, 0.5f);
            else
                viewportRect = Float4(0.5f, 0, 0.5f, 1);
        }
        else if (localPlayers.Length() == 4)
        {
            // 4 players: 2x2 grid
            if (i == 0)
                viewportRect = Float4(0, 0, 0.5f, 0.5f);
            else if (i == 1)
                viewportRect = Float4(0.5f, 0, 0.5f, 0.5f);
            else if (i == 2)
                viewportRect = Float4(0, 0.5f, 0.5f, 0.5f);
            else
                viewportRect = Float4(0.5f, 0.5f, 0.5f, 0.5f);
        }
        else
        {
            // TODO: support more players (via procedural grid layout)
            float split = 1.0f / localPlayers.Length();
            viewportRect = Float4(i * split, 0, split, 1);
        }

        // Apply the viewport rect to the player's camera and HUD
        PlayerState* localPlayer = localPlayers[i];
        SetPlayerViewport(localPlayer, viewportRect);
    }

    // TODO: when players are nearby each other (depends on game), custom SceneRenderTasks could link their buffers into main render task to reuse shadow maps, DDGI and other rendering data
}

SceneRenderTask* SplitScreenController::SetPlayerViewport(PlayerState* localPlayer, const Float4& viewportRect)
{
    // Find existing task for the player
    SceneRenderTask* task = nullptr;
    for (auto& e : _splitScreens)
    {
        if (e.Player == localPlayer)
        {
            task = e.RenderTask;
            break;
        }
    }
    if (!task)
    {
        if (_mainUsed)
        {
            // Allocate a new task
            task = New<SceneRenderTask>();

            // Draw after the main and in stable order
            task->Order = 10 + localPlayer->PlayerId;

            // Draw to the main output
            task->SwapChain = MainRenderTask::Instance->SwapChain;
            task->Output = MainRenderTask::Instance->Output;
            task->IsComposite = true;
        }
        else
        {
            // Use main task
            task = MainRenderTask::Instance;
            _mainUsed = true;
        }

        _splitScreens.Add({ localPlayer, task });
    }

    // Setup camera
    if (auto* camera = localPlayer->PlayerPawn->GetPlayerCamera())
    {
        camera->ViewportRect = viewportRect;
        // TODO: what about cut-scenes and dynamic camera changes? should we expose more camera-related logic to gameplay? what about cut-scenes in local coop?
        task->Camera = camera;
    }

    // Setup UI
    if (localPlayer->PlayerUI)
    {
        localPlayer->PlayerUI->SetViewport(viewportRect);
    }

    return task;
}
