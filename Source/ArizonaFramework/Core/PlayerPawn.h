// Copyright (c) Wojciech Figat. All rights reserved.

#pragma once

#include "Engine/Scripting/Script.h"
#include "Types.h"

/// <summary>
/// Player script on a scene (attached to player prefab root actor) that represents it in the game (on level).
/// </summary>
API_CLASS() class ARIZONAFRAMEWORK_API PlayerPawn : public Script
{
    friend GameInstance;
    API_AUTO_SERIALIZATION();
    DECLARE_SCRIPTING_TYPE(PlayerPawn);

protected:
    PlayerState* _playerState = nullptr;
    uint32 _playerId = MAX_uint32;
    bool _spawned = false;

public:
    /// <summary>
    /// Gets the player state for this pawn.
    /// </summary>
    API_PROPERTY(NetworkReplicated) FORCE_INLINE PlayerState* GetPlayerState() const
    {
        return _playerState;
    }

    /// <summary>
    /// Gets the unique player identifier for this pawn.
    /// </summary>
    API_PROPERTY(NetworkReplicated) FORCE_INLINE uint32 GetPlayerId() const
    {
        return _playerId;
    }

public:
    /// <summary>
    /// Event called after player is spawned on a level (locally or after replicated).
    /// </summary>
    API_FUNCTION() virtual void OnPlayerSpawned()
    {
    }

private:
    API_PROPERTY(NetworkReplicated) void SetPlayerState(PlayerState* value);
    API_PROPERTY(NetworkReplicated) void SetPlayerId(uint32 value);

public:
    // [Script]
    void OnStart() override;
    void OnDestroy() override;
};
