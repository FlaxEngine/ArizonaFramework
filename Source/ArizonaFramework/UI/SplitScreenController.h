// Copyright (c) Wojciech Figat. All rights reserved.

#pragma once

#include "ArizonaFramework/Core/Types.h"
#include "Engine/Core/Collections/Array.h"
#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Scripting/ScriptingObjectReference.h"

/// <summary>
/// Player script on a scene (attached to player prefab root actor) that represents it in the game (on level).
/// </summary>
API_CLASS() class ARIZONAFRAMEWORK_API SplitScreenController : public ScriptingObject
{
    friend GameInstance;
    friend PlayerPawn;
    DECLARE_SCRIPTING_TYPE(SplitScreenController);
private:
    struct SplitScreen
    {
        ScriptingObjectReference<PlayerState> Player;
        class SceneRenderTask* RenderTask = nullptr;
    };

    Array<SplitScreen, InlinedAllocation<8>> _splitScreens;
    bool _mainUsed = false;

    void PreUpdate(Span<PlayerState*> localPlayers);
    void Tick();

public:
    ~SplitScreenController();

    /// <summary>
    /// Updates the layout of the split-screen viewports and player UIs. Called when local players are spawned or despawned.
    /// </summary>
    /// <param name="localPlayers">List of local players.</param>
    API_FUNCTION() virtual void Update(Span<PlayerState*> localPlayers);

    /// <summary>
    /// Updates the layout of the split-screen viewports and player UIs. Called when local players are spawned or despawned.
    /// </summary>
    /// <param name="localPlayer">The local player to set its viewport.</param>
    /// <param name="viewportRect">Desire viewport placement for the player view rendering (see Camera.ViewportRect to learn more).</param>
    /// <returns>Scene Render Task object that will draw this player view. It's main for the first player or allocated for secondary players (who draw to the main view output as a composite).</returns>
    API_FUNCTION() SceneRenderTask* SetPlayerViewport(PlayerState* localPlayer, const Float4& viewportRect);
};
