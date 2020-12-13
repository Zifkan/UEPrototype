#include "MovementStateSystem.h"

void MovementStateSystem::OnCreate()
{
    SystemRun->signature("CASE | MoveState").each([](flecs::entity e, AnimComponent& animComponent, MoveDirectionData& moveDirectionData)
    {
      //  UE_LOG(LogTemp, Warning, TEXT("Move Working"));

        animComponent.AnimInstancePtr->MoveDirection = FVector2D(moveDirectionData.Axises);
    });
}
