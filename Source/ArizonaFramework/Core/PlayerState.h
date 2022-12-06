// Copyright (c) Wojciech Figat. All rights reserved.

#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Types.h"

/// <summary>
/// Player gameplay state container.
/// </summary>
API_CLASS() class ARIZONAFRAMEWORK_API PlayerState : public ScriptingObject
{
    DECLARE_SCRIPTING_TYPE(PlayerState);

public:
    /// <summary>
    /// Unique network client identifier.
    /// </summary>
    API_FIELD(NetworkReplicated, ReadOnly) uint32 NetworkClientId = MAX_uint32;
    
    /// <summary>
    /// Unique player identifier.
    /// </summary>
    API_FIELD(NetworkReplicated, ReadOnly) uint32 PlayerId = MAX_uint32;

    /// <summary>
    /// Player pawn script (attached to the player pawn actor).
    /// </summary>
    API_FIELD(NetworkReplicated, ReadOnly) PlayerPawn* PlayerPawn = nullptr;

    /// <summary>
    /// Player controller script (attached to the player controller actor). Created only on server and local player client.
    /// </summary>
    API_FIELD(NetworkReplicated, ReadOnly) PlayerController* PlayerController = nullptr;

    /// <summary>
    /// Player UI script (attached to the player UI actor). Created only on local player client. Not replicated.
    /// </summary>
    API_FIELD(ReadOnly) PlayerUI* PlayerUI = nullptr;
};
