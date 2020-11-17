#pragma once
#include "SystemBase.h"
#include "Components/MovementComponents.h"
#include "Components/PlayerComponents.h"

class InputMoveProcessingSystem final : public SystemBase<PlayerTag, MoveDirectionData, ViewDirectionData>
{
public:
    void OnCreate() override;
};
