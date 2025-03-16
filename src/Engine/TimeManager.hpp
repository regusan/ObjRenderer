#pragma once
#include <vector>
#include <memory>
#include <chrono>
#include "Timer.hpp"

using namespace std;

/// @brief ReguEngine名前空間
namespace REngine
{
    using namespace std;

    /// @brief タイマーを管理するマネージャー
    class TimeManager
    {
    private:
        vector<shared_ptr<Timer>> timers;
        float elapsedTime = 0.0f;
        float recordedDeltaSecond = 1.0f / 30.0f;
        std::chrono::high_resolution_clock::time_point lastTimeClock = std::chrono::high_resolution_clock::now();

    public:
        float timeScale = 1.0f;
        TimeManager(float timeScale = 1.0f);
        ~TimeManager();
        void SetTimeScale(const float timeScale);
        float GetCurrentTime() const;
        float GetDeltatime() const;
        float Tick();
        shared_ptr<Timer> CreateTimer(const float time, const bool loop = false, const function<void()> onTimeout = nullptr, const bool autoStart = false);
        void RemoveUnusedTimers();
        void Clear();
        vector<shared_ptr<Timer>> GetAllTimers() const;
    };
}