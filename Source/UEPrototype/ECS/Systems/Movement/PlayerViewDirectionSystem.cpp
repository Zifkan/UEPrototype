#include "PlayerViewDirectionSystem.h"
#include "Components/CamComponent.h"
#include "Components/MovementComponents.h"
#include "Components/TransformComponents.h"

void PlayerViewDirectionSystem::OnCreate()
{
    SystemRun->signature("PlayerTag").each([this](Entity e, ViewDirectionData& viewDirection)
    {
        const auto s = m_pWorld->DefaultWorld->singleton<CamEntityType>();
        auto* camLocalToWorld = s.get<WorldToLocal>();

        if (camLocalToWorld ==nullptr) return;


    	auto forward = camLocalToWorld->Forward();
        auto right = camLocalToWorld->Right();
 
        //project forward and right vectors on the horizontal plane (y = 0)
        forward.Z = 0.f;
        right.Z = 0.f;    
     
    	forward = forward.ProjectOnToNormal(FVector::UpVector);   
    	forward.Normalize();
	
        //this is the direction in the world space we want to move:
        auto desiredMoveDirection = forward  + right;



    	
         viewDirection.Axises = desiredMoveDirection;       
       
    }).kind(0);    
}
