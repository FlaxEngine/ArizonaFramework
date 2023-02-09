// Copyright (c) Wojciech Figat. All rights reserved.

#pragma once

#include "Engine/Scripting/Plugins/GamePlugin.h"
#include "Types.h"

class Scene;
class Actor;
class NetworkClient;

/// <summary>
/// Main game singleton plugin that manages the game systems and handles Game Mode setup and lifetime for the play.
/// </summary>
API_CLASS() class ARIZONAFRAMEWORK_API GameInstance : public GamePlugin
{
    friend PlayerPawn;
    friend PlayerController;
    DECLARE_SCRIPTING_TYPE(GameInstance);

private:
    Array<GameSystem*> _systems;
    Array<ScriptingTypeHandle> _sceneSystemTypes;
    bool _gameStarted = false;
    bool _isHosting = false;
    GameMode* _gameMode = nullptr;
    GameState* _gameState = nullptr;
    Array<uint32, InlinedAllocation<8>> _playersToSpawn;
#if !BUILD_RELEASE
    String _windowTitle;
#endif
    Array<Actor*> _sceneTransitionActors;
    Array<PlayerState*> _sceneTransitionPlayers;

public:
    /// <summary>
    /// Gets the singleton instance of the game instance.
    /// </summary>
    API_PROPERTY() static GameInstance* GetInstance();

    /// <summary>
    /// Gets the list of active game systems (including scene systems).
    /// </summary>
    API_PROPERTY() FORCE_INLINE const Array<GameSystem*>& GetSystems() const
    {
        return _systems;
    }

    /// <summary>
    /// Gets the game system of the given type.
    /// </summary>
    /// <param name="type">Type of the system to search for. Includes any actors derived from the type.</param>
    /// <returns>Found system or null.</returns>
    API_FUNCTION() GameSystem* GetGameSystem(API_PARAM(Attributes="TypeReference(typeof(GameSystem))") const MClass* type);

    /// <summary>
    /// Gets the game system of the given type.
    /// </summary>
    /// <param name="type">Type of the system to search for. Includes any actors derived from the type.</param>
    /// <returns>Found system or null.</returns>
    GameSystem* GetGameSystem(const ScriptingTypeHandle& type);

    /// <summary>
    /// Gets the game system of the given type.
    /// </summary>
    template<typename T>
    FORCE_INLINE T* GetGameSystem()
    {
        return (T*)GetGameSystem(T::GetStaticType());
    }

public:
    /// <summary>
    /// Event called when game starts.
    /// </summary>
    API_EVENT() Action GameStarting;

    /// <summary>
    /// Event called when game started.
    /// </summary>
    API_EVENT() Action GameStarted;

    /// <summary>
    /// Event called when game ends.
    /// </summary>
    API_EVENT() Action GameEnding;

    /// <summary>
    /// Event called when game ended.
    /// </summary>
    API_EVENT() Action GameEnded;

    /// <summary>
    /// Event called when player is spawned on a level.
    /// </summary>
    API_EVENT() Delegate<PlayerPawn*> PlayerSpawned;

    /// <summary>
    /// Event called when player is despawned from a level.
    /// </summary>
    API_EVENT() Delegate<PlayerPawn*> PlayerDespawned;

public:
    /// <summary>
    /// Gets the current game mode. Exists only on server or host, null on clients.
    /// </summary>
    API_PROPERTY() FORCE_INLINE GameMode* GetGameMode() const
    {
        return _gameMode;
    }

    /// <summary>
    /// Gets the current game state (always valid during game).
    /// </summary>
    API_PROPERTY() FORCE_INLINE GameState* GetGameState() const
    {
        return _gameState;
    }

    /// <summary>
    /// Gets the local player state (null on server). Returns the first local player in case of local coop.
    /// </summary>
    API_PROPERTY() PlayerState* GetLocalPlayerState() const;

    /// <summary>
    /// Gets all the local player states.
    /// </summary>
    API_PROPERTY() Array<PlayerState*, InlinedAllocation<8>> GetLocalPlayerStates() const;

public:
    /// <summary>
    /// Starts the game. Use it to control local game flow. Called automatically on NetworkManager events for multiplayer games.
    /// </summary>
    API_FUNCTION() void StartGame();

    /// <summary>
    /// Starts the game. Use it to end the local game.
    /// </summary>
    API_FUNCTION() void EndGame();

    /// <summary>
    /// Spans a local player. Use it when playing local game or coop.
    /// </summary>
    /// <returns>The newly added player.</returns>
    API_FUNCTION() PlayerState* SpawnLocalPlayer();

private:
    // [GamePlugin]
    void Initialize() override;
    void Deinitialize() override;

    void OnUpdate();
    void OnNetworkStateChanged();
    void OnNetworkClientConnected(NetworkClient* client);
    void OnNetworkClientDisconnected(NetworkClient* client);
    void OnSceneLoading(Scene* scene, const Guid& sceneId);
    void OnSceneLoaded(Scene* scene, const Guid& sceneId);
    void OnSceneUnloading(Scene* scene, const Guid& sceneId);
    void OnSceneUnloaded(Scene* scene, const Guid& sceneId);
    PlayerState* CreatePlayer(NetworkClient* client);
};
