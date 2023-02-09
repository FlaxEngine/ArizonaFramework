// Copyright (c) Wojciech Figat. All rights reserved.

#include "GameInstance.h"
#include "GameSystem.h"
#include "GameSceneSystem.h"
#include "GameInstanceSettings.h"
#include "GameMode.h"
#include "GameState.h"
#include "PlayerPawn.h"
#include "PlayerController.h"
#include "PlayerState.h"
#include "PlayerUI.h"
#include "Engine/Content/Content.h"
#include "Engine/Content/JsonAsset.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Config/GameSettings.h"
#include "Engine/Engine/Engine.h"
#include "Engine/Level/Level.h"
#include "Engine/Level/Scene/Scene.h"
#include "Engine/Level/Actors/EmptyActor.h"
#include "Engine/Level/Prefabs/PrefabManager.h"
#include "Engine/Networking/NetworkClient.h"
#include "Engine/Networking/NetworkManager.h"
#include "Engine/Networking/NetworkReplicator.h"
#include "Engine/Networking/NetworkRpc.h"
#include "Engine/Profiler/ProfilerCPU.h"
#if !BUILD_RELEASE
#include "Engine/Platform/Window.h"
#endif
#include "Engine/Scripting/BinaryModule.h"
#include "Engine/Scripting/Scripting.h"
#include "Engine/Scripting/ManagedCLR/MClass.h"
#include "Engine/Scripting/Plugins/PluginManager.h"
#include "Engine/Threading/Threading.h"

namespace
{
    template<typename T>
    T* Setup(const StringAnsi& typeName)
    {
        const ScriptingTypeHandle type = Scripting::FindScriptingType(typeName);
        auto obj = ScriptingObject::NewObject<T>(type);
        if (!obj)
        {
            LOG(Error, "Unknown or invalid type {0}", String(typeName));
            obj = ScriptingObject::NewObject<T>();
        }
        return obj;
    }

    template<typename T>
    void DeleteScript(T*& obj)
    {
        if (obj)
        {
            if (obj->GetActor())
                obj->GetActor()->DeleteObject();
            else
                obj->DeleteObject();
            obj = nullptr;
        }
    }
}

GameSystem::GameSystem(const SpawnParams& params)
    : ScriptingObject(params)
{
}

GameSceneSystem::GameSceneSystem(const SpawnParams& params)
    : GameSystem(params)
{
}

GameMode::GameMode(const SpawnParams& params)
    : ScriptingObject(params)
{
}

void GameMode::StartGame()
{
}

void GameMode::StopGame()
{
}

Actor* GameMode::CreatePlayerPawn(PlayerState* playerState)
{
    const auto& settings = *GameInstanceSettings::Get();
    if (Prefab* playerPawnPrefab = settings.PlayerPawnPrefab.Get())
    {
        return PrefabManager::SpawnPrefab(playerPawnPrefab, nullptr, nullptr);
    }
    return nullptr;
}

Actor* GameMode::CreatePlayerController(PlayerState* playerState)
{
    const auto& settings = *GameInstanceSettings::Get();
    if (Prefab* playerControllerPrefab = settings.PlayerControllerPrefab.Get())
    {
        return PrefabManager::SpawnPrefab(playerControllerPrefab, nullptr, nullptr);
    }
    return nullptr;
}

void GameMode::OnPlayerJoined(PlayerState* playerState)
{
}

void GameMode::OnPlayerLeft(PlayerState* playerState)
{
}

void GameMode::OnPlayerSpawned(PlayerState* playerState)
{
}

GameState::GameState(const SpawnParams& params)
    : ScriptingObject(params)
{
}

PlayerState* GameState::GetPlayerStateByNetworkClientId(uint32 networkClientId) const
{
    for (PlayerState* playerState : PlayerStates)
    {
        if (playerState && playerState->NetworkClientId == networkClientId)
            return playerState;
    }
    return nullptr;
}

