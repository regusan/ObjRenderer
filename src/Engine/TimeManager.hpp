#pragma once
#include <vector>
#include <memory>
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
        float lastTickedTime = 0.0f;

    public:
        float timeScale = 1.0f;
        TimeManager(float timeScale = 1.0f);
        ~TimeManager();
        void SetTimeScale(float timeScale);
        float GetCurrentTime() const;
        void Tick(float deltaSecond);
        shared_ptr<Timer> CreateTimer(float time, bool loop = false, function<void()> onTimeout = nullptr, bool autoStart = false);
        void RemoveUnusedTimers();
        void Clear();
        vector<shared_ptr<Timer>> GetAllTimers() const;
    };
}