#include "InputMoveProcessingSystem.h"

#include "Engine/Engine.h"

void InputMoveProcessingSystem::OnCreate()
{
    SystemRun->each([this](flecs::entity e, PlayerInputComponent& input)
    {
       

    });
}
