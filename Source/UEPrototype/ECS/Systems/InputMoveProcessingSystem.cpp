#include "InputMoveProcessingSystem.h"

#include "Engine/Engine.h"

void InputMoveProcessingSystem::OnCreate()
{
    SystemRun->each([this](flecs::entity e, PlayerInputComponent& input)
    {
        UE_LOG(LogTemp, Warning, TEXT("Some warning message %f"), input.MovementAxis.X);

    });
}
