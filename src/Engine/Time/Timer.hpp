#pragma once
#include <functional>
namespace REngine
{
    using namespace std;

    /// @brief 時間を管理しコールバックを実行するタイマー
    class Timer
    {
    private:
        float remainingTime;
        float setTime;
        bool isLooping;
        bool isRunning;
        function<void()> callback;

    public:
        Timer(float time, bool loop = false, function<void()> onTimeout = nullptr, bool autoStart = false);
        void Start();
        void Stop();
        void Pause();
        void Resume();
        void SetTime(float time);
        void Tick(float deltaSecond);
        void SetCallback(function<void()> onTimeout);
        float GetRemainingTime() const;
    };
}