#pragma once

#include <iostream>
#include <list>
#include <set>
#include <map>
#include "InputStates.hpp"
#include "InputEvent.hpp"

#include "../../BuildSetting.hpp"
#ifdef TARGET_X11BACKEND
#include "../../GUI/X11Display.hpp"
#endif

namespace REngine::Input
{
    using namespace std;
    using namespace REngine;
    using namespace REngine::Input;

    /// @brief  インプットのバインドなどを中継する
    class InputSubSystem
    {
    protected:
        // コンストラクタを private にして外部からのインスタンス化を禁止
        InputSubSystem() {}
        map<KeyID, KeyState> keyStates;
        list<InputEvent> inputEvents;

    private:
        // コピーコンストラクタと代入演算子を削除してコピーを禁止
        InputSubSystem(const InputSubSystem &) = delete;
        InputSubSystem &operator=(const InputSubSystem &) = delete;

    public:
#ifdef TARGET_X11BACKEND
        X11Display *display = nullptr;
#endif
        AxisState axisState;

        static InputSubSystem &getInstance()
        {
            static InputSubSystem instance;
            return instance;
        }

        void UpdateKeyStatus(const float deltatime, const set<KeyID> &pushedKeys)
        {
            for (auto &keyState : keyStates)
            {
                keyState.second.UpdateKeyState(pushedKeys.find(keyState.first) != pushedKeys.end()); // 集合に存在したらPush判定
            }
        }
        void UpdateAxisStatus(const float deltatime, const Vector3f position)
        {
            axisState.UpdateAxisState(deltatime, position);
        }
        KeyState GetKeyStatus(const KeyID keyID)
        {
            auto retval = this->keyStates.find(keyID);
            if (retval != this->keyStates.end())
                return this->keyStates[keyID];
            this->keyStates[keyID] = KeyState(keyID);
            return this->keyStates[keyID];
        }

        void ShowMouse(bool show)
        {
#ifdef TARGET_X11BACKEND
            if (this->display)
                this->display->X11ShowMouseCursor(show);
#endif
        }

        void SetMousePos(Vector2i pos)
        {
#ifdef TARGET_X11BACKEND
            if (this->display)
                this->display->SetMousePos(pos);
#endif
        }
        void InvokeEvents()
        {
        }
        friend ostream &operator<<(ostream &os, const InputSubSystem &is)
        {
            cout << "InputSubSystem:Registed(" << is.keyStates.size() << ")" << endl;
            for (auto keyState : is.keyStates)
            {
                os << keyState.second << endl;
            }
            return os;
        }
    };
}