// Copyright (c) Wojciech Figat. All rights reserved.

#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Types.h"

class Actor;
class NetworkClient;

/// <summary>
/// Main, root system of the game that implements the logic and flow of the gameplay.
/// </summary>
API_CLASS() class ARIZONAFRAMEWORK_API GameMode : public ScriptingObject
{
    DECLARE_SCRIPTING_TYPE(GameMode);

public:
    /// <summary>
    /// Starts the game (after creating Game State but before any players joining).
    /// </summary>
    API_FUNCTION() virtual void StartGame();

    /// <summary>
    /// Stops the game.
    /// </summary>
    API_FUNCTION() virtual void StopGame();

    /// <summary>
    /// Creates the Player Pawn actor for a given player which will be spawned over network on all connected clients.
    /// </summary>
    /// <param name="playerState">The player state.</param>
    /// <returns>The created player pawn actor (eg. from prefab).</returns>
    API_FUNCTION() virtual Actor* CreatePlayerPawn(PlayerState* playerState);

    /// <summary>
    /// Creates the Player Controller actor for a given player which will be spawned over network on connected client.
    /// </summary>
    /// <param name="playerState">The player state.</param>
    /// <returns>The created player controller actor (eg. from prefab).</returns>
    API_FUNCTION() virtual Actor* CreatePlayerController(PlayerState* playerState);

    /// <summary>
    /// Called when player joins the game.
    /// </summary>
    /// <param name="playerState">The player state.</param>
    API_FUNCTION() virtual void OnPlayerJoined(PlayerState* playerState);

    /// <summary>
    /// Called when player leaves the game (eg. disconnected). Despawns player pawn and controller.
    /// </summary>
    /// <param name="playerState">The player state.</param>
    API_FUNCTION() virtual void OnPlayerLeft(PlayerState* playerState);
};
