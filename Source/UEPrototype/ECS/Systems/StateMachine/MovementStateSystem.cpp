#include "MovementStateSystem.h"

#include "Components/GeneralComponents.h"

void MovementStateSystem::OnCreate()
{
    SystemRun->signature("CASE | MoveState").each([](flecs::entity e, AnimComponent& animComponent, MoveDirectionData& moveDirectionData,MovementSpeed& movementSpeed,FActorComponent actor)
    {
        UE_LOG(LogTemp, Warning, TEXT("MoveState Working"));
        movementSpeed.Speed = 2000.0f;

       
        
        animComponent.AnimInstancePtr->MoveDirection = FVector2D( actor.ptr->GetActorForwardVector()-moveDirectionData.Axises);
        e.set<CheckActionData>({ 0,0.0f,0.0f});
        
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
