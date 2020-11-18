#pragma once

#include "Components/MovementComponents.h"
#include "Components/TransformComponents.h"
#include "Systems/SystemBase.h"

class MovementCharacterSystem final : public SystemBase<MovementVelocity,Translation,Rotation>
{
public:
    virtual void OnCreate() override;
private:   
    FQuat lastRot;
};