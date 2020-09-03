#pragma once
#include "SystemBase.h"
#include "UEPrototype/ECS/Components/InputComponent.h"

class InputMoveProcessingSystem final : public SystemBase<PlayerInputComponent>
{
public:
    void OnCreate() override;
};