PlayerState* GameState::GetPlayerStateByPlayerId(uint32 playerId) const
{
    for (PlayerState* playerState : PlayerStates)
    {
        if (playerState && playerState->PlayerId == playerId)
            return playerState;
    }
    return nullptr;
}

PlayerState::PlayerState(const SpawnParams& params)
    : ScriptingObject(params)
{
}

PlayerPawn::PlayerPawn(const SpawnParams& params)
    : Script(params)
{
}

void PlayerPawn::SetPlayerState(PlayerState* value)
{
    if (_playerState == value)
        return;
    _playerState = value;
    NetworkReplicator::DirtyObject(this);
}

void PlayerPawn::SetPlayerId(uint32 value)
{
    if (_playerId == value || value == MAX_uint32)
        return;
    _playerId = value;

    // Register for player spawn event
    if (auto* instance = GameInstance::GetInstance())
        instance->_playersToSpawn.AddUnique(value);
}

void PlayerPawn::OnStart()
{
    // Automatic spawn in the network for replication
    NetworkReplicator::SpawnObject(this);
}

void PlayerPawn::OnDestroy()
{
    // Invoke player despawn event
    if (_spawned)
    {
        if (auto* instance = GameInstance::GetInstance())
        {
            instance->_playersToSpawn.Remove(_playerId);
            instance->PlayerDespawned(this);
            if (auto* gameState = instance->GetGameState())
            {
                if (auto* playerState = gameState->GetPlayerStateByPlayerId(_playerId))
                {
                    playerState->PlayerPawn = nullptr;
                }
            }
        }
        _spawned = false;
    }
}

PlayerController::PlayerController(const SpawnParams& params)
    : Script(params)
{
    _tickUpdate = true;
}

PlayerPawn* PlayerController::GetPlayerPawn() const
{
    return _playerState ? _playerState->PlayerPawn : nullptr;
}

uint32 PlayerController::GetPlayerId() const
{
    return _playerState ? _playerState->PlayerId : MAX_uint32;
}

Actor* PlayerController::CreatePlayerUI(PlayerState* playerState)
{
    const auto& settings = *GameInstanceSettings::Get();
    if (Prefab* playerUIPrefab = settings.PlayerUIPrefab.Get())
    {
        return PrefabManager::SpawnPrefab(playerUIPrefab, nullptr, nullptr);
    }
    return nullptr;
}

void PlayerController::MovePawn(const Vector3& translation, const Quaternion& rotation)
{
    const PlayerPawn* pawn = GetPlayerPawn();
    Actor* pawnActor = pawn ? pawn->GetActor() : nullptr;
    if (!pawnActor)
        return;

    // Replicate on server
    const NetworkManagerMode networkMode = NetworkManager::Mode;
    if (networkMode == NetworkManagerMode::Client)
    {
        MovePawnServer(translation, rotation);
    }

    // Perform local move
    pawnActor->AddMovement(translation, rotation);
}

void PlayerController::MovePawnServer(const Vector3& translation, const Quaternion& rotation)
{
    NETWORK_RPC_IMPL(PlayerController, MovePawnServer, translation, rotation);

    if (OnValidateMove(translation, rotation))
    {
        MovePawn(translation, rotation);
    }
}

void PlayerController::OnUpdate()
{
}

void PlayerController::OnDestroy()
{
    if (_spawned)
    {
        if (_playerState)
        {
            _playerState->PlayerController = nullptr;
            _playerState = nullptr;
        }
        _spawned = false;
    }
}

PlayerUI::PlayerUI(const SpawnParams& params)
    : Script(params)
{
}

void PlayerUI::SetPlayerState(PlayerState* value)
{
    _playerState = value;
}

void PlayerUI::OnDestroy()
{
    // Unlink from player state
    if (_playerState)
    {
        _playerState->PlayerUI = nullptr;
        _playerState = nullptr;
    }

    Script::OnDestroy();
}

IMPLEMENT_GAME_SETTINGS_GETTER(GameInstanceSettings, "GameInstance");

