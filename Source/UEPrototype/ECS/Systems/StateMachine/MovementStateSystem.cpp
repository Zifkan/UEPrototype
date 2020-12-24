#include "MovementStateSystem.h"

void MovementStateSystem::OnCreate()
{
    SystemRun->signature("CASE | MoveState").each([](flecs::entity e, AnimComponent& animComponent, MoveDirectionData& moveDirectionData,MovementSpeed& movementSpeed)
    {
      //  UE_LOG(LogTemp, Warning, TEXT("Move Working"));

        movementSpeed.Speed = 2000.0f;
        animComponent.AnimInstancePtr->MoveDirection = FVector2D(moveDirectionData.Axises);
    });
}

void AttackStateSystem::OnCreate()
{
   SystemRun->signature("CASE | AttackState").each([](flecs::entity e, AnimComponent& animComponent,MovementSpeed& movementSpeed)
  {
      UE_LOG(LogTemp, Warning, TEXT("AttackState Working"));

      animComponent.AnimInstancePtr->PlayAnim();
       movementSpeed.Speed = 0.0f;
     /*   e.
       e.set<CheckActionData>({ });        */
       
      e.remove_case<AttackState>();
  });
}
