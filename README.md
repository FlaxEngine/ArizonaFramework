# Arizona Framework

Ready to use, open-source framework for creating games in Flax.

## Features

* Core game architecture concepts
  * Game Mode and Game State
  * Player State, Player Pawn and Player UI
* Multiplayer-ready (server-client infrastructure with optional local co op)
* Extendable (eg. via `GameSystem` or `GameSceneSystem`)
* Debug UI ([ImGui](https://github.com/FlaxEngine/ImGui))

Minimum supported Flax version: `1.5`.

## Installation

1. Clone repo into `<game-project>\Plugins\ArizonaFramework`

2. Add reference to Arizona Framework project in your game by modyfying your game `<game-project>.flaxproj` as follows:

```
...
"References": [
    {
        "Name": "$(EnginePath)/Flax.flaxproj"
    },
    {
        "Name": "$(ProjectPath)/Plugins/ArizonaFramework/ArizonaFramework.flaxproj"
    }
]
```

3. Add reference to *ArizonaFramework* module in your game build script (eg. `Game.Build.cs`) as follows:

```cs
/// <inheritdoc />
public override void Setup(BuildOptions options)
{
    base.Setup(options);

    BuildNativeCode = false;
    options.ScriptingAPI.IgnoreMissingDocumentationWarnings = true;

    // Add reference to ArizonaFramework
    options.PrivateDependencies.Add("ArizonaFramework");
}
```

4. Create new `Game Instance Settings` (linked to Game Settings by Editor).

5. Customize it

Now you can use *Arizona Framework* in your project. Use created `Game Instance Settings` asset to define your game mode, player pawn and other types to control the game data and logic.

## License

Both this plugin and ImGui are released under **MIT License**.

## Core Components

### Game Instance

Main game singleton plugin that manages the game systems and handles Game Mode setup and lifetime for the play.

### Game System

Gameplay system component attached to the Game Instance. Lifetime tied with the game.

### Game Scene System

Scene gameplay component attached to the Game Instance. Lifetime tied with the scene (created for each loaded scene).

### Game Mode

Main, root system of the game that implements the logic and flow of the gameplay. Exists only on server. Handles clients joining and spawning them on the level with local-client authority. Controls the limit of the players on a map, team sizes, allowed weapons and characters. Controls bots and spectators, but also level changes. Persists between scene changes.

### Game State

Global gameplay state container, maintained by server and replicated into all clients. Allows clients to access a list of players or gameplay conditions state (eg. current match score, elapsed game time).

### Player State

Player gameplay state container, maintained by server and replicated into all clients. Allows clients to access each player information (eg. name, score, ping, team, isSpectator, isBot).

### Player Pawn

Player actor on a scene (eg. prefab) that represents it in the game. Replicated to all clients (depending on the relevance) and owned by the server (simulated based on controller inputs). Locally simulated pawn can use autonomous replication mode to allow using locally playing human inputs for smooth gameplay but will still be validated against server state to prevent cheating.

### Player Controller

Player inputs controller that receives input from the player and converts it into movement for pawn. Exists on owning-client and is replicated on server-only.

### Player UI

Player User Interface with HUD. Exists only on local clients and is using Player State with Game State to inform players about the gameplay.

## Architecture

* Game Instance (GamePlugin)
  * Game System
  * Game Scene System
  * Game Mode
    * Player Pawn (script attached to spawned actor)
    * Player Controller (script attached to spawned actor)
    * Player UI (script attached to spawned actor)
  * Game State
    * Player State
