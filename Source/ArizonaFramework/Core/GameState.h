// Copyright (c) Wojciech Figat. All rights reserved.

#pragma once

#include "Engine/Core/Collections/Array.h"
#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Scripting/ScriptingObjectReference.h"
#include "Types.h"

/// <summary>
/// Global gameplay state container.
/// </summary>
API_CLASS() class ARIZONAFRAMEWORK_API GameState : public ScriptingObject
{
    DECLARE_SCRIPTING_TYPE(GameState);

public:
    /// <summary>
    /// Global counter for player identifiers. Managed by game host when spawning players (including local coop).
    /// </summary>
    API_FIELD(NetworkReplicated, ReadOnly) uint32 NextPlayerId = 0;

    /// <summary>
    /// List with all connected players state.
    /// </summary>
    API_FIELD(NetworkReplicated, ReadOnly) Array<ScriptingObjectReference<PlayerState>> PlayerStates;

public:
    /// <summary>
    /// Gets the player state for a given unique NetworkClientId. In case of local coop, the first player is returned.
    /// </summary>
    API_FUNCTION() PlayerState* GetPlayerStateByNetworkClientId(uint32 networkClientId) const;

    /// <summary>
    /// Gets the player state for a given unique PlayerId.
    /// </summary>
    API_FUNCTION() PlayerState* GetPlayerStateByPlayerId(uint32 playerId) const;
};
