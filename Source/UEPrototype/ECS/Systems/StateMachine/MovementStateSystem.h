#pragma once
#include "Components/AnimComponent.h"
#include "Components/MovementComponents.h"
#include "Components/CharacterStates/CharacterStates.h"
#include "Systems/SystemBase.h"

class MovementStateSystem final :public SystemBase<AnimComponent,MoveDirectionData,MovementSpeed>
{
public:
    virtual void OnCreate() override;
};


class AttackStateSystem final :public SystemBase<AnimComponent,MovementSpeed>
{
    public:
    virtual void OnCreate() override;
};