#include "MovementStateSystem.h"

void MovementStateSystem::OnCreate()
{
    SystemRun->signature("CASE | MoveState").iter([](flecs::iter it)
    {
        UE_LOG(LogTemp, Warning, TEXT("Move Working"));
    });
}
