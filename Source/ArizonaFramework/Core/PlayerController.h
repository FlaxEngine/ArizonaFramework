// Copyright (c) Wojciech Figat. All rights reserved.

#pragma once

#include "Engine/Scripting/Script.h"
#include "Types.h"

/// <summary>
/// Player inputs controller that receives input from the player and converts it into movement for pawn.
/// </summary>
API_CLASS() class ARIZONAFRAMEWORK_API PlayerController : public Script
{
    friend GameInstance;
    friend PlayerPawn;
    API_AUTO_SERIALIZATION();
    DECLARE_SCRIPTING_TYPE(PlayerController);

protected:
    PlayerState* _playerState = nullptr;
    bool _spawned = false;

public:
    /// <summary>
    /// Gets the player state for this controller.
    /// </summary>
    API_PROPERTY() FORCE_INLINE PlayerState* GetPlayerState() const
    {
        return _playerState;
    }

    /// <summary>
    /// Gets the player pawn for this controller.
    /// </summary>
    API_PROPERTY() PlayerPawn* GetPlayerPawn() const;

    /// <summary>
    /// Gets the unique PlayerId for this controller.
    /// </summary>
    API_PROPERTY() uint32 GetPlayerId() const;

public:
    /// <summary>
    /// Event called every update for local controller to fetch new inputs before any gameplay logic updates. Never called on server.
    /// </summary>
    API_FUNCTION() virtual void OnUpdateInput()
    {
    }

    /// <summary>
    /// Event called after controller is spawned on a level (locally or after replicated).
    /// </summary>
    API_FUNCTION() virtual void OnPlayerSpawned()
    {
    }

    /// <summary>
    /// Event called after receiving pawn movement from the client. Can be used to reject too big deltas that prevent players from cheating. Called on server-only.
    /// </summary>
    /// <param name="translation">The translation vector.</param>
    /// <param name="rotation">The rotation quaternion.</param>
    API_FUNCTION() virtual bool OnValidateMove(const Vector3& translation, const Quaternion& rotation)
    {
        return true;
    }

public:
    /// <summary>
    /// Creates the Player UI actor for a player which will be used by the local player.
    /// </summary>
    /// <param name="playerState">The player state.</param>
    /// <returns>The created player UI actor (eg. from prefab).</returns>
    API_FUNCTION() virtual Actor* CreatePlayerUI(PlayerState* playerState);

    /// <summary>
    /// Moves pawn (on both local client and server).
    /// </summary>
    /// <param name="translation">The translation vector.</param>
    /// <param name="rotation">The rotation quaternion.</param>
    API_FUNCTION() void MovePawn(const Vector3& translation, const Quaternion& rotation);

protected:
    // Performs local movement of the pawn actor.
    virtual void OnMovePawn(Actor* pawnActor, const Vector3& translation, const Quaternion& rotation);

private:
    API_FUNCTION(NetworkRpc=Server) void MovePawnServer(const Vector3& translation, const Quaternion& rotation);

public:
    // [Script]
    void OnUpdate() override;
    void OnDestroy() override;
};
