#pragma once

#include "ReplicationSettings.h"
#include "Engine/Core/ISerializable.h"

/// <summary>
/// Replication settings container for object.
/// </summary>
API_STRUCT() struct ARIZONAFRAMEWORK_API ReplicationSettings : ISerializable
{
    API_AUTO_SERIALIZATION();
    DECLARE_SCRIPTING_TYPE_MINIMAL(ReplicationSettings);

    // The target amount of the replication updates per second (frequency of the replication). Constrained by NetworkFPS specified in NetworkSettings. Use 0 for 'always relevant' object.
    API_FIELD() float ReplicationFPS = 60;
    // The minimum distance from the player to the object at which it can process replication. For example, players further away won't receive object data. Use 0 if unused.
    API_FIELD() float CullDistance = 15000;
};
