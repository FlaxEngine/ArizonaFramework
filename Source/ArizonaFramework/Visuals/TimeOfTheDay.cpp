// Copyright (c) Wojciech Figat. All rights reserved.

#include "TimeOfTheDay.h"
#include "Engine/Core/Log.h"
#include "Engine/Engine/Time.h"
#include "Engine/Profiler/ProfilerCPU.h"

TimeOfTheDay* TimeOfTheDay::Instance = nullptr;

TimeOfTheDay::TimeOfTheDay(const SpawnParams& params)
    : Script(params)
    , _time(0)
    , StartTime(2020, 5, 3, 10, 30)
    , DayTime(0, 1, 0)
{
    _tickUpdate = true;

    // Default sun curve
    SunLightColorCurve.Resize(5);
    SunLightColorCurve[0] = LinearCurveKeyframe<Color>(0.0f, Color::Transparent);
    SunLightColorCurve[1] = LinearCurveKeyframe<Color>(0.1f, Color(0.991379f, 0.796836f, 0.427773f, 0.9f));
    SunLightColorCurve[2] = LinearCurveKeyframe<Color>(0.2f, Color(0.991379f, 0.893238f, 0.598813f));
    SunLightColorCurve[3] = LinearCurveKeyframe<Color>(0.4f, Color(1.0f, 0.966467f, 0.911958f));
    SunLightColorCurve[4] = LinearCurveKeyframe<Color>(1.0f, Color(1.0f, 0.977460f, 0.911515f));
}

DateTime TimeOfTheDay::GetCurrentTime() const
{
    return _time;
}

void TimeOfTheDay::SetCurrentTime(DateTime value)
{
    if (_time == value)
        return;
    DateTime previous = _time;
    _time = value;
    OnTimeChanged(previous);
}

TimeSpan TimeOfTheDay::GetCurrentTimeOfDay() const
{
    return _time.GetTimeOfDay();
}

DateTime TimeOfTheDay::GetPreciseTime() const
{
    return _time + TimeSpan::FromSeconds((double)_timeAccumulated * ((double)TimeSpan::TicksPerDay / (double)DayTime.Ticks));
}

void TimeOfTheDay::OnTimeChanged(DateTime previous)
{
    PROFILE_CPU();

    // Fixed sunrise at 6:00, sunset at 18:00
    TimeSpan timeOfDay = _time.GetTimeOfDay();
    float dayProgress = (float)((double)timeOfDay.Ticks / (double)TimeSpan::TicksPerDay);
    float sunAngle = dayProgress * 360.0f - 90.0f;
    _sunPosition = Math::Sin(sunAngle * DegreesToRadians);
    //LOG(Info, "Day time {}:{}, progress: {}", timeOfDay.GetHours(), timeOfDay.GetMinutes(), dayProgress);

    // Update sun rotation to match the current time
    auto sunLight = SunLight.Get();
    if (!sunLight)
        sunLight = Cast<DirectionalLight>(GetParent());
    if (sunLight)
    {
        if (_sunPitch > MAX_uint16)
        {
            // Cache base values
            _sunPitch = sunLight->GetOrientation().GetEuler().Y;
            _sunShadowDepthBias = sunLight->ShadowsDepthBias;
        }

        // Control sun rotation based on the current time
        sunLight->SetOrientation(Quaternion::Euler(sunAngle, _sunPitch, 0));

        // Control sun color based on the current time
        SunLightColorCurve.Evaluate(sunLight->Color, _sunPosition, false);

        // Scale sun shadow bias to reduce shadow acne at low angles
        sunLight->ShadowsDepthBias = _sunShadowDepthBias * (1 + (1 - Math::Abs(_sunPosition)) * 3.0f);

        // TODO: add control over skylight to match lighting
        // TODO: add moon with its own rotation and color curve
        // TODO: add night skybox and fade out day-time sky
    }
}

void TimeOfTheDay::OnEnable()
{
    ASSERT(!Instance)
    Instance = this;
}

void TimeOfTheDay::OnDisable()
{
    Instance = nullptr;
}

void TimeOfTheDay::OnStart()
{
    SetCurrentTime(StartTime);
}

void TimeOfTheDay::OnUpdate()
{
    // Merge global time scale with the local one
    float speedScale = SpeedScale * (UseTimeScale ? Time::TimeScale : 1.0f);
    if (Math::IsZero(speedScale))
        return;

    // Move time forward
    _timeAccumulated += Time::GetDeltaTime() * speedScale;

    // Calculate the time delta
    float updateInternal = UpdateInternal / Math::Abs(speedScale);
    updateInternal *= Math::Lerp(0.01f, 1.0f, Math::Square(Math::Abs(_sunPosition))); // Perform more frequent updates when sun is close to the horizon
    float timeDelta;
    if (updateInternal > 0)
    {
        // Intervals
        if (Math::Abs(_timeAccumulated) < updateInternal)
            return;
        float timeLeft = Math::Mod(_timeAccumulated, updateInternal);
        timeDelta = _timeAccumulated - timeLeft;
        _timeAccumulated = timeLeft;
    }
    else
    {
        // Every frame
        timeDelta = _timeAccumulated;
        _timeAccumulated = 0;
    }

    // Convert in-game time to gameplay time
    timeDelta *= (float)((double)TimeSpan::TicksPerDay / (double)DayTime.Ticks);

    // Update time
    SetCurrentTime(_time + TimeSpan::FromSeconds(timeDelta));
}
