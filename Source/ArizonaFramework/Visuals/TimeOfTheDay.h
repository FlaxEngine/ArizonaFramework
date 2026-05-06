// Copyright (c) Wojciech Figat. All rights reserved.

#pragma once

#include "Engine/Scripting/Script.h"
#include "Engine/Core/Types/DateTime.h"
#include "Engine/Core/Types/TimeSpan.h"
#include "Engine/Level/Actors/DirectionalLight.h"
#include "Engine/Scripting/ScriptingObjectReference.h"
#include "Engine/Animations/Curve.h"

/// <summary>
/// Simulates realistic time fo the day cycle by changing the sun position and sky colors based on the date and time. It can be used to create a dynamic day-night cycle in the game.
/// </summary>
API_CLASS() class ARIZONAFRAMEWORK_API TimeOfTheDay : public Script
{
    API_AUTO_SERIALIZATION();
    DECLARE_SCRIPTING_TYPE(TimeOfTheDay);

private:
    DateTime _time;
    float _timeAccumulated = 0;
    float _sunPitch = MAX_float;
    float _sunShadowDepthBias;
    float _sunPosition = 1.0f; // [-1; 1] where 0 is sunset/sunrise, 1 is noon

public:
    /// <summary>
    /// Defines the start date and time for the day-night cycle.
    /// </summary>
    API_FIELD(Attributes="EditorDisplay(\"Time\")")
    DateTime StartTime;

    /// <summary>
    /// Gameplay time it takes for a full in-game cycle. For example, 24-hours in game can be equal to 1h of the gameplay time.
    /// </summary>
    API_FIELD(Attributes="EditorDisplay(\"Time\")")
    TimeSpan DayTime;

    /// <summary>
    /// Speed scale for the time of the day dilatation. Can be used to speed up or slow down the time.
    /// </summary>
    API_FIELD(Attributes="EditorDisplay(\"Time\")")
    float SpeedScale = 1.0f;

    /// <summary>
    /// Enables using time scale to dilate the time of the day.
    /// </summary>
    API_FIELD(Attributes="EditorDisplay(\"Time\")")
    bool UseTimeScale = true;

    /// <summary>
    /// Gameplay time interval (in seconds) for updating the time of the day. For example, if set to 1 second, the time will be updated every second of the gameplay time. Setting it to 0 will update the time every frame, which can be performance intensive.
    /// </summary>
    API_FIELD(Attributes="EditorDisplay(\"Time\")")
    float UpdateInternal = 1.0f;

public:
    /// <summary>
    /// Directional light actor reference. If empty, parent actor can be used.
    /// </summary>
    API_FIELD(Attributes="EditorDisplay(\"Visuals\")")
    ScriptingObjectReference<DirectionalLight> SunLight;

    /// <summary>
    /// Color curve for sun light colors. In time range [-1;1] where positive time values (between [0;1]) are for day-time and negative values (between [-1;0)) are for night-time.
    /// </summary>
    API_FIELD(Attributes = "EditorDisplay(\"Visuals\")")
    LinearCurve<Color> SunLightColorCurve;

public:
    /// <summary>
    /// Global time of the day instance (singleton).
    /// </summary>
    API_FIELD() static TimeOfTheDay* Instance;

    /// <summary>
    /// Gets the current date and time.
    /// </summary>
    API_PROPERTY(Attributes="EditorDisplay(\"Time\"), NoSerialize, HideInEditor(ShowInPlayMode = true)")
    DateTime GetCurrentTime() const;

    /// <summary>
    /// Set the current date and time.
    /// </summary>
    /// <param name="value">The value to set.</param>
    API_PROPERTY() void SetCurrentTime(DateTime value);

    /// <summary>
    /// Gets the current time within a day (elapsed since midnight of this date).
    /// </summary>
    API_PROPERTY() TimeSpan GetCurrentTimeOfDay() const;

    /// <summary>
    /// Gets the current precise date and time (including accumulated time deltas).
    /// </summary>
    API_PROPERTY() DateTime GetPreciseTime() const;

protected:
    virtual void OnTimeChanged(DateTime previous);

public:
    // [Script]
    void OnEnable() override;
    void OnDisable() override;
    void OnStart() override;
    void OnUpdate() override;
};
