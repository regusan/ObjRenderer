#pragma once

#include <iostream>
#include "../TermFormatter.hpp"
#include "../../header/EigenHeader.hpp"

namespace REngine::Input
{
    using namespace std;
    using namespace REngine;
    using namespace REngine::Input;
    using namespace REngine::Terminal;

    enum class KeyID
    {
        // アルファベットキー
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,

        // 数字キー（メインキーボード）
        Num0,
        Num1,
        Num2,
        Num3,
        Num4,
        Num5,
        Num6,
        Num7,
        Num8,
        Num9,

        // ファンクションキー
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,

        // 制御キー
        Escape,
        Tab,
        CapsLock,
        Shift,
        Ctrl,
        Alt,
        Space,
        Enter,
        Backspace,
        Delete,
        Home,
        End,
        Insert,
        PageUp,
        PageDown,

        // 矢印キー
        Left,
        Right,
        Up,
        Down,

        // 記号キー
        GraveAccent,
        Minus,
        Equal,
        LeftBracket,
        RightBracket,
        Backslash,
        Semicolon,
        Apostrophe,
        Comma,
        Period,
        Slash,

        // その他のキー
        PrintScreen,
        ScrollLock,
        Pause,
        NumLock,

        // テンキー
        Keypad0,
        Keypad1,
        Keypad2,
        Keypad3,
        Keypad4,
        Keypad5,
        Keypad6,
        Keypad7,
        Keypad8,
        Keypad9,
        KeypadMultiply,
        KeypadAdd,
        KeypadSubtract,
        KeypadDecimal,
        KeypadDivide,

        // マウス
        MouseLeft,
        MouseRight,
        MouseMiddle,

        Unknown
    };

    class KeyState
    {
    private:
        /* data */
    public:
        bool isPressed = false;
        bool isReleased = false;
        bool isHeld = false;
        KeyID keyID = KeyID::A;

        void UpdateKeyState(const bool isPressing)
        {
            this->isReleased = this->isHeld && !isPressing; // 今まで押されていて今回押されなかったら
            this->isPressed = !this->isHeld && isPressing;  // 今まで押されておらず今回押されてたら
            this->isHeld = isPressing;
        }
        KeyState() = default;
        KeyState(const KeyID keyID);
        ~KeyState();

        friend ostream &operator<<(ostream &os, const KeyState &ko)
        {

            os << "Key: " << C_CYAN << static_cast<int>(ko.keyID) << C_RESET
               << ",Pressed:" << C_CYAN << ko.isPressed << C_RESET
               << ",Held:" << C_CYAN << ko.isHeld << C_RESET
               << ",Released:" << C_CYAN << ko.isReleased << C_RESET;
            return os;
        }
    };

    class AxisState
    {
    private:
        /* data */
        Vector3f lastPosition;

    public:
        Vector3f position = Vector3f::Zero();
        Vector3f velocity = Vector3f::Zero();
        AxisState(/* args */);
        ~AxisState();
        void UpdateAxisState(const float deltasecond, const Vector3f position)
        {
            this->lastPosition = this->position;
            this->position = position;
            this->velocity = (this->position - this->lastPosition) / deltasecond;
        }
        friend ostream &operator<<(ostream &os, const AxisState &ko)
        {
            os << "Axis: " << "Position:" << C_CYAN << ko.position << C_RESET << ",Velocity:" << C_CYAN << ko.velocity << C_RESET;
            return os;
        }
    };
}
