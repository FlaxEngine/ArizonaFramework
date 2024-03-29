﻿// Copyright (c) 2012-2022 Wojciech Figat. All rights reserved.

#pragma once

#include "Engine/Core/Config/Settings.h"
#include "Engine/Content/SoftAssetReference.h"
#include "Engine/Scripting/SoftTypeReference.h"
#include "Engine/Level/Prefabs/Prefab.h"
#include "../Networking/ReplicationSettings.h"

class NetworkReplicationHierarchy;

/// <summary>
/// The settings for Game Instance.
/// </summary>
API_CLASS(NoConstructor) class ARIZONAFRAMEWORK_API GameInstanceSettings : public SettingsBase
{
    API_AUTO_SERIALIZATION();
    DECLARE_SCRIPTING_TYPE_MINIMAL(GameInstanceSettings);
    DECLARE_SETTINGS_GETTER(GameInstanceSettings);

public:
    /// <summary>
    /// The type of the Game Mode that will be spawned on game host/server. Used to control the game logic and match flow.
    /// </summary>
    API_FIELD(Attributes="EditorOrder(100), EditorDisplay(\"Types\"), TypeReference(typeof(GameMode))")
    SoftTypeReference<GameMode> GameModeType = "ArizonaFramework.GameMode";

    /// <summary>
    /// The type of the Game State that will be spawned on host and clients (server-authoritative). Holds the global game state (eg. teams score).
    /// </summary>
    API_FIELD(Attributes="EditorOrder(110), EditorDisplay(\"Types\"), TypeReference(typeof(GameState))")
    SoftTypeReference<GameState> GameStateType = "ArizonaFramework.GameState";

    /// <summary>
    /// The type of the Player State that will be spawned on host and clients (server-authoritative). Holds the per-player state (eg. health and inventory).
    /// </summary>
    API_FIELD(Attributes="EditorOrder(120), EditorDisplay(\"Types\"), TypeReference(typeof(PlayerState))")
    SoftTypeReference<PlayerState> PlayerStateType = "ArizonaFramework.PlayerState";

    /// <summary>
    /// The Player Pawn prefab asset to spawn by Game Mode for each joining player.
    /// </summary>
    API_FIELD(Attributes="EditorOrder(150), EditorDisplay(\"Types\")")
    SoftAssetReference<Prefab> PlayerPawnPrefab;

    /// <summary>
    /// The Player Controller prefab asset to spawn by Game Mode for each joining player (on server and player's client only).
    /// </summary>
    API_FIELD(Attributes="EditorOrder(160), EditorDisplay(\"Types\")")
    SoftAssetReference<Prefab> PlayerControllerPrefab;

    /// <summary>
    /// The Player UI prefab asset to spawn for local players.
    /// </summary>
    API_FIELD(Attributes="EditorOrder(160), EditorDisplay(\"Types\")")
    SoftAssetReference<Prefab> PlayerUIPrefab;

public:
    /// <summary>
    /// Type of the network replication hierarchy system to use.
    /// </summary>
    API_FIELD(Attributes="EditorOrder(1000), EditorDisplay(\"Replication\"), TypeReference(typeof(FlaxEngine.Networking.NetworkReplicationHierarchy))")
    SoftTypeReference<NetworkReplicationHierarchy> ReplicationHierarchy = "ArizonaFramework.ReplicationHierarchy";

    /// <summary>
    /// Default replication settings. Used when not overriden by specific object type.
    /// </summary>
    API_FIELD(Attributes="EditorOrder(1010), EditorDisplay(\"Replication\")")
    ReplicationSettings DefaultReplicationSettings;

    /// <summary>
    /// Per-type replication settings. Runtime lookup includes base classes (but not interfaces).
    /// </summary>
    API_FIELD(Attributes="EditorOrder(1050), EditorDisplay(\"Replication\")")
    Dictionary<SoftTypeReference<>, ReplicationSettings> ReplicationSettingsPerType;
};
