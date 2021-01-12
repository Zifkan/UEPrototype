#include "MovementVelocitySystem.h"

void MovementVelocitySystem::OnCreate()
{
    SystemRun->each([this](flecs::entity e, MoveDirectionData& moveData, MovementSpeed& movementSpeed, MovementVelocity& velocity)
    {        
        velocity.Value = movementSpeed.Speed *  GetDeltaTime() * moveData.Axises;

        UE_LOG(LogTemp, Warning, TEXT("velocity.Value : x = %f y = %f"),movementSpeed.Speed,velocity.Value.Y);
    });
}
