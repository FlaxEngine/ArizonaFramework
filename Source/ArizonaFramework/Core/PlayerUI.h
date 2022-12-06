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

private:
    API_PROPERTY(NetworkReplicated) void SetPlayerState(PlayerState* value);

public:
    // [Script]
    void OnDestroy() override;
};
