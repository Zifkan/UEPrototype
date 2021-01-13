#include "MovementStateSystem.h"

#include "Components/GeneralComponents.h"
#include "Components/CharacterActions/CharacterActionsComponents.h"

void MovementStateSystem::OnCreate()
{
    SystemRun->signature("CASE | MoveState").each([](flecs::entity e, AnimComponent& animComponent, MoveDirectionData& moveDirectionData,MovementSpeed& movementSpeed,FActorComponent actor)
    {       
        movementSpeed.Speed = 2000.0f;              
        animComponent.AnimInstancePtr->MoveDirection = FVector2D( actor.ptr->GetActorForwardVector()-moveDirectionData.Axises);
     
    	for (auto children : e.children())
    	{
            for (auto i : children)
            {
                if(children.entity(i).has<MoveInputTag>())
                {
                	children.entity(i).remove<ActionAvailableTag>();
                    children.entity(i).set<CheckActionData>({ 0,0.0f,0.0f});
                }
            }
        }
    });
}

void AttackStateSystem::OnCreate()
{
	SystemRun->signature("CASE | AttackState").each([](flecs::entity e, AnimComponent& animComponent,MovementSpeed& movementSpeed)
	{
	    animComponent.AnimInstancePtr->PlayAnim();
        movementSpeed.Speed = 0.0f;

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
	});
}
