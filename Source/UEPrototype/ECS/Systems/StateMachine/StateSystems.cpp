#include "StateSystems.h"
#include "Components/GeneralComponents.h"
#include "Components/CharacterActions/CharacterActionsComponents.h"


void MovementStateSystem::OnCreate()
{
    SystemRun->signature("CASE | MoveState").each([this](flecs::entity e, AnimComponent& animComponent, MoveDirectionData& moveDirectionData,MovementSpeed& movementSpeed,FActorComponent actor)
    {
    	auto dir = moveDirectionData.Axises - actor.ptr->GetActorForwardVector();
        movementSpeed.Speed = 1000.0f;  


    	//TODO If camera focus lock on target for player or enemy just focusing on target should use Inputs direction to set anim move direction  
        animComponent.AnimInstancePtr->MoveDirection = FVector2D( 0,movementSpeed.SpeedModifier>1?1:0.5f);
  
    	SetActionAvailable<MoveInputTag>(e,{ 0.0,0.0,0.0f});		
    	
    }).kind(0);
}

void AttackStateSystem::OnCreate()
{
	SystemRun->signature("CASE | AttackState").each([this](flecs::entity e, AnimComponent& animComponent,MovementSpeed& movementSpeed)
	{
	    animComponent.AnimInstancePtr->PlayAnim();
        movementSpeed.Speed = 0.0f;

		SetActionAvailable<AttackInputTag>(e,{ FPlatformTime::Seconds(),1.66f,0.1f});		
		
		e.remove_case<AttackState>();
	}).kind(0);
}

void BlockingStateSystem::OnCreate()
{
	SystemRun->signature("CASE | AttackState").each([this](flecs::entity e, AnimComponent& animComponent)
    {
        animComponent.AnimInstancePtr->IsBlocking = true;
       
		
        for (auto children : e.children())
        {
            for (auto i : children)
            {
                if(children.entity(i).has<AttackInputTag>())
                {
                    children.entity(i).remove<ActionAvailableTag>();
                    children.entity(i).set<CheckActionData>({ FPlatformTime::Seconds(),1.66f,0.1f});
                }
            }
        }
		
        e.remove_case<AttackState>();
    }).kind(0);
}
