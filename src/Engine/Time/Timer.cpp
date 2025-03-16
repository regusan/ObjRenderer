#include "Timer.hpp"
namespace REngine
{
    Timer::Timer(float time, bool loop, function<void()> onTimeout, bool autoStart)
        : remainingTime(time), setTime(time), isLooping(loop), isRunning(autoStart), callback(onTimeout) {}

    void Timer::Start()
    {
        isRunning = true;
        remainingTime = setTime;
    }

    void Timer::Stop()
    {
        isRunning = false;
        remainingTime = setTime;
    }

    void Timer::Pause()
    {
        isRunning = false;
    }

    void Timer::Resume()
    {
        isRunning = true;
    }

    void Timer::SetTime(float time)
    {
        setTime = time;
        remainingTime = time;
    }

    void Timer::SetCallback(function<void()> onTimeout)
    {
        callback = onTimeout;
    }

    void Timer::Tick(float deltaSecond)
    {
        if (isRunning && remainingTime > 0.0f)
        {
            remainingTime -= deltaSecond;
            if (remainingTime <= 0.0f)
            {
                if (callback)
                {
                    callback();
                }
                if (isLooping)
                {
                    remainingTime = setTime;
                }
                else
                {
                    isRunning = false;
                }
            }
        }
    }

    float Timer::GetRemainingTime() const
    {
        return remainingTime;
    }
}