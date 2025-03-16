#include "InputStates.hpp"

namespace REngine::Input
{

    KeyState::KeyState(const KeyID keyID)
    {
        this->keyID = keyID;
    }

    KeyState::~KeyState()
    {
    }
}