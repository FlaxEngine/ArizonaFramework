// Copyright (c) Wojciech Figat. All rights reserved.

#pragma once

#include "Engine/Scripting/ScriptingType.h"
#include "Engine/Scripting/Script.h"
#include "Engine/Level/Actor.h"

/// <summary>
/// Game utilities function library.
/// </summary>
API_CLASS(Static) class ARIZONAFRAMEWORK_API Utilities
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(Utilities);

    // Gets the first typed script from the actor that is active. Input actor can be null to return null.
    template<typename T>
    static T* GetActiveScript(Actor* a)
    {
        if (!a)
            return nullptr;
        for (auto* script : a->Scripts)
        {
            if (script->Is<T>() && script->IsEnabledInHierarchy())
                return (T*)script;
        }
        return nullptr;
    }
};