GameInstance::GameInstance(const SpawnParams& params)
    : GamePlugin(SpawnParams(Guid(0x12345678, 0x99634f61, 0x84723632, 0x54c776af), params.Type)) // Override ID to be the same on all clients (a cross-device singleton) to keep network id stable
{
    _description.Category = TEXT("Game");
#if USE_EDITOR
    _description.Description = TEXT("Main game singleton plugin that manages the game systems and handles Game Mode setup and lifetime for the play.");
#endif
}

GameInstance* GameInstance::GetInstance()
{
    return PluginManager::GetPlugin<GameInstance>();
}

GameSystem* GameInstance::GetGameSystem(const MClass* type)
{
    for (auto* e : _systems)
    {
        if (e->Is(type))
            return e;
    }
    return nullptr;
}

GameSystem* GameInstance::GetGameSystem(const ScriptingTypeHandle& type)
{
    for (auto* e : _systems)
    {
        if (e->Is(type))
            return e;
    }
    return nullptr;
}

void GameInstance::Initialize()
{
    GamePlugin::Initialize();

    // Find all game system types from all loaded binary modules
    _sceneSystemTypes.Clear();
    for (BinaryModule* e : BinaryModule::GetModules())
    {
        for (int32 i = 0; i < e->Types.Count(); i++)
        {
            const ScriptingType& type = e->Types[i];
            const ScriptingTypeHandle typeHandle(e, i);
            if (type.Type == ScriptingTypes::Script && typeHandle.IsSubclassOf(GameSystem::TypeInitializer))
            {
                // Skip abstract types
                if (type.ManagedClass && type.ManagedClass->IsAbstract())
                    continue;

                if (GameSceneSystem::TypeInitializer.IsAssignableFrom(typeHandle))
                {
                    // Cache scene types
                    _sceneSystemTypes.Add(typeHandle);
                }
                else
                {
                    // Spawn game system
                    const ScriptingObjectSpawnParams spawnParams(Guid::New(), typeHandle);
                    auto* system = (GameSystem*)type.Script.Spawn(spawnParams);
                    if (!system)
                        continue;
                    system->_instance = this;
                    if (system->CanBeUsed())
                    {
                        system->Initialize();
                        _systems.Add(system);
                    }
                    else
                    {
                        Delete(system);
                    }
                }
            }
        }
    }

    // Register for network events
    Engine::Update.Bind<GameInstance, &GameInstance::OnUpdate>(this);
    NetworkManager::StateChanged.Bind<GameInstance, &GameInstance::OnNetworkStateChanged>(this);
    NetworkManager::ClientConnected.Bind<GameInstance, &GameInstance::OnNetworkClientConnected>(this);
    NetworkManager::ClientDisconnected.Bind<GameInstance, &GameInstance::OnNetworkClientDisconnected>(this);

    // Initialize scene systems
    Level::ScenesLock.Lock();
    for (Scene* scene : Level::Scenes)
        OnSceneLoading(scene, scene->GetID());
    Level::SceneLoading.Bind<GameInstance, &GameInstance::OnSceneLoading>(this);
    Level::SceneLoaded.Bind<GameInstance, &GameInstance::OnSceneLoaded>(this);
    Level::SceneUnloading.Bind<GameInstance, &GameInstance::OnSceneUnloading>(this);
    Level::SceneUnloaded.Bind<GameInstance, &GameInstance::OnSceneUnloaded>(this);
    Level::ScenesLock.Unlock();
}

