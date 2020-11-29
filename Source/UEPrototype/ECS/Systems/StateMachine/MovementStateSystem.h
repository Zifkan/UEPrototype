#pragma once
#include "Systems/SystemBase.h"

class MovementStateSystem final :public SystemBase<>
{
public:
    virtual void OnCreate() override;
};
