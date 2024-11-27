#include <iostream>
#include <functional>
#include <vector>
#include <type_traits>

// イベントのディスパッチャ
template <typename EventType>
class EventDispatcher
{
public:
    // イベントハンドラを登録する
    void addListener(std::function<void(const EventType &)> listener)
    {
        listeners.push_back(listener);
    }

    // イベントをディスパッチ（発生）させる
    void dispatch(const EventType &event) const
    {
        for (const auto &listener : listeners)
        {
            listener(event); // リスナーを呼び出す
        }
    }

private:
    std::vector<std::function<void(const EventType &)>> listeners;
};
