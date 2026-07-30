// Copyright (c) Wojciech Figat. All rights reserved.

#pragma once

#include "Engine/Scripting/Script.h"
#include "Types.h"

/// <summary>
/// Player User Interface with HUD.
/// </summary>
API_CLASS() class ARIZONAFRAMEWORK_API PlayerUI : public Script
{
    friend GameInstance;
    API_AUTO_SERIALIZATION();
    DECLARE_SCRIPTING_TYPE(PlayerUI);

protected:
    PlayerState* _playerState = nullptr;

public:
    /// <summary>
    /// Gets the player state for this UI.
    /// </summary>
    API_PROPERTY(NetworkReplicated) FORCE_INLINE PlayerState* GetPlayerState() const
    {
        return _playerState;
    }

public:
    /// <summary>
    /// Event called after player is spawned on a level (locally).
    /// </summary>
    API_FUNCTION() virtual void OnPlayerSpawned()
    {
    }

    /// <summary>
    /// Sets player viewport for HUD placement within screen. Used to arrange split-screen viewports and UIs. Called when local players are spawned or despawned.
    /// </summary>
    /// <param name="viewportRect">Desire viewport placement for the player view rendering (see Camera.ViewportRect to learn more).</param>
    API_FUNCTION() virtual void SetViewport(const Float4& viewportRect);

private:
    API_PROPERTY(NetworkReplicated) void SetPlayerState(PlayerState* value);

public:
    // [Script]
    void OnDestroy() override;
};
