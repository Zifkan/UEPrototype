#pragma once
#include "Systems/SystemBase.h"
#include "Components/StateMachine/CheckActionTag.h"


class CheckActionAvailableSystem final :public SystemBase<CheckActionData>
{
public:
    virtual void OnCreate() override;
};
