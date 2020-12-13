#pragma once
#include "Components/AnimComponent.h"
#include "Components/MovementComponents.h"
#include "Systems/SystemBase.h"

class MovementStateSystem final :public SystemBase<AnimComponent,MoveDirectionData>
{
public:
    virtual void OnCreate() override;
};