void GameInstance::Deinitialize()
{
    // Ensure to stop any game
    EndGame();

    // Unregister from events
    NetworkManager::StateChanged.Unbind<GameInstance, &GameInstance::OnNetworkStateChanged>(this);
    NetworkManager::ClientConnected.Unbind<GameInstance, &GameInstance::OnNetworkClientConnected>(this);
    NetworkManager::ClientDisconnected.Unbind<GameInstance, &GameInstance::OnNetworkClientDisconnected>(this);
    Level::SceneLoading.Unbind<GameInstance, &GameInstance::OnSceneLoading>(this);
    Level::SceneLoaded.Unbind<GameInstance, &GameInstance::OnSceneLoaded>(this);
    Level::SceneUnloading.Unbind<GameInstance, &GameInstance::OnSceneUnloading>(this);
    Level::SceneUnloaded.Unbind<GameInstance, &GameInstance::OnSceneUnloaded>(this);
    Engine::Update.Unbind<GameInstance, &GameInstance::OnUpdate>(this);

    // Shutdown game systems (reversed order)
    for (int32 i = _systems.Count() - 1; i >= 0; i--)
    {
        GameSystem* system = _systems[i];
        _systems.RemoveAt(i);
        system->Deinitialize();
        Delete(system);
    }
    _sceneSystemTypes.Clear();

    GamePlugin::Deinitialize();
}

void GameInstance::OnUpdate()
{
    if (!_gameStarted)
        return;
    PROFILE_CPU();

    // Process players spawn events (ensure that both pawn and controller are ready on server and client)
    for (int32 i = 0; i < _playersToSpawn.Count() && _playersToSpawn.Count() != 0; i++)
    {
        const uint32 playerId = _playersToSpawn[i];
        if (PlayerState* playerState = _gameState->GetPlayerStateByPlayerId(playerId))
        {
            const bool needController = !NetworkManager::IsClient() || playerState->NetworkClientId == NetworkManager::LocalClientId;
            if (playerState->PlayerPawn == nullptr || playerState->PlayerPawn->GetPlayerId() != playerId)
                continue;
            if (playerState->PlayerPawn->_spawned)
            {
                // Already spawned
                _playersToSpawn.RemoveAtKeepOrder(i--);
                continue;
            }
            if (needController && playerState->PlayerController == nullptr)
                continue;

#if !BUILD_RELEASE
            // Set proper name for the player actors to improve dev usage
            ASSERT(playerState->PlayerPawn->GetParent());
            if (playerState->PlayerPawn->GetParent())
                playerState->PlayerPawn->GetParent()->SetName(String::Format(TEXT("Player Pawn PlayerId={}"), playerId));
            if (playerState->PlayerController && playerState->PlayerController->GetParent())
                playerState->PlayerController->GetParent()->SetName(String::Format(TEXT("Player Controller PlayerId={}"), playerId));
#endif

            // Spawn player
            playerState->PlayerPawn->_spawned = true;
            playerState->PlayerPawn->SetPlayerState(playerState);
            if (playerState->PlayerController)
            {
                playerState->PlayerController->_spawned = true;
                playerState->PlayerController->_playerState = playerState;
            }
            playerState->PlayerPawn->OnPlayerSpawned();
            if (playerState->PlayerController)
                playerState->PlayerController->OnPlayerSpawned();
            _playersToSpawn.RemoveAtKeepOrder(i--);

            // Create UI for local player
            if (playerState->NetworkClientId == NetworkManager::LocalClientId)
            {
                Actor* uiActor = playerState->PlayerController->CreatePlayerUI(playerState);
                PlayerUI* uiScript = uiActor ? uiActor->GetScript<PlayerUI>() : nullptr;
                if (uiActor && !uiScript)
                {
                    LOG(Error, "Invalid player UI actor spawned without PlayerUI script attached (to the root actor).");
                    Delete(uiActor);
                    uiActor = nullptr;
                }
                if (!uiActor)
                {
                    // Fallback to default UI
                    uiActor = New<EmptyActor>();
                    uiScript = uiActor->AddScript<PlayerUI>();
                }
                uiScript->SetPlayerState(playerState);
                playerState->PlayerUI = uiScript;
#if !BUILD_RELEASE
                uiActor->SetName(String::Format(TEXT("Player UI PlayerId={}"), playerId));
#endif
                Level::SpawnActor(uiActor);
                uiScript->OnPlayerSpawned();
            }

            // Custom logic after spawning player
            PlayerSpawned(playerState->PlayerPawn);
        }
    }

    // Update inputs (before scripting update)
    const PlayerState* localPlayerState = GetLocalPlayerState();
    if (localPlayerState && localPlayerState->PlayerController && localPlayerState->PlayerController->_spawned)
    {
        localPlayerState->PlayerController->OnUpdateInput();
    }
}

