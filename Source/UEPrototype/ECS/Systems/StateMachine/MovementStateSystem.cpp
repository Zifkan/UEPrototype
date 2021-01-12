#include "MovementStateSystem.h"

void MovementStateSystem::OnCreate()
{
    SystemRun->signature("CASE | MoveState").each([](flecs::entity e, AnimComponent& animComponent, MoveDirectionData& moveDirectionData,MovementSpeed& movementSpeed)
    {    
        movementSpeed.Speed = 2000.0f;
        animComponent.AnimInstancePtr->MoveDirection = FVector2D(moveDirectionData.Axises);
        e.set<CheckActionData>({ FPlatformTime::Seconds(),0.0f,0.1f});
    });
}

void AttackStateSystem::OnCreate()
{
   SystemRun->signature("CASE | AttackState").each([](flecs::entity e, AnimComponent& animComponent,MovementSpeed& movementSpeed)
  {
      UE_LOG(LogTemp, Warning, TEXT("AttackState Working Ent ID : %i"),e.id());

      animComponent.AnimInstancePtr->PlayAnim();
      movementSpeed.Speed = 0.0f;
    
       e.set<CheckActionData>({ FPlatformTime::Seconds(),1.66f,0.1f});

       e.remove_case<AttackState>();
  });
}
