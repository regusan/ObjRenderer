#pragma once
#pragma once

#include "Actor.hpp"
#include "../header/RenderingHeader.hpp"

namespace REngine::Component
{
    /// @brief メッシュを持つアクター
    class RigitBody : public Actor
    {
    private:
    public:
        bool enable;
        RigitBody();
        RigitBody(json args);
        ~RigitBody();
    };
    GCLASS(RigitBody)
}