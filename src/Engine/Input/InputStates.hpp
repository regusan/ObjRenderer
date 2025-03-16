#pragma once

#include <iostream>

namespace REngine::Input
{
    using namespace std;
    using namespace REngine;
    using namespace REngine::Input;
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
            os << "Key: " << (int)ko.keyID << ",Pressed:" << ko.isPressed << ",Held:" << ko.isHeld << ",Released:" << ko.isReleased;
            return os;
        }
    };
}
