#include "Time.hpp"

using namespace std::chrono;

Time::TimeSince::TimeSince()
{
    timePoint = high_resolution_clock::now();
}

Time::TimeSince::TimeSince(double since)
{
    timePoint = time_point_cast<microseconds>(high_resolution_clock::now() - duration_cast<microseconds>(duration<float>(since)));
}

Time::TimeSince::operator double() const
{
    return static_cast<float>(duration_cast<microseconds>(high_resolution_clock::now() - timePoint).count()) / 10e5f;
}

float Time::timeScale = 1.f;

high_resolution_clock::time_point Time::appStart;
high_resolution_clock::time_point Time::curFrame;
high_resolution_clock::time_point Time::lastFrame;

float Time::time = 0.f;
float Time::deltaTime = 0.f;

void Time::Init()
{
    appStart = high_resolution_clock::now();
    curFrame = appStart;
    lastFrame = appStart;
}

void Time::Update()
{
    CalculateCurrentFrame();
    CalculateTime();
    CalculateDeltaTime();
}

void Time::CalculateCurrentFrame()
{
    curFrame = high_resolution_clock::now();
}

void Time::CalculateTime()
{
    time = static_cast<float>(duration_cast<milliseconds>(curFrame - appStart).count()) / 10e3f;
}

void Time::CalculateDeltaTime()
{
    deltaTime = static_cast<float>(duration_cast<microseconds>(curFrame - lastFrame).count()) / 10e5f;
    lastFrame = curFrame;
}

float Time::GetTime()
{
    return time;
}

float Time::GetDeltaTime()
{
    return deltaTime * timeScale;
}
