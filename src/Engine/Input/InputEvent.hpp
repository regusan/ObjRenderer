#pragma once

#include <iostream>
#include <functional>
#include "InputStates.hpp"

namespace REngine::Input
{
    using namespace std;
    using namespace REngine;
    using namespace REngine::Input;

    class InputEvent
    {
    private:
        /* data */
    public:
        KeyID keyID;
        function<void(const KeyState keyState)> callback;

        InputEvent(/* args */);
        ~InputEvent();
    };
}