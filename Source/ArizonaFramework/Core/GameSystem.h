// Copyright (c) Wojciech Figat. All rights reserved.

#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Types.h"

/// <summary>
/// Gameplay system component attached to the Game Instance. Lifetime tied  with the game.
/// </summary>
API_CLASS(Abstract) class ARIZONAFRAMEWORK_API GameSystem : public ScriptingObject
{
    DECLARE_SCRIPTING_TYPE(GameSystem);
    friend GameInstance;

private:
    GameInstance* _instance = nullptr;

public:
    /// <summary>
    /// Gets the game instance that owns this system.
    /// </summary>
    API_PROPERTY() FORCE_INLINE GameInstance* GetGameInstance() const
    {
        return _instance;
    }

    /// <summary>
    /// Checks if the system can be used. Called before Initialize/Deinitialize but with game instance set.
    /// </summary>
    API_FUNCTION() virtual bool CanBeUsed()
    {
        return true;
    }

    /// <summary>
    /// Initialization method for the system. Can be used to allocate resource and setup event handlers.
    /// </summary>
    API_FUNCTION() virtual void Initialize()
    {
    }

    /// <summary>
    /// Cleanup method for the system. Used to release any used resources.
    /// </summary>
    API_FUNCTION() virtual void Deinitialize()
    {
    }
};
