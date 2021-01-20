﻿#pragma once
#include "SystemBase.h"
#include "Components/MovementComponents.h"
#include "Components/PlayerComponents.h"

class InputMoveProcessingSystem final : public SystemBase< MoveDirectionData, ViewDirectionData>
{
public:
    InputMoveProcessingSystem() : SystemBase("InputMoveProcessingSystem"){}
   
    virtual void OnCreate() override;
};
