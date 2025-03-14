#pragma once

#include <chrono>

namespace Utils
{
    using namespace std::chrono;
    
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
            time_point<high_resolution_clock> timePoint;
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

        
        static high_resolution_clock::time_point appStart;
        static high_resolution_clock::time_point curFrame;
        static high_resolution_clock::time_point lastFrame;

        static float time;
        static float deltaTime;
    };
}

typedef Utils::Time Time;
using Time = Utils::Time;