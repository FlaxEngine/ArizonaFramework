// Copyright (c) 2012-2022 Wojciech Figat. All rights reserved.

#pragma once

#include "Engine/Core/Config/Settings.h"
#include "Engine/Content/SoftAssetReference.h"
#include "Engine/Level/Prefabs/Prefab.h"

/// <summary>
/// The settings for Game Instance.
/// </summary>
API_CLASS() class ARIZONAFRAMEWORK_API GameInstanceSettings : public SettingsBase
{
    API_AUTO_SERIALIZATION();
    DECLARE_SCRIPTING_TYPE_MINIMAL(GameInstanceSettings);
    DECLARE_SETTINGS_GETTER(GameInstanceSettings);

public:
    /// <summary>
    /// The type of the Game Mode that will be spawned on game host/server. Used to control the game logic and match flow.
    /// </summary>
    API_FIELD(Attributes="EditorOrder(100), EditorDisplay(\"Types\"), TypeReference(typeof(GameMode)), CustomEditorAlias(\"FlaxEditor.CustomEditors.Editors.TypeNameEditor\")")
    StringAnsi GameModeType = "ArizonaFramework.GameMode";

    /// <summary>
    /// The type of the Game State that will be spawned on host and clients (server-authoritative). Holds the global game state (eg. teams score).
    /// </summary>
    API_FIELD(Attributes="EditorOrder(110), EditorDisplay(\"Types\"), TypeReference(typeof(GameState)), CustomEditorAlias(\"FlaxEditor.CustomEditors.Editors.TypeNameEditor\")")
    StringAnsi GameStateType = "ArizonaFramework.GameState";

    /// <summary>
    /// The type of the Player State that will be spawned on host and clients (server-authoritative). Holds the per-player state (eg. health and inventory).
    /// </summary>
    API_FIELD(Attributes="EditorOrder(120), EditorDisplay(\"Types\"), TypeReference(typeof(PlayerState)), CustomEditorAlias(\"FlaxEditor.CustomEditors.Editors.TypeNameEditor\")")
    StringAnsi PlayerStateType = "ArizonaFramework.PlayerState";

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
};
