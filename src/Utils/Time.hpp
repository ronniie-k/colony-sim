#pragma once

#include <chrono>

class Time
{
public:
    class TimeSince
    {
    public:
        TimeSince();
        TimeSince(double since);

        operator double() const;

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> timePoint;
    };

public:
    static float timeScale;

    static void Init();
    static void Update();

    static float GetTime();
    static float GetDeltaTime();

private:
    static void CalculateCurrentFrame();
    static void CalculateTime();
    static void CalculateDeltaTime();

    static std::chrono::high_resolution_clock::time_point appStart;
    static std::chrono::high_resolution_clock::time_point curFrame;
    static std::chrono::high_resolution_clock::time_point lastFrame;

    static float time;
    static float deltaTime;
};
