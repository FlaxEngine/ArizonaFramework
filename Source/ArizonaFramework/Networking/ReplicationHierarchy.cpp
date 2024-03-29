#include "ReplicationHierarchy.h"
#include "ArizonaFramework/Core/GameInstance.h"
#include "ArizonaFramework/Core/GameInstanceSettings.h"
#include "ArizonaFramework/Core/GameState.h"
#include "ArizonaFramework/Core/PlayerPawn.h"
#include "ArizonaFramework/Core/PlayerState.h"
#include "Engine/Level/Actor.h"
#include "Engine/Networking/NetworkClient.h"
#include "Engine/Networking/NetworkManager.h"

Dictionary<ScriptingTypeHandle, ReplicationSettings> GlobalReplicationSettings;
float ReplicationHierarchy::ReplicationScale = 1.0f;

ReplicationHierarchy::~ReplicationHierarchy()
{
    SAFE_DELETE(_grid);
}

void ReplicationHierarchy::SetSettings(ScriptingTypeHandle type, const ReplicationSettings& settings)
{
    GlobalReplicationSettings[type] = settings;
#if USE_EDITOR
    // TODO: register event for type.Module unloading to safely remove type ref
#endif
}

void ReplicationHierarchy::AddObject(NetworkReplicationHierarchyObject obj)
{
    // Get object settings
    ScriptingTypeHandle typeHandle = obj.Object->GetTypeHandle();
    ReplicationSettings settings;
    if (!_settingsCache.TryGet(typeHandle, settings))
    {
        // Resolve settings
        const auto& gameSettings = GameInstanceSettings::Get();
        settings = gameSettings->DefaultReplicationSettings;
        while (typeHandle)
        {
            // Overriden by code
            if (GlobalReplicationSettings.TryGet(typeHandle, settings))
                break;

            // Overriden by game settings
            const ScriptingType& type = typeHandle.GetType();
            if (gameSettings->ReplicationSettingsPerType.TryGet(type.Fullname, settings))
                break;

            typeHandle = type.GetBaseType();
        }

        // Cache result
        typeHandle = obj.Object->GetTypeHandle();
        _settingsCache.Add(typeHandle, settings);
    }
    obj.ReplicationFPS = settings.ReplicationFPS;
    obj.CullDistance = settings.CullDistance;

    const Actor* actor = obj.GetActor();
    if (actor && actor->HasStaticFlag(StaticFlags::Transform))
    {
        // Insert static objects into a grid for faster replication
        if (!_grid)
            _grid = New<NetworkReplicationGridNode>();
        _grid->AddObject(obj);
        return;
    }

    NetworkReplicationHierarchy::AddObject(obj);
}

bool ReplicationHierarchy::RemoveObject(ScriptingObject* obj)
{
    if (_grid && _grid->RemoveObject(obj))
        return true;
    return NetworkReplicationHierarchy::RemoveObject(obj);
}

bool ReplicationHierarchy::DirtyObject(ScriptingObject* obj)
{
    if (_grid && _grid->DirtyObject(obj))
        return true;
    return NetworkReplicationHierarchy::DirtyObject(obj);
}

void ReplicationHierarchy::Update(NetworkReplicationHierarchyUpdateResult* result)
{
    if (const auto* instance = GameInstance::GetInstance())
    {
        // Setup players locations for distance culling
        const auto& clients = NetworkManager::Clients;
        for (int32 i = 0; i < clients.Count(); i++)
        {
            if (const auto* playerState = instance->GetGameState()->GetPlayerStateByNetworkClientId(clients[i]->ClientId))
            {
                if (playerState->PlayerPawn && playerState->PlayerPawn->GetActor())
                {
                    const Vector3 playerPosition = playerState->PlayerPawn->GetActor()->GetPosition();
                    result->SetClientLocation(i, playerPosition);
                }
            }
        }
    }

    // Apply settings
    result->ReplicationScale *= ReplicationScale;

    // Update hierarchy
    if (_grid)
        _grid->Update(result);
    NetworkReplicationHierarchy::Update(result);
}
