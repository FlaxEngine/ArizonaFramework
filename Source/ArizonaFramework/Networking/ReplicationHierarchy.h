#pragma once

#include "Engine/Networking/NetworkReplicationHierarchy.h"
#include "ReplicationSettings.h"

/// <summary>
/// Basic implementation of NetworkReplicationHierarchy that uses spatial grid for static actor objects and allows to configure replication settings per-type.
/// </summary>
API_CLASS() class ARIZONAFRAMEWORK_API ReplicationHierarchy : public NetworkReplicationHierarchy
{
    DECLARE_SCRIPTING_TYPE_WITH_CONSTRUCTOR_IMPL(ReplicationHierarchy, NetworkReplicationHierarchy);
    ~ReplicationHierarchy();

private:
    NetworkReplicationGridNode* _grid = nullptr;
    Dictionary<ScriptingTypeHandle, ReplicationSettings> _settingsCache;

public:
    // Scales globally replication rate for all objects in hierarchy (normalized scale - eg. 0.7 slows down rep rate by 30%).
    API_FIELD() static float ReplicationScale;

    /// <summary>
    /// Sets the replication settings for a given type (globally).
    /// </summary>
    /// <param name="type">The object type.</param>
    /// <param name="settings">The replication settings.</param>
    API_FUNCTION() static void SetSettings(ScriptingTypeHandle type, const ReplicationSettings& settings);

    // [NetworkReplicationHierarchy]
    void AddObject(NetworkReplicationHierarchyObject obj) override;
    bool RemoveObject(ScriptingObject* obj) override;
    bool DirtyObject(ScriptingObject* obj) override;
    void Update(NetworkReplicationHierarchyUpdateResult* result) override;
};
