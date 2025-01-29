#pragma once

#include "VertOutputStandard.hpp"

class LightVertOutputStandard : public VertOutputStandard
{
public:
    ~LightVertOutputStandard() {}
    Vector3f lightCentor;
};