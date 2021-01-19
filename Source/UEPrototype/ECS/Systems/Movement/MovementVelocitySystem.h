#pragma once

#include "Components/MovementComponents.h"
#include "Systems/SystemBase.h"

class MovementVelocitySystem final :public SystemBase<MoveDirectionData,MovementSpeed, MovementVelocity>
{
public:
    MovementVelocitySystem() : SystemBase("MovementVelocitySystem"){}
   
    virtual void OnCreate() override;
};