PlayerState* GameInstance::GetLocalPlayerState() const
{
    return _gameState ? _gameState->GetPlayerStateByNetworkClientId(NetworkManager::LocalClientId) : nullptr;
}

Array<PlayerState*, InlinedAllocation<8>> GameInstance::GetLocalPlayerStates() const
{
    Array<PlayerState*, InlinedAllocation<8>> result;
    if (_gameState)
    {
        for (PlayerState* playerState : _gameState->PlayerStates)
        {
            if (playerState && playerState->NetworkClientId == NetworkManager::LocalClientId)
                result.Add(playerState);
        }
    }
    return result;
}

void GameInstance::StartGame()
{
    ASSERT(IsInMainThread());
    if (_gameStarted)
        return;
    GameStarting();
    const NetworkManagerMode networkMode = NetworkManager::Mode;
    _isHosting = networkMode != NetworkManagerMode::Client;
    const auto& settings = *GameInstanceSettings::Get();

    // Register Game Instance as a root for networking objects
    NetworkReplicator::AddObject(this);

    // Create game mode and state
    if (_isHosting)
    {
        _gameMode = Setup<GameMode>(settings.GameModeType);
    }
    _gameState = Setup<GameState>(settings.GameStateType);
    NetworkReplicator::AddObject(_gameState, this);

    if (_isHosting)
    {
        _gameMode->StartGame();
    }

    _gameStarted = true;
    GameStarted();

    // Spawn local player
    if (networkMode == NetworkManagerMode::Host)
        OnNetworkClientConnected(NetworkManager::LocalClient);
}

void GameInstance::EndGame()
{
    ASSERT(IsInMainThread());
    if (!_gameStarted)
        return;
    if (NetworkManager::State == NetworkConnectionState::Connected)
    {
        // Disconnect when ending multi game via local end
        NetworkManager::Stop();
        return;
    }
    GameEnding();

    if (_gameMode)
    {
        ASSERT(_gameMode);
        _gameMode->StopGame();
    }

    // Delete game objects
    if (_gameState)
    {
        for (ScriptingObjectReference<PlayerState>& playerState : _gameState->PlayerStates)
        {
            if (playerState)
            {
                DeleteScript(playerState->PlayerUI);
                DeleteScript(playerState->PlayerController);
                DeleteScript(playerState->PlayerPawn);
                playerState->DeleteObject();
            }
        }
        _gameState->DeleteObject();
        _gameState = nullptr;
    }
    _sceneTransitionActors.Clear();
    _sceneTransitionPlayers.Clear();
    if (_isHosting)
    {
        _gameMode->DeleteObject();
        _gameMode = nullptr;
    }

    _gameStarted = false;
    GameEnded();
}

PlayerState* GameInstance::SpawnLocalPlayer()
{
    return CreatePlayer(NetworkManager::LocalClient);
}

void GameInstance::OnNetworkStateChanged()
{
    switch (NetworkManager::State)
    {
    case NetworkConnectionState::Connected:
        StartGame();
#if !BUILD_RELEASE
        if (Engine::MainWindow)
        {
            // Rename window to make it easier to debug multiple sessions locally
            _windowTitle = Engine::MainWindow->GetTitle();
            Engine::MainWindow->SetTitle(String::Format(TEXT("{} - {} Id {}"), _windowTitle, NetworkManager::IsClient() ? TEXT("Client") : (NetworkManager::IsHost() ? TEXT("Host") : TEXT("Server")), NetworkManager::LocalClientId));
        }
#endif
        break;
    case NetworkConnectionState::Offline:
    case NetworkConnectionState::Disconnected:
#if !BUILD_RELEASE
        if (Engine::MainWindow)
            Engine::MainWindow->SetTitle(_windowTitle);
#endif
        EndGame();
        break;
    }
}

