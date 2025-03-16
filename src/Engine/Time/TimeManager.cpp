#include "TimeManager.hpp"
namespace REngine
{
    TimeManager::TimeManager(float timeScale)
    {
        this->timeScale = timeScale;
    }
    TimeManager::~TimeManager() {}
    void TimeManager::SetTimeScale(float timeScale)
    {
        this->timeScale = timeScale;
    }
    float TimeManager::GetCurrentTime() const
    {
        return elapsedTime;
    }
    float TimeManager::GetDeltatime() const
    {
        return this->recordedDeltaSecond;
    }
    float TimeManager::Tick()
    {
        // 時間計測
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - lastTimeClock); // ms
        this->recordedDeltaSecond = (float)elapsed.count() * this->timeScale / 1000.0f;
        this->lastTimeClock = end;
        this->elapsedTime += this->recordedDeltaSecond;

        RemoveUnusedTimers();
        for (auto &timer : timers)
        {
            timer->Tick(this->recordedDeltaSecond);
        }
        return this->recordedDeltaSecond;
    }

    shared_ptr<Timer> TimeManager::CreateTimer(float time, bool loop, function<void()> onTimeout, bool autoStart)
    {
        auto timer = make_shared<Timer>(time, loop, onTimeout);
        timers.push_back(timer);
        return timer;
    }

    void TimeManager::RemoveUnusedTimers()
    {
        timers.erase(
            remove_if(timers.begin(), timers.end(),
                      [](const shared_ptr<Timer> &timer)
                      {
                          return timer.use_count() == 1; // TimeManager だけが保持している場合削除
                      }),
            timers.end());
    }

    void TimeManager::Clear()
    {
        timers.clear();
    }
    vector<shared_ptr<Timer>> TimeManager::GetAllTimers() const
    {
        return this->timers;
    }
}