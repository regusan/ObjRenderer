#pragma once
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <set>
#include <map>
#include "../Engine/Input/InputEvent.hpp"

namespace REngine::Input
{
    using namespace REngine::Input;

    std::map<KeySym, KeyID> remap = {
        // アルファベットキー
        {XK_a, KeyID::A},
        {XK_b, KeyID::B},
        {XK_c, KeyID::C},
        {XK_d, KeyID::D},
        {XK_e, KeyID::E},
        {XK_f, KeyID::F},
        {XK_g, KeyID::G},
        {XK_h, KeyID::H},
        {XK_i, KeyID::I},
        {XK_j, KeyID::J},
        {XK_k, KeyID::K},
        {XK_l, KeyID::L},
        {XK_m, KeyID::M},
        {XK_n, KeyID::N},
        {XK_o, KeyID::O},
        {XK_p, KeyID::P},
        {XK_q, KeyID::Q},
        {XK_r, KeyID::R},
        {XK_s, KeyID::S},
        {XK_t, KeyID::T},
        {XK_u, KeyID::U},
        {XK_v, KeyID::V},
        {XK_w, KeyID::W},
        {XK_x, KeyID::X},
        {XK_y, KeyID::Y},
        {XK_z, KeyID::Z},

        // 数字キー
        {XK_0, KeyID::Num0},
        {XK_1, KeyID::Num1},
        {XK_2, KeyID::Num2},
        {XK_3, KeyID::Num3},
        {XK_4, KeyID::Num4},
        {XK_5, KeyID::Num5},
        {XK_6, KeyID::Num6},
        {XK_7, KeyID::Num7},
        {XK_8, KeyID::Num8},
        {XK_9, KeyID::Num9},

        // ファンクションキー
        {XK_F1, KeyID::F1},
        {XK_F2, KeyID::F2},
        {XK_F3, KeyID::F3},
        {XK_F4, KeyID::F4},
        {XK_F5, KeyID::F5},
        {XK_F6, KeyID::F6},
        {XK_F7, KeyID::F7},
        {XK_F8, KeyID::F8},
        {XK_F9, KeyID::F9},
        {XK_F10, KeyID::F10},
        {XK_F11, KeyID::F11},
        {XK_F12, KeyID::F12},

        // 制御キー
        {XK_Escape, KeyID::Escape},
        {XK_Tab, KeyID::Tab},
        {XK_Caps_Lock, KeyID::CapsLock},
        {XK_Shift_L, KeyID::Shift},
        {XK_Shift_R, KeyID::Shift},
        {XK_Control_L, KeyID::Ctrl},
        {XK_Control_R, KeyID::Ctrl},
        {XK_Alt_L, KeyID::Alt},
        {XK_Alt_R, KeyID::Alt},
        {XK_space, KeyID::Space},
        {XK_Return, KeyID::Enter},
        {XK_BackSpace, KeyID::Backspace},
        {XK_Delete, KeyID::Delete},
        {XK_Home, KeyID::Home},
        {XK_End, KeyID::End},
        {XK_Insert, KeyID::Insert},
        {XK_Page_Up, KeyID::PageUp},
        {XK_Page_Down, KeyID::PageDown},

        // 矢印キー
        {XK_Left, KeyID::Left},
        {XK_Right, KeyID::Right},
        {XK_Up, KeyID::Up},
        {XK_Down, KeyID::Down},

        // 記号キー
        {XK_grave, KeyID::GraveAccent},
        {XK_minus, KeyID::Minus},
        {XK_equal, KeyID::Equal},
        {XK_bracketleft, KeyID::LeftBracket},
        {XK_bracketright, KeyID::RightBracket},
        {XK_backslash, KeyID::Backslash},
        {XK_semicolon, KeyID::Semicolon},
        {XK_apostrophe, KeyID::Apostrophe},
        {XK_comma, KeyID::Comma},
        {XK_period, KeyID::Period},
        {XK_slash, KeyID::Slash},

        // その他のキー
        {XK_Print, KeyID::PrintScreen},
        {XK_Scroll_Lock, KeyID::ScrollLock},
        {XK_Pause, KeyID::Pause},
        {XK_Num_Lock, KeyID::NumLock},

        // テンキー
        {XK_KP_0, KeyID::Keypad0},
        {XK_KP_1, KeyID::Keypad1},
        {XK_KP_2, KeyID::Keypad2},
        {XK_KP_3, KeyID::Keypad3},
        {XK_KP_4, KeyID::Keypad4},
        {XK_KP_5, KeyID::Keypad5},
        {XK_KP_6, KeyID::Keypad6},
        {XK_KP_7, KeyID::Keypad7},
        {XK_KP_8, KeyID::Keypad8},
        {XK_KP_9, KeyID::Keypad9},
        {XK_KP_Multiply, KeyID::KeypadMultiply},
        {XK_KP_Add, KeyID::KeypadAdd},
        {XK_KP_Subtract, KeyID::KeypadSubtract},
        {XK_KP_Decimal, KeyID::KeypadDecimal},
        {XK_KP_Divide, KeyID::KeypadDivide}};
    set<KeyID> X11Event2REvent(Display *display)
    {
        set<KeyID> retval;

        char keys[32];
        XQueryKeymap(display, keys);
        // 全てのキーが押されているかチェックしする
        for (int i = 0; i < 256; ++i)
        {
            if (keys[i / 8] & (1 << (i % 8))) // ビットマスクでチェック
            {
                KeySym keysym = XkbKeycodeToKeysym(display, i, 0, 0);
                // 無効なキーは無視
                if (keysym != NoSymbol)
                {
                    retval.insert(remap[keysym]);
                }
            }
        } // 🔹 マウスボタンの状態を取得
        Window root, child;
        int rootX, rootY, winX, winY;
        unsigned int mask;
        if (XQueryPointer(display, DefaultRootWindow(display), &root, &child, &rootX, &rootY, &winX, &winY, &mask))
        {
            if (mask & Button1Mask)
                retval.insert(KeyID::MouseLeft);
            if (mask & Button2Mask)
                retval.insert(KeyID::MouseMiddle);
            if (mask & Button3Mask)
                retval.insert(KeyID::MouseRight);
        }

        return retval;
    }
}