void GameInstance::OnNetworkClientConnected(NetworkClient* client)
{
    if (NetworkManager::IsClient() || !_gameStarted)
        return;
    CreatePlayer(client);
}

void GameInstance::OnNetworkClientDisconnected(NetworkClient* client)
{
    if (NetworkManager::IsClient() || !_gameStarted)
        return;

    // Remove player(s) from that client
    for (int32 i = 0; i < _gameState->PlayerStates.Count(); i++)
    {
        auto playerState = _gameState->PlayerStates[i];
        if (playerState && playerState->NetworkClientId == client->ClientId)
        {
            _gameMode->OnPlayerLeft(playerState);
            _gameState->PlayerStates.RemoveAtKeepOrder(i--);
            DeleteScript(playerState->PlayerUI);
            NetworkReplicator::DespawnObject(playerState->PlayerController);
            NetworkReplicator::DespawnObject(playerState->PlayerPawn);
            NetworkReplicator::DespawnObject(playerState);
        }
    }
}

void GameInstance::OnSceneLoading(Scene* scene, const Guid& sceneId)
{
    for (const ScriptingTypeHandle& typeHandle : _sceneSystemTypes)
    {
        const ScriptingObjectSpawnParams spawnParams(Guid::New(), typeHandle);
        auto* system = (GameSceneSystem*)typeHandle.GetType().Script.Spawn(spawnParams);
        if (!system)
            continue;
        system->_instance = this;
        system->_scene = scene;
        if (system->CanBeUsed())
        {
            system->Initialize();
            _systems.Add(system);
        }
        else
        {
            Delete(system);
        }
    }
}

void GameInstance::OnSceneLoaded(Scene* scene, const Guid& sceneId)
{
    // If game performed scene transition, then respawn any cached scene objects
    if (_gameStarted)
    {
        for (Actor* a : _sceneTransitionActors)
            a->SetParent(scene);
        _sceneTransitionActors.Clear();
        if (_gameMode)
        {
            for (PlayerState* player : _sceneTransitionPlayers)
                _gameMode->OnPlayerSpawned(player);
        }
        _sceneTransitionPlayers.Clear();
    }
}

void GameInstance::OnSceneUnloading(Scene* scene, const Guid& sceneId)
{
    // If game performs scene transition, then unlink any scene objects (player pawn/controller/ui actors) to be respawned after new map gets loaded
    if (_gameStarted)
    {
        for (int32 i = 0; i < _gameState->PlayerStates.Count(); i++)
        {
            auto playerState = _gameState->PlayerStates[i];
            if (playerState)
            {
                Actor* a;
#define TRANSITION_SCRIPT(s) \
                a = playerState->s ? playerState->s->GetActor() : nullptr; \
                if (a && a->GetScene() == scene) \
                { \
                    _sceneTransitionActors.Add(a); \
                    a->SetParent(nullptr); \
                }
                TRANSITION_SCRIPT(PlayerUI);
                TRANSITION_SCRIPT(PlayerController);
                TRANSITION_SCRIPT(PlayerPawn);
#undef TRANSITION_SCRIPT
                _sceneTransitionPlayers.Add(playerState);
            }
        }
    }
}

void GameInstance::OnSceneUnloaded(Scene* scene, const Guid& sceneId)
{
    for (int32 i = _systems.Count() - 1; i >= 0; i--)
    {
        auto* system = Cast<GameSceneSystem>(_systems[i]);
        if (!system || system->_scene != scene)
            continue;
        _systems.RemoveAt(i);
        system->Deinitialize();
        Delete(system);
    }
}

