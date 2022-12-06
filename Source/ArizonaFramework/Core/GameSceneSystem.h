// Copyright (c) Wojciech Figat. All rights reserved.

#pragma once

#include "GameSystem.h"

class Scene;

/// <summary>
/// Scene gameplay component attached to the Game Instance. Lifetime tied with the scene (multiple systems can exists, one for each loaded scene).
/// </summary>
API_CLASS(Abstract) class ARIZONAFRAMEWORK_API GameSceneSystem : public GameSystem
{
    DECLARE_SCRIPTING_TYPE(GameSceneSystem);
    friend GameInstance;

private:
    Scene* _scene = nullptr;

public:
    /// <summary>
    /// Gets the scene that is connected to this system.
    /// </summary>
    API_PROPERTY() FORCE_INLINE Scene* GetScene() const
    {
        return _scene;
    }
};