PlayerState* GameInstance::CreatePlayer(NetworkClient* client)
{
    // Add player
    const auto& settings = *GameInstanceSettings::Get();
    auto playerState = Setup<PlayerState>(settings.PlayerStateType);
    if (client)
        playerState->NetworkClientId = client->ClientId;
    else
        playerState->NetworkClientId = NetworkManager::LocalClientId;
    playerState->PlayerId = _gameState->NextPlayerId++; // TODO: for local coop use RPC to synchronize remote session with server
    _gameState->PlayerStates.Add(playerState);
    NetworkReplicator::AddObject(playerState, this);
    NetworkReplicator::SpawnObject(playerState);

    // Create player pawn
    Actor* pawnActor = _gameMode->CreatePlayerPawn(playerState);
    PlayerPawn* pawnScript = pawnActor ? pawnActor->GetScript<PlayerPawn>() : nullptr;
    if (pawnActor && !pawnScript)
    {
        LOG(Error, "Invalid player pawn actor spawned without PlayerPawn script attached (to the root actor).");
        Delete(pawnActor);
        pawnActor = nullptr;
    }
    if (!pawnActor)
    {
        // Fallback to default pawn
        pawnActor = New<EmptyActor>();
        pawnScript = pawnActor->AddScript<PlayerPawn>();
    }
    pawnScript->SetPlayerState(playerState);
    pawnScript->SetPlayerId(playerState->PlayerId);
    playerState->PlayerPawn = pawnScript;

    // Spawn player pawn on all connected clients and locally
    NetworkReplicator::SpawnObject(pawnActor);
    Level::SpawnActor(pawnActor);

    // Create player controller
    Actor* controllerActor = _gameMode->CreatePlayerController(playerState);
    PlayerController* controllerScript = controllerActor ? controllerActor->GetScript<PlayerController>() : nullptr;
    if (controllerActor && !controllerScript)
    {
        LOG(Error, "Invalid player controller actor spawned without PlayerController script attached (to the root actor).");
        Delete(controllerActor);
        controllerActor = nullptr;
    }
    if (!controllerActor)
    {
        // Fallback to default controller
        controllerActor = New<EmptyActor>();
        controllerScript = controllerActor->AddScript<PlayerController>();
    }
    controllerScript->_playerState = playerState;
    playerState->PlayerController = controllerScript;

    if (NetworkManager::IsConnected())
    {
        // Spawn player controller on connected client and locally (client ownership over controller)
        if (NetworkManager::LocalClientId != playerState->NetworkClientId)
        {
            const uint32 controllerTargetsData[2] = { NetworkManager::LocalClientId, playerState->NetworkClientId };
            const DataContainer<uint32> controllerTargets(controllerTargetsData, ARRAY_COUNT(controllerTargetsData));
            // TODO: support inheritance of targetClientIds for spawned objects so if we set it for controller actor, all attached scripts to it will inherit that too
            NetworkReplicator::SpawnObject(controllerActor, controllerTargets);
            NetworkReplicator::SpawnObject(controllerScript, controllerTargets);
            NetworkReplicator::SetObjectOwnership(controllerActor, client->ClientId, NetworkObjectRole::ReplicatedSimulated, true);
        }
        else
        {
            const uint32 controllerTargetsData[1] = { NetworkManager::LocalClientId };
            const DataContainer<uint32> controllerTargets(controllerTargetsData, ARRAY_COUNT(controllerTargetsData));
            // TODO: support inheritance of targetClientIds for spawned objects so if we set it for controller actor, all attached scripts to it will inherit that too
            NetworkReplicator::SpawnObject(controllerActor, controllerTargets);
            NetworkReplicator::SpawnObject(controllerScript, controllerTargets);
            NetworkReplicator::SetObjectOwnership(controllerActor, client->ClientId, NetworkObjectRole::OwnedAuthoritative, true);
        }
    }
    else
    {
        // Offline mode requires some manual setup (no events from replication system)
        _playersToSpawn.AddUnique(playerState->PlayerId);
    }
    Level::SpawnActor(controllerActor);

    _gameMode->OnPlayerJoined(playerState);
    _gameMode->OnPlayerSpawned(playerState);

    return playerState;
}